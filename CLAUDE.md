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

## ビルドコマンド

`CMakePresets.json` には、2つの CMake Preset が定義されています。

```bash
# Windows アプリケーション（Clang、WASAPI オーディオ）
cmake --preset app-win
cmake --build build/app

# ユニットテスト（GCC + Google Test）
cmake --preset gtest-debug-gcc
cmake --build build/gtest-gcc

# 全テストの実行
ctest --test-dir build/gtest-gcc

# テストバイナリを直接実行
./build/gtest-gcc/TEST_FrameSyncProcess
```

`.clangd` の設定では、コンパイルデータベース（IDE の補完や解析機能用）として `build/app` を参照します。

## アーキテクチャ

本プロジェクトは、差し替え可能な DSP コンポーネントを実現するために **Strategy パターン** を採用した C++20 の音声信号処理パイプラインです。

### コア抽象

#### `AudioFrame` (`lib/inc/AudioFrame.hpp`)

音声サンプルを保持するテンプレートコンテナ `AudioFrameTemplate<NumSamples, SampleType>` です。サンプルレート情報を保持し、スタック上に確保される値型（Value Semantics）として設計されています。

以下の2種類の特殊化が定義されています。

- `AudioFrame` : 1024 サンプル（完全なフレーム）
- `AudioHop` : 512 サンプル（ホップサイズ／出力フレーム）

#### `FrameSyncProcess` (`lib/inc/FrameSyncProcess.hpp`)

差し替え可能な Strategy を保持する ETL Delegate スロットを持つオーケストレータです。

1. `AudioAquireStrategy` — 入力取得（512サンプルのホップ）
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

### リアルタイム音声アプリ (`app/main.cpp`)

Windows の WASAPI を使用し、48kHz / 256サンプルバッファで動作します。

入力ストリームと出力ストリームは RtAudio によって独立して管理されており、両者の間は **ロックフリーな SPSC（Single Producer Single Consumer）リングバッファ**（atomic acquire/release による実装）で接続されています。

専用の処理スレッドが入力バッファからデータを取得し、DSP パイプラインを実行した後に出力バッファへ書き込みます。

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

- `FrameSyncProcessConfig` はタグディスパッチを使用してパイプラインの動作を構成する
  - bool フラグではなく型タグによる設定を採用する
