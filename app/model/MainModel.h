///
/// @file MainModel.h
///
#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#include "FrameSyncProcess.hpp"
#include "Strategies/HannOverlapAdder.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/SineGenerator.hpp"
#include "common/PipelineSelection.h"
#include "model/AudioInputBuffer.h"
#include "model/AudioOutputBuffer.h"
#include "model/FilePlayer.h"
#include "model/InputSource.h"
#include "model/RingBufferAcquire.h"
#include "model/RingBufferOutput.h"

struct PipelineResult;
class DeviceInput;
class DeviceOutput;

///
/// @brief MVP の Model 層.
///
/// FrameSyncProcess と入力系 (Producer / リングバッファ) を所有し,
/// 「1 ホップ分溜まったら ProcessFrame() を実行する」処理スレッドを管理する.
///
class MainModel {
public:
    MainModel();
    ~MainModel();

    MainModel(const MainModel&) = delete;
    auto operator=(const MainModel&) -> MainModel& = delete;
    MainModel(MainModel&&) = delete;
    auto operator=(MainModel&&) -> MainModel& = delete;

    ///
    /// Producer / 処理スレッドを起動する.
    ///
    void Start();

    ///
    /// Producer / 処理スレッドを停止する (join 完了まで待機).
    ///
    void Stop();

    ///
    /// ComboBox の選択 (stage, index) に対応する Strategy を FrameSyncProcess へ設定する.
    ///
    /// index は GetStrategyNames(stage) の並びに対応する.
    /// kAcquire は Acquire Strategy ではなく Producer (InputSource / DeviceInput) を切り替える.
    /// "Device" 選択時はキャプチャを開始せず, ApplyDeviceSelection() を待つ.
    ///
    void ApplyStrategySelection(PipelineStage stage, int index);

    ///
    /// 利用可能な入力デバイス名の一覧を取得する.
    ///
    [[nodiscard]] static auto GetAudioInputDeviceNames() -> std::vector<std::string>;

    ///
    /// 指定 index の入力デバイスでキャプチャを開始する (入力が "Device" のときのみ有効).
    ///
    /// device_index は GetAudioInputDeviceNames() の並びに対応する.
    ///
    void ApplyDeviceSelection(int device_index);

    ///
    /// サイン波生成の周波数を設定する.
    ///
    /// Producer スレッドと排他して SineGenerator を更新する (スレッドセーフ).
    ///
    void ApplySineFrequency(int frequency_hz);

    ///
    /// 音声ファイル (WAV) を読み込む.
    ///
    /// Producer スレッドと排他して FilePlayer を更新する (スレッドセーフ).
    /// 読み込み失敗時は既存データ (未ロードなら無音) を維持する.
    ///
    void ApplyFileSelection(const std::string& path);

    ///
    /// 利用可能な出力デバイス名の一覧を取得する.
    ///
    [[nodiscard]] static auto GetAudioOutputDeviceNames() -> std::vector<std::string>;

    ///
    /// 指定 index の出力デバイスで再生を開始する (出力が "Device" のときのみ有効).
    ///
    /// device_index は GetAudioOutputDeviceNames() の並びに対応する.
    ///
    void ApplyOutputDeviceSelection(int device_index);

    ///
    /// FrameSyncProcess への参照を取得する.
    ///
    [[nodiscard]] auto Process() -> FrameSyncProcess&;

    ///
    /// 処理済みフレーム数を取得する (スレッドセーフ).
    ///
    [[nodiscard]] auto GetProcessedFrameCount() const -> std::uint64_t;

private:
    ///
    /// 処理スレッドのループ本体.
    ///
    void RunProcessing(const std::stop_token& stop_token);

    ///
    /// ProcessFrame() 完了通知の Observer.
    ///
    void OnFrameProcessed(const PipelineResult& result);

    FrameSyncProcess process_;

    ///
    /// @name 入力系 (Producer → リングバッファ → Acquire Strategy).
    ///
    /// Producer (InputSource / DeviceInput) は常にどちらか一方のみ動作させる.
    /// {@
    AudioInputBuffer input_buffer_;
    RingBufferAcquire ring_buffer_acquire_{&input_buffer_};
    InputSource input_source_{&input_buffer_};
    std::unique_ptr<DeviceInput> device_input_;
    /// @}

    ///
    /// @name 代替 Strategy の実体.
    ///
    /// StrategySlot は非所有ポインタを bind するため, 実体は MainModel が所有する.
    /// 既定 (index 0) の Strategy は FrameSyncProcessConfig の static 実体を再利用する.
    /// {@
    SineGenerator sine_generator_;
    HannWindow hann_window_;
    HannOverlapAdder hann_overlap_adder_;
    /// @}

    ///
    /// 音声ファイル入力のデータソース.
    ///
    FilePlayer file_player_;

    ///
    /// @name 出力系 (Output Strategy → リングバッファ → 出力デバイス).
    /// {@
    AudioOutputBuffer output_buffer_;
    RingBufferOutput ring_buffer_output_{&output_buffer_};
    std::unique_ptr<DeviceOutput> device_output_;
    /// @}

    std::jthread processing_thread_;

    std::atomic<std::uint64_t> processed_frame_count_{0};

    ///
    /// 入力が "Device" 選択中かどうか (ApplyDeviceSelection() の有効判定に使用).
    ///
    bool device_mode_{false};

    ///
    /// 出力が "Device" 選択中かどうか (ApplyOutputDeviceSelection() の有効判定に使用).
    ///
    bool output_device_mode_{false};
};
