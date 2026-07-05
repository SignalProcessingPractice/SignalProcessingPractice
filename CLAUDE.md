# CLAUDE.md

このファイルは、このリポジトリ内のコードを扱う際に、Claude Code（claude.ai/code）へ指針を提供するものです。

## コーディングルール

生成する C++ コードは、必ず clang-format および clang-tidy に適合させること。

コードを提案する際は、以下を遵守すること。

1. clang-tidy の警告や指摘事項を考慮すること。
2. 新たな警告を発生させないこと。
3. 警告数を削減できる修正を優先すること。
4. 明示的な指示がない限り、NOLINT による警告抑制を行わないこと。

clang-tidy が警告を出力した場合、そのコードは不適合とみなし、ビルド失敗として扱う。

### 検査スクリプト

scripts/ 配下に、ユーザコード全域を対象に clang-format または clang-tidy を実行するスクリプトを用意しています。
このスクリプトの検査対象はユーザコードのみとします。サードパーティのソースコードは検査対象とはしません。

```bash
# clang_format 実行
python3 run_clang_format.py

# clang_tidy 実行 (検査のみ)
python3 run_clang_tidy.py

# clang_tidy 実行 (自動修正あり)
python3 run_clang_tidy.py --fix
```

## ビルドコマンド

`CMakePresets.json` には、以下の CMake Preset が定義されています。

```bash
# Windows アプリケーション（Clang、WASAPI オーディオ）
cmake --preset app-win
cmake --build build/app

# Linux アプリケーション（Clang、ALSA オーディオ）
cmake --preset app-linux
cmake --build build/app

# ユニットテスト（Clang + Google Test）
cmake --preset gtest-debug-clang
cmake --build build/gtest-clang

# 全テストの実行
ctest --test-dir build/gtest-clang

# テストバイナリを直接実行
./build/gtest-clang/TEST_FrameSyncProcess
```

## アーキテクチャ

本プロジェクトは、差し替え可能な DSP コンポーネントを実現するために **Strategy パターン** を採用した C++20 の音声信号処理パイプラインです。

### コア抽象

#### `AudioFrame` (`lib/inc/AudioFrame.hpp`)

音声サンプルを保持するテンプレートコンテナ `AudioFrameTemplate<NumSamples, SampleType>` です。サンプルレート情報を保持し、スタック上に確保される値型（Value Semantics）として設計されています。

以下の2種類の特殊化が定義されています。

- `AudioFrame` : 1024 サンプル（完全なフレーム）
- `AudioHop` : 512 サンプル（ホップサイズ／出力フレーム）

#### `FrameSyncProcess` (`lib/inc/FrameSyncProcess.hpp`)

差し替え可能な Strategy を保持するオーケストレータです。

1. `AudioAcquireStrategy` — 入力取得（512サンプルのホップ）
2. `PreProcessStrategy`
3. `OverlapStrategy` — `Overlapper` による 512→1024 サンプル変換
4. `WindowStrategy` — 窓関数の適用
5. `FftStrategy` — CMSIS-DSP による FFT
6. `InferStrategy` — モデル推論
7. `PostProcessStrategy` — CMSIS-DSP による IFFT
8. `OverlapAddStrategy` — 1024→512 サンプルの合成
9. `AudioOutputStrategy` — 出力ホップの送出

#### `PipelineContext` (`lib/src/core/pipeline/`)

Strategy の実体を所有する具体的なパイプラインです。`exec()` を呼び出すことで、処理チェーン全体を順番に実行します。

### Strategy 実装 (`lib/src/platform/common/`)

| ディレクトリ | Strategy |
|------------|------------|
| `acquire/` | `SineGenerator` — テスト用サイン波生成器 |
| `window/` | `HannWindow`, `RectangleWindow` |
| `overlap/` | `Overlapper` — 循環バッファによるオーバーラップ処理 |
| `fft/` | `FFT` — `arm_rfft_fast_f32` のラッパ |
| `postprocess/` | `IFFT` — `arm_rfft_fast_f32`（逆変換）のラッパ |
| `overlap_add/` | `HannOverlapAdder`, `RectangleOverlapAdder` |
| `shared_logic/` | テスト用の Null Strategy およびパススルー Strategy |

### GUI アプリ (`app/`)

Qt6 (Widgets) を使用した GUI アプリケーションを、**MVP（Model-View-Presenter）アーキテクチャ** に準拠する形式で開発中です。

- `app/view/` — View 層（Qt 依存はこの層に閉じ込める）
- `app/presenter/` — Presenter 層（View 抽象と Model の仲介）
- `app/model/` — Model 層（`FrameSyncProcess` のラップ、処理スレッド管理）

旧実装（RtAudio による CLI アプリ）は `app/_old/_main.cpp` に退避されています。旧実装は 48kHz / 256 サンプルバッファで動作し、入出力ストリーム間を **ロックフリーな SPSC リングバッファ**（atomic acquire/release による実装）で接続していました。この設計は Model 層実装時の参考とします。

### 依存ライブラリ（CMake FetchContent により取得）

- **RtAudio 6.0.1** — リアルタイム音声入出力（Windows では WASAPI を使用）
- **ETL 20.47.1** — Embedded Template Library（Strategy の実装に使用する `etl::delegate` を提供）
- **CMSIS-DSP v1.17.0** — FFT/IFFT に使用する ARM DSP ライブラリ
- **Google Test v1.17.0** — ユニットテストフレームワーク

## 主要な設計判断
- 組み込み環境にも対応したマルチプラットフォームライブラリの開発を目的とする
  - ヒープメモリ確保は禁止する
  - 大容量オブジェクトのスタック確保は避ける
  - メモリ使用量はコンパイル時に把握可能であること
  - 実行時のメモリフットプリントを予測可能にする
