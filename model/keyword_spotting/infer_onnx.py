"""export_onnx.py で出力した keyword_spotting.onnx を使ってキーワード識別を行う。

PyTorch/torchaudio に依存せず、onnxruntime + numpy だけで動く軽量推論スクリプト。

使い方:
    # wav ファイルを識別
    python infer_onnx.py predict --model keyword_spotting.onnx --audio sample.wav

    # マイク入力で連続識別 (要: pip install sounddevice, sudo apt install libportaudio2)
    python infer_onnx.py listen --model keyword_spotting.onnx
    python infer_onnx.py listen --once

前提:
    keyword_spotting.onnx の入力 "waveform_8k" は 8kHz モノラルの (batch, 1, num_samples) float32。
    16kHz など他のレートからの変換はこのスクリプトが行う。
    出力 "logprobs" は (batch, 35) の log-softmax 値。
"""

from __future__ import annotations

import argparse
import json
import wave

import numpy as np
import onnxruntime as ort

TARGET_RATE = 8000  # keyword_spotting.onnx が期待するサンプリングレート


# --------------------------------------------------------------------------- #
# 音声入出力 (torch 非依存)
# --------------------------------------------------------------------------- #
def load_wav(path: str) -> tuple[np.ndarray, int]:
    """PCM WAV を読み込み、[-1, 1] に正規化したモノラル float32 と sr を返す。"""
    with wave.open(path, "rb") as wf:
        n_channels = wf.getnchannels()
        sampwidth = wf.getsampwidth()
        sr = wf.getframerate()
        raw = wf.readframes(wf.getnframes())

    if sampwidth == 1:  # 8-bit unsigned
        data = (np.frombuffer(raw, dtype=np.uint8).astype(np.float32) - 128) / 128.0
    elif sampwidth == 2:  # 16-bit signed
        data = np.frombuffer(raw, dtype=np.int16).astype(np.float32) / 32768.0
    elif sampwidth == 4:  # 32-bit signed
        data = np.frombuffer(raw, dtype=np.int32).astype(np.float32) / 2147483648.0
    else:
        raise ValueError(f"未対応のサンプル幅: {sampwidth * 8} bit (16-bit PCM を推奨)")

    if n_channels > 1:  # インターリーブ -> モノラル平均
        data = data.reshape(-1, n_channels).mean(axis=1)
    return data, sr


def resample_linear(x: np.ndarray, sr_in: int, sr_out: int) -> np.ndarray:
    """線形補間による簡易リサンプル (キーワード識別には十分な品質)。"""
    if sr_in == sr_out:
        return x.astype(np.float32)
    n_out = int(round(len(x) * sr_out / sr_in))
    t_in = np.arange(len(x), dtype=np.float64)
    t_out = np.linspace(0, len(x) - 1, n_out, dtype=np.float64)
    return np.interp(t_out, t_in, x).astype(np.float32)


# --------------------------------------------------------------------------- #
# ONNX 推論
# --------------------------------------------------------------------------- #
class KeywordSpotter:
    def __init__(self, model_path: str):
        self.sess = ort.InferenceSession(model_path, providers=["CPUExecutionProvider"])
        self.input_name = self.sess.get_inputs()[0].name
        self.labels = _load_labels(model_path, self.sess)

    def predict(self, waveform: np.ndarray, sr: int, topk: int = 3) -> list[tuple[str, float]]:
        """モノラル波形 (任意レート) を分類し、[(label, prob), ...] を確率降順で返す。"""
        wav = resample_linear(np.asarray(waveform, dtype=np.float32), sr, TARGET_RATE)
        x = wav.reshape(1, 1, -1)  # (batch, channel, num_samples)
        logprobs = self.sess.run(["logprobs"], {self.input_name: x})[0][0]
        probs = np.exp(logprobs)
        order = np.argsort(probs)[::-1][:topk]
        return [(self.labels[i], float(probs[i])) for i in order]


