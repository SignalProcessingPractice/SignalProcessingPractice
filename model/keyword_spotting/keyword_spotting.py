"""torchaudio を使った簡易的なキーワード識別 (keyword spotting) スクリプト.

torchaudio 標準の SpeechCommands データセット (35 単語) を使い、
小さな 1D-CNN (M5) を学習してキーワードを分類する。

使い方:
    # 学習 (初回はデータセット約 2.3GB を自動ダウンロード)
    python keyword_spotting.py train --epochs 10 --out keyword_spotting.pt

    # 学習済みモデルで wav ファイルを識別
    python keyword_spotting.py predict --model keyword_spotting.pt --audio sample.wav

    # テストセットで精度を評価
    python keyword_spotting.py evaluate --model keyword_spotting.pt

    # マイク入力で連続認識 (要: pip install sounddevice, sudo apt install libportaudio2)
    python keyword_spotting.py listen --model keyword_spotting.pt
    python keyword_spotting.py listen --once          # Enter で 1 回録音
    python keyword_spotting.py listen --list-devices  # 入力デバイス確認
"""

from __future__ import annotations

import argparse
import os

import torch
import torch.nn as nn
import torch.nn.functional as F
import torchaudio
from torch.utils.data import DataLoader

SAMPLE_RATE = 16000       # SpeechCommands の元サンプリングレート
RESAMPLE_RATE = 8000      # 計算量削減のためダウンサンプリング
DATASET_ROOT = "./data"


# --------------------------------------------------------------------------- #
# データセット
# --------------------------------------------------------------------------- #
class SubsetSC(torchaudio.datasets.SPEECHCOMMANDS):
    """SpeechCommands を training / validation / testing に分割して読み込む。"""

    def __init__(self, subset: str):
        os.makedirs(DATASET_ROOT, exist_ok=True)  # torchaudio は root を自動作成しない
        super().__init__(DATASET_ROOT, download=True)

        def load_list(filename):
            filepath = os.path.join(self._path, filename)
            with open(filepath) as fileobj:
                return [os.path.join(self._path, line.strip()) for line in fileobj]

        if subset == "validation":
            self._walker = load_list("validation_list.txt")
        elif subset == "testing":
            self._walker = load_list("testing_list.txt")
        elif subset == "training":
            excludes = set(load_list("validation_list.txt") + load_list("testing_list.txt"))
            self._walker = [w for w in self._walker if w not in excludes]
        else:
            raise ValueError(f"unknown subset: {subset}")


def build_label_table(dataset) -> list[str]:
    """データセットに含まれるラベル一覧をソートして返す。"""
    return sorted({datapoint[2] for datapoint in dataset})


# --------------------------------------------------------------------------- #
# モデル (M5: Very Deep Convolutional Networks for Raw Waveforms)
# --------------------------------------------------------------------------- #
class M5(nn.Module):
    def __init__(self, n_input: int = 1, n_output: int = 35, stride: int = 16, n_channel: int = 32):
        super().__init__()
        self.conv1 = nn.Conv1d(n_input, n_channel, kernel_size=80, stride=stride)
        self.bn1 = nn.BatchNorm1d(n_channel)
        self.pool1 = nn.MaxPool1d(4)
        self.conv2 = nn.Conv1d(n_channel, n_channel, kernel_size=3)
        self.bn2 = nn.BatchNorm1d(n_channel)
        self.pool2 = nn.MaxPool1d(4)
        self.conv3 = nn.Conv1d(n_channel, 2 * n_channel, kernel_size=3)
        self.bn3 = nn.BatchNorm1d(2 * n_channel)
        self.pool3 = nn.MaxPool1d(4)
        self.conv4 = nn.Conv1d(2 * n_channel, 2 * n_channel, kernel_size=3)
        self.bn4 = nn.BatchNorm1d(2 * n_channel)
        self.pool4 = nn.MaxPool1d(4)
        self.fc1 = nn.Linear(2 * n_channel, n_output)

    def forward(self, x):
        x = self.pool1(F.relu(self.bn1(self.conv1(x))))
        x = self.pool2(F.relu(self.bn2(self.conv2(x))))
        x = self.pool3(F.relu(self.bn3(self.conv3(x))))
        x = self.pool4(F.relu(self.bn4(self.conv4(x))))
        x = F.adaptive_avg_pool1d(x, 1)  # 時間軸全体を平均 -> (B, C, 1)
        x = torch.flatten(x, 1)          # (B, C)
        x = self.fc1(x)                  # (B, n_output)  -- 2D Gemm
        return F.log_softmax(x, dim=1)   # 2D・axis=1 (組込ランタイム互換のため)


# --------------------------------------------------------------------------- #
# 前処理ユーティリティ
# --------------------------------------------------------------------------- #
def make_collate_fn(labels: list[str], resampler: nn.Module):
    label_to_index = {label: i for i, label in enumerate(labels)}

    def pad_sequence(batch):
        batch = [item.t() for item in batch]
        batch = nn.utils.rnn.pad_sequence(batch, batch_first=True, padding_value=0.0)
        return batch.permute(0, 2, 1)

    def collate_fn(batch):
        tensors, targets = [], []
        for waveform, _sr, label, *_ in batch:
            tensors.append(resampler(waveform))
            targets.append(label_to_index[label])
        return pad_sequence(tensors), torch.tensor(targets)

    return collate_fn


