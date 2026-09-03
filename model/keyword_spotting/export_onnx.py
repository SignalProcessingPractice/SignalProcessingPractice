"""keyword_spotting.py で学習したモデル (keyword_spotting.pt) を ONNX 形式に変換する。

出力先: ST Edge AI Core (v4.x) がインポート可能な形式。
参考: https://stedgeai-dc.st.com/assets/embedded-docs/supported_ops_onnx.html

使い方:
    python export_onnx.py --model keyword_spotting.pt --out keyword_spotting.onnx

    # 入力長 (秒) やバッチ、opset を変える
    python export_onnx.py --model keyword_spotting.pt --out keyword_spotting.onnx --seconds 1.0 --batch 1 --opset 17

ST Edge AI Core 向けに施している対応:
  * 入力・出力を **静的 shape** に固定 (ST は dynamic tensor 不可、可変バッチは 1 扱い)
  * opset を 7〜20 の範囲に固定 (既定 17)、ir_version を 10 以下に固定
  * TorchScript ベースのエクスポータを使用し constant folding で BatchNormalization を
    Conv に畳み込む → 生成される op は Conv / Relu / MaxPool / GlobalAveragePool /
    Flatten / Gemm / LogSoftmax のみ (いずれも ST サポート対象)
  * LogSoftmax は 2D テンソル・axis=1 (ST は axis=1 のみ、-1 不可)
  * 重みは .onnx に埋め込み (.onnx.data に分離しない)
  * 変換後、使用 op が ST サポート表に含まれるか / shape が静的かを自動チェック

入力仕様:
    入力 "waveform_8k" : (batch, 1, num_samples) float32、**8kHz にリサンプル済み** の波形。
                         16kHz -> 8kHz 変換は呼び出し側 (infer_onnx.py 等) で行う。
    出力 "logprobs"    : (batch, n_labels) の log-softmax 値。
"""

from __future__ import annotations

import argparse
import json
import warnings
from pathlib import Path

import torch

from keyword_spotting import RESAMPLE_RATE, load_model

# ST Edge AI Core 4.0 "ONNX toolbox support" に掲載されている演算子
# (https://stedgeai-dc.st.com/assets/embedded-docs/supported_ops_onnx.html)
ST_SUPPORTED_OPS = frozenset("""
Abs Acos Acosh Add And ArgMax ArgMin ArrayFeatureExtractor Asin Asinh Atan Atanh
AveragePool BatchNormalization BitShift Cast Ceil Clip Concat Constant ConstantOfShape
Conv ConvTranspose Cos Cosh DepthToSpace DequantizeLinear Div Dropout Einsum Elu Equal
Erf Exp Expand Flatten Floor Gather GatherElements GatherND Gelu Gemm GlobalAveragePool
GlobalMaxPool Greater GreaterOrEqual GRU Hardmax HardSigmoid HardSwish Identity
InPlaceAccumulatorV2 InstanceNormalization LabelEncoder LayerNormalization LeakyRelu Less
LessOrEqual LinearClassifier LinearRegressor Log LogSoftmax LpNormalization LRN LSTM
MatMul Max MaxPool Mean Min Mod Mul Neg Normalizer Not Or Pad Pow PRelu QLinearAdd
QLinearAveragePool QLinearConcat QLinearConv QLinearGlobalAveragePool QLinearMatMul
QLinearMul QuantizeLinear Range Reciprocal ReduceL1 ReduceL2 ReduceLogSumExp ReduceMax
ReduceMean ReduceMin ReduceProd ReduceSum ReduceSumSquare Relu Reshape Resize Round Scale
Scaler ScatterND Selu Shape Sigmoid Sign Sin Sinh Size Slice Softmax Softplus Softsign
SpaceToDepth Split Sqrt Squeeze Sub Sum SVMClassifier SVMRegressor Tan Tanh ThresholdedRelu
Tile TopK Transpose TreeEnsembleClassifier TreeEnsembleRegressor Unsqueeze Upsample Where
Xor ZipMap
""".split())


