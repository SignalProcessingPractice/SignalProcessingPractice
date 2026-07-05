///
/// @file SpectrumPresenter.h
///
#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <span>

#include "FrameSyncProcess.hpp"
#include "PipelineResult.hpp"

class MainModel;

///
/// @brief 音声波形（周波数軸）表示の Presenter 層.
///
/// View の Tick 通知ごとに MainModel から結果スナップショットを取得し,
/// FFT 結果 (fft_frame) から振幅スペクトラム (dB) を計算して View へ描画指示する.
///
class SpectrumPresenter {
public:
    ///
    /// Tick Observer 登録関数の型 (View の AttachFrameTickObserver() を注入する).
    ///
    using TickRegistrar = std::function<void(std::function<void()>)>;

    ///
    /// 描画指示関数の型 (View の UpdateSpectrum() を注入する).
    ///
    using RenderFn = std::function<void(std::span<const float>)>;

    ///
    /// スペクトラムのビン数 (DC〜ナイキスト).
    ///
    static constexpr std::size_t kNumBins = (FrameSyncProcess::audio_frame_length / 2) + 1;

    SpectrumPresenter(MainModel* model, const TickRegistrar& registrar, RenderFn render);

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

    ///
    /// 振幅スペクトラム (dB) の計算バッファ.
    ///
    std::array<float, kNumBins> magnitude_db_{};
};