def _load_labels(model_path: str, sess: ort.InferenceSession) -> list[str]:
    """ラベル一覧を ONNX メタデータ → サイドカー JSON → 連番 の順に探す。"""
    meta = sess.get_modelmeta().custom_metadata_map
    if "labels" in meta:
        return json.loads(meta["labels"])

    sidecar = model_path + ".labels.json"
    try:
        with open(sidecar, encoding="utf-8") as f:
            return json.load(f)
    except FileNotFoundError:
        pass

    n = sess.get_outputs()[0].shape[-1]
    n = n if isinstance(n, int) else 35
    print(f"警告: ラベル情報が見つかりません。index 0..{n - 1} を使います。")
    return [str(i) for i in range(n)]


# --------------------------------------------------------------------------- #
# コマンド
# --------------------------------------------------------------------------- #
def cmd_predict(args):
    spotter = KeywordSpotter(args.model)
    waveform, sr = load_wav(args.audio)
    results = spotter.predict(waveform, sr, args.topk)

    print(f"input: {args.audio}  ({sr} Hz, {len(waveform) / sr:.2f} s)")
    for rank, (label, prob) in enumerate(results, 1):
        print(f"  {rank}. {label:<12} {prob:.3f}")


def cmd_listen(args):
    try:
        import sounddevice as sd
    except OSError as e:
        raise SystemExit(f"PortAudio が見つかりません: sudo apt install libportaudio2\n  {e}")
    except ModuleNotFoundError:
        raise SystemExit("sounddevice が未インストールです: pip install sounddevice")

    if args.list_devices:
        print(sd.query_devices())
        return
    if args.device is not None:
        sd.default.device = (args.device, None)

    spotter = KeywordSpotter(args.model)
    rec_rate = 16000
    print(f"model: {args.model} / labels: {len(spotter.labels)}")
    print(f"入力デバイス: {sd.query_devices(sd.default.device[0])['name']}")

    def record() -> np.ndarray:
        frames = int(args.seconds * rec_rate)
        audio = sd.rec(frames, samplerate=rec_rate, channels=1, dtype="float32")
        sd.wait()
        return audio[:, 0]

    if args.once:
        input(f"Enter で {args.seconds}s 録音 > ")
        _report(spotter.predict(record(), rec_rate, args.topk))
        return

    print(f"{args.seconds}s 窓で連続認識中。Ctrl+C で終了。\n")
    try:
        while True:
            wav = record()
            rms = float(np.sqrt(np.mean(wav ** 2)))
            if rms < args.threshold:
                continue
            results = spotter.predict(wav, rec_rate, args.topk)
            if results[0][1] >= args.min_prob:
                _report(results, rms)
    except KeyboardInterrupt:
        print("\n終了しました。")


def _report(results: list[tuple[str, float]], rms: float | None = None):
    head = f"[rms={rms:.3f}]  " if rms is not None else ""
    print(head + " / ".join(f"{label} {prob:.2f}" for label, prob in results))


# --------------------------------------------------------------------------- #
# CLI
# --------------------------------------------------------------------------- #
def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    p_pred = sub.add_parser("predict", help="wav ファイルを識別する")
    p_pred.add_argument("--model", default="keyword_spotting.onnx")
    p_pred.add_argument("--audio", required=True)
    p_pred.add_argument("--topk", type=int, default=3)
    p_pred.set_defaults(func=cmd_predict)

    p_listen = sub.add_parser("listen", help="マイク入力で識別する")
    p_listen.add_argument("--model", default="keyword_spotting.onnx")
    p_listen.add_argument("--seconds", type=float, default=1.0)
    p_listen.add_argument("--topk", type=int, default=3)
    p_listen.add_argument("--once", action="store_true")
    p_listen.add_argument("--threshold", type=float, default=0.02, help="無音判定の RMS しきい値")
    p_listen.add_argument("--min-prob", type=float, default=0.5, help="表示する最低確率")
    p_listen.add_argument("--device", type=int, default=None)
    p_listen.add_argument("--list-devices", action="store_true")
    p_listen.set_defaults(func=cmd_listen)

    args = parser.parse_args(argv)
    args.func(args)


if __name__ == "__main__":
    main()
