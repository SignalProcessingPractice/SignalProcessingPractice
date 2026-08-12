///
/// @file WaveformPresenter.h
///
#pragma once

#include <functional>
#include <span>

#include "PipelineResult.hpp"

class MainModel;

///
/// @brief 音声波形（時間軸）表示の Presenter 層.
///
/// View の Tick 通知ごとに MainModel から結果スナップショットを取得し,
/// 入力ホップ (input_hop) の描画を View へ指示する.
///
class WaveformPresenter {
public:
    ///
    /// Tick Observer 登録関数の型 (View の AttachFrameTickObserver() を注入する).
    ///
    using TickRegistrar = std::function<void(std::function<void()>)>;

    ///
    /// 描画指示関数の型 (View の UpdateWaveform() を注入する).
    ///
    using RenderFn = std::function<void(std::span<const float>)>;

    WaveformPresenter(MainModel* model, const TickRegistrar& registrar, RenderFn render);

private:
    ///
    /// Tick ごとの表示更新.
    ///
    void OnTick();

    MainModel* model_;
    RenderFn render_;

    ///
    /// GetResult() のコピー先バッファ.
    ///
    PipelineResult result_;
};