def load_waveform(path: str) -> torch.Tensor:
    waveform, sr = torchaudio.load(path)
    if waveform.shape[0] > 1:  # ステレオ -> モノラル
        waveform = waveform.mean(dim=0, keepdim=True)
    if sr != SAMPLE_RATE:
        waveform = torchaudio.transforms.Resample(sr, SAMPLE_RATE)(waveform)
    return waveform


def load_model(path: str, device: torch.device):
    """チェックポイントからモデルとラベル表を復元する。"""
    ckpt = torch.load(path, map_location=device)
    labels = ckpt["labels"]
    model = M5(n_output=len(labels)).to(device)
    model.load_state_dict(ckpt["model_state"])
    model.eval()
    return model, labels


@torch.no_grad()
def classify(model, labels, waveform_16k: torch.Tensor, device: torch.device, topk: int = 3):
    """16kHz モノラル波形 (1, T) を分類し、[(label, prob), ...] を返す。"""
    x = torchaudio.transforms.Resample(SAMPLE_RATE, RESAMPLE_RATE)(waveform_16k)
    x = x.unsqueeze(0).to(device)  # (1, 1, T)
    probs = model(x).squeeze().exp()
    k = min(topk, len(labels))
    values, indices = probs.topk(k)
    return [(labels[i], v) for v, i in zip(values.tolist(), indices.tolist())]


# --------------------------------------------------------------------------- #
# 学習 / 評価
# --------------------------------------------------------------------------- #
def get_device() -> torch.device:
    if torch.cuda.is_available():
        return torch.device("cuda")
    if torch.backends.mps.is_available():
        return torch.device("mps")
    return torch.device("cpu")


