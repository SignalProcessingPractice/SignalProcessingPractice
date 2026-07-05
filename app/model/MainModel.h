///
/// @file MainModel.h
///
#pragma once

#include <atomic>
#include <cstdint>
#include <stop_token>
#include <thread>

#include "FrameSyncProcess.hpp"
#include "Strategies/HannOverlapAdder.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/SineGenerator.hpp"
#include "common/PipelineSelection.h"
#include "model/AudioInputBuffer.h"
#include "model/InputSource.h"
#include "model/RingBufferAcquire.h"

struct PipelineResult;

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
    /// kAcquire は Acquire Strategy ではなく InputSource の generator を切り替える.
    ///
    void ApplyStrategySelection(PipelineStage stage, int index);

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
    /// {@
    AudioInputBuffer input_buffer_;
    RingBufferAcquire ring_buffer_acquire_{&input_buffer_};
    InputSource input_source_{&input_buffer_};
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

    std::jthread processing_thread_;

    std::atomic<std::uint64_t> processed_frame_count_{0};
};