def parse_args(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--model", default="keyword_spotting.pt", help="入力チェックポイント (keyword_spotting.py train の出力)")
    p.add_argument("--out", default="keyword_spotting.onnx", help="出力 ONNX ファイル")
    p.add_argument("--seconds", type=float, default=1.0, help="固定する入力長 (秒, 8kHz 換算)")
    p.add_argument("--batch", type=int, default=1, help="固定するバッチサイズ (ST では実質 1)")
    p.add_argument("--opset", type=int, default=17, help="ONNX opset (7〜20)")
    p.add_argument("--ir-version", type=int, default=10, help="ONNX ir_version の上限 (ONNX 1.15 は 10)")
    p.add_argument("--no-check", action="store_true", help="onnxruntime 照合と ST 適合チェックをスキップ")
    return p.parse_args(argv)


def main(argv=None):
    args = parse_args(argv)
    if not 7 <= args.opset <= 20:
        raise SystemExit("--opset は 7〜20 の範囲で指定してください (ST Edge AI Core の対応範囲)。")

    device = torch.device("cpu")
    model, labels = load_model(args.model, device)
    model.eval()
    print(f"loaded {args.model}: {len(labels)} labels")

    num_samples = int(args.seconds * RESAMPLE_RATE)
    if num_samples < 512:
        raise SystemExit(f"--seconds が短すぎます (num_samples={num_samples})。0.5 以上を推奨。")

    out_path = Path(args.out)
    dummy = torch.randn(args.batch, 1, num_samples, dtype=torch.float32)

    # TorchScript ベースのエクスポータ (dynamo=False) を意図的に使用する。
    # torch.export ベース (既定) より生成グラフが素直で、BatchNormalization が
    # Conv に畳み込まれ、ReduceMean/Reshape 等の余分な op が出ないため ST 向き。
    # dynamic_axes を渡さないことで入出力 shape を静的に固定する。
    with warnings.catch_warnings():
        warnings.filterwarnings("ignore", message=".*legacy TorchScript-based ONNX export.*")
        torch.onnx.export(
            model, dummy, str(out_path),
            input_names=["waveform_8k"], output_names=["logprobs"],
            opset_version=args.opset, do_constant_folding=True, dynamo=False,
        )
    _cleanup_external_data(out_path)
    print(f"exported -> {out_path}  (input={tuple(dummy.shape)}, opset={args.opset})")

    _postprocess(out_path, labels, args.ir_version)

    sidecar = out_path.with_suffix(out_path.suffix + ".labels.json")
    sidecar.write_text(json.dumps(labels, ensure_ascii=False, indent=2))
    print(f"labels   -> {sidecar}")

    if not args.no_check:
        _check_st_compat(out_path)
        _verify(model, out_path, args.batch, num_samples)


def _cleanup_external_data(out_path: Path):
    stale = Path(str(out_path) + ".data")
    if stale.exists():
        stale.unlink()


def _postprocess(out_path: Path, labels: list[str], ir_version_cap: int):
    """ir_version を丸め、ラベルを metadata に埋め込み、単一ファイルで保存し直す。"""
    import onnx

    m = onnx.load(str(out_path))

    if m.ir_version > ir_version_cap:
        print(f"ir_version {m.ir_version} -> {ir_version_cap} に変更")
        m.ir_version = ir_version_cap

    entry = m.metadata_props.add()
    entry.key = "labels"
    entry.value = json.dumps(labels, ensure_ascii=False)

    onnx.checker.check_model(m)
    onnx.save(m, str(out_path), save_as_external_data=False)
    print(f"metadata -> labels ({len(labels)} 件) を埋め込み / ir_version={m.ir_version}")


def _check_st_compat(out_path: Path):
    """使用 op が ST サポート表に含まれるか、shape が静的かを検証する。"""
    import onnx

    m = onnx.load(str(out_path))
    g = m.graph

    used_ops = sorted({n.op_type for n in g.node})
    unsupported = [op for op in used_ops if op not in ST_SUPPORTED_OPS]
    print(f"使用 op: {', '.join(used_ops)}")
    if unsupported:
        raise SystemExit(f"ST 非対応の op が含まれます: {unsupported}")

    dynamic = []
    tensors = list(g.input) + list(g.output) + list(g.value_info)
    for t in tensors:
        dims = t.type.tensor_type.shape.dim
        shape = [d.dim_value if d.HasField("dim_value") else (d.dim_param or "?") for d in dims]
        if any(not isinstance(s, int) or s <= 0 for s in shape):
            dynamic.append((t.name, shape))
    if dynamic:
        raise SystemExit(f"動的 shape のテンソルがあります (ST は非対応): {dynamic}")

    # LogSoftmax / Softmax は axis=1 のみ ST 対応
    for n in g.node:
        if n.op_type in ("LogSoftmax", "Softmax"):
            axis = next((a.i for a in n.attribute if a.name == "axis"), 1)
            if axis != 1:
                raise SystemExit(f"{n.op_type} の axis={axis} は ST 非対応 (axis=1 のみ)。")

    in_shape = [d.dim_value for d in g.input[0].type.tensor_type.shape.dim]
    out_shape = [d.dim_value for d in g.output[0].type.tensor_type.shape.dim]
    print(f"ST 適合チェック OK  (静的 shape: 入力{in_shape} -> 出力{out_shape})")


def _verify(model: torch.nn.Module, out_path: Path, batch: int, num_samples: int):
    """固定 shape で PyTorch と onnxruntime の出力が一致するか確認する。"""
    import numpy as np
    import onnxruntime as ort

    sess = ort.InferenceSession(str(out_path), providers=["CPUExecutionProvider"])
    for seed in range(3):
        torch.manual_seed(seed)
        x = torch.randn(batch, 1, num_samples, dtype=torch.float32)
        with torch.no_grad():
            ref = model(x).numpy()
        got = sess.run(["logprobs"], {"waveform_8k": x.numpy()})[0]
        np.testing.assert_allclose(ref, got, rtol=1e-3, atol=1e-4)
        print(f"  check ok: seed={seed}  max|Δ|={np.abs(ref - got).max():.2e}")
    print("onnxruntime との出力照合 OK")


if __name__ == "__main__":
    main()