def train(args):
    device = get_device()
    print(f"device: {device}")

    train_set = SubsetSC("training")
    val_set = SubsetSC("validation")
    labels = build_label_table(train_set)
    print(f"{len(labels)} labels: {labels}")

    resampler = torchaudio.transforms.Resample(SAMPLE_RATE, RESAMPLE_RATE)
    collate_fn = make_collate_fn(labels, resampler)

    pin = device.type == "cuda"
    train_loader = DataLoader(
        train_set, batch_size=args.batch_size, shuffle=True,
        collate_fn=collate_fn, num_workers=args.workers, pin_memory=pin, drop_last=True,
    )
    val_loader = DataLoader(
        val_set, batch_size=args.batch_size, shuffle=False,
        collate_fn=collate_fn, num_workers=args.workers, pin_memory=pin,
    )

    model = M5(n_output=len(labels)).to(device)
    n_params = sum(p.numel() for p in model.parameters())
    print(f"model parameters: {n_params}")

    optimizer = torch.optim.Adam(model.parameters(), lr=args.lr, weight_decay=1e-4)
    scheduler = torch.optim.lr_scheduler.StepLR(optimizer, step_size=max(1, args.epochs // 2), gamma=0.1)

    best_acc = 0.0
    for epoch in range(1, args.epochs + 1):
        model.train()
        running_loss = 0.0
        for i, (data, target) in enumerate(train_loader):
            data, target = data.to(device), target.to(device)
            output = model(data)
            loss = F.nll_loss(output.squeeze(1), target)
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            running_loss += loss.item()
            if i % 50 == 0:
                print(f"epoch {epoch} [{i * len(data)}/{len(train_loader.dataset)}] loss={loss.item():.4f}")
        scheduler.step()

        acc = _evaluate(model, val_loader, device)
        print(f"=> epoch {epoch}: train_loss={running_loss / len(train_loader):.4f}  val_acc={acc:.4f}")

        if acc > best_acc:
            best_acc = acc
            torch.save({"model_state": model.state_dict(), "labels": labels}, args.out)
            print(f"   saved best model to {args.out} (acc={acc:.4f})")

    print(f"done. best val_acc={best_acc:.4f}")


@torch.no_grad()
def _evaluate(model, loader, device) -> float:
    model.eval()
    correct = 0
    total = 0
    for data, target in loader:
        data, target = data.to(device), target.to(device)
        pred = model(data).squeeze(1).argmax(dim=-1)
        correct += (pred == target).sum().item()
        total += target.size(0)
    return correct / total if total else 0.0


def evaluate(args):
    device = get_device()
    model, labels = load_model(args.model, device)

    test_set = SubsetSC("testing")
    resampler = torchaudio.transforms.Resample(SAMPLE_RATE, RESAMPLE_RATE)
    collate_fn = make_collate_fn(labels, resampler)
    test_loader = DataLoader(
        test_set, batch_size=args.batch_size, shuffle=False,
        collate_fn=collate_fn, num_workers=args.workers,
    )
    acc = _evaluate(model, test_loader, device)
    print(f"test accuracy: {acc:.4f} ({len(test_set)} samples)")


def predict(args):
    device = get_device()
    model, labels = load_model(args.model, device)

    waveform = load_waveform(args.audio)
    results = classify(model, labels, waveform, device, args.topk)

    print(f"input: {args.audio}")
    for rank, (label, prob) in enumerate(results, 1):
        print(f"  {rank}. {label:<12} {prob:.3f}")


# --------------------------------------------------------------------------- #
# マイク入力
# --------------------------------------------------------------------------- #
def _import_sounddevice():
    try:
        import sounddevice as sd
        return sd
    except OSError as e:  # PortAudio 本体が無い
        raise SystemExit(
            "PortAudio ライブラリが見つかりません。\n"
            "  Ubuntu/WSL: sudo apt install libportaudio2\n"
            f"  詳細: {e}"
        )
    except ModuleNotFoundError:
        raise SystemExit("sounddevice が未インストールです:  pip install sounddevice")


def record(seconds: float, sd) -> torch.Tensor:
    """マイクから録音し、16kHz モノラルの (1, T) テンソルを返す。"""
    frames = int(seconds * SAMPLE_RATE)
    audio = sd.rec(frames, samplerate=SAMPLE_RATE, channels=1, dtype="float32")
    sd.wait()
    return torch.from_numpy(audio.T).contiguous()  # (1, T)


def listen(args):
    sd = _import_sounddevice()

    if args.list_devices:
        print(sd.query_devices())
        return
    if args.device is not None:
        sd.default.device = (args.device, None)

    device = get_device()
    model, labels = load_model(args.model, device)
    print(f"device: {device} / model: {args.model}")
    print(f"入力デバイス: {sd.query_devices(sd.default.device[0])['name']}")

    if args.once:
        input(f"Enter で {args.seconds}s 録音 > ")
        wav = record(args.seconds, sd)
        _report(classify(model, labels, wav, device, args.topk), wav)
        return

    print(f"{args.seconds}s 窓で連続認識中。Ctrl+C で終了。\n")
    try:
        while True:
            wav = record(args.seconds, sd)
            rms = wav.pow(2).mean().sqrt().item()
            if rms < args.threshold:  # 無音はスキップ
                continue
            results = classify(model, labels, wav, device, args.topk)
            if results[0][1] < args.min_prob:
                continue
            _report(results, wav, rms)
    except KeyboardInterrupt:
        print("\n終了しました。")


def _report(results, wav: torch.Tensor, rms: float | None = None):
    rms = rms if rms is not None else wav.pow(2).mean().sqrt().item()
    top = " / ".join(f"{label} {prob:.2f}" for label, prob in results)
    print(f"[rms={rms:.3f}]  {top}")


# --------------------------------------------------------------------------- #
# CLI
# --------------------------------------------------------------------------- #
def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    p_train = sub.add_parser("train", help="モデルを学習する")
    p_train.add_argument("--epochs", type=int, default=10)
    p_train.add_argument("--batch-size", type=int, default=256)
    p_train.add_argument("--lr", type=float, default=1e-2)
    p_train.add_argument("--workers", type=int, default=os.cpu_count() or 2)
    p_train.add_argument("--out", type=str, default="keyword_spotting.pt")
    p_train.set_defaults(func=train)

    p_eval = sub.add_parser("evaluate", help="テストセットで精度を測る")
    p_eval.add_argument("--model", type=str, default="keyword_spotting.pt")
    p_eval.add_argument("--batch-size", type=int, default=256)
    p_eval.add_argument("--workers", type=int, default=os.cpu_count() or 2)
    p_eval.set_defaults(func=evaluate)

    p_pred = sub.add_parser("predict", help="wav ファイルのキーワードを識別する")
    p_pred.add_argument("--model", type=str, default="keyword_spotting.pt")
    p_pred.add_argument("--audio", type=str, required=True)
    p_pred.add_argument("--topk", type=int, default=3)
    p_pred.set_defaults(func=predict)

    p_listen = sub.add_parser("listen", help="マイク入力でキーワードを識別する")
    p_listen.add_argument("--model", type=str, default="keyword_spotting.pt")
    p_listen.add_argument("--seconds", type=float, default=1.0, help="録音窓の長さ (秒)")
    p_listen.add_argument("--topk", type=int, default=3)
    p_listen.add_argument("--once", action="store_true", help="1回だけ録音して終了")
    p_listen.add_argument("--threshold", type=float, default=0.02, help="無音判定の RMS しきい値")
    p_listen.add_argument("--min-prob", type=float, default=0.5, help="この確率未満の予測は表示しない")
    p_listen.add_argument("--device", type=int, default=None, help="入力デバイス番号")
    p_listen.add_argument("--list-devices", action="store_true", help="デバイス一覧を表示して終了")
    p_listen.set_defaults(func=listen)

    args = parser.parse_args(argv)
    args.func(args)


if __name__ == "__main__":
    main()
