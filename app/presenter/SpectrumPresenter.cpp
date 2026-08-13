///
/// @file SpectrumPresenter.cpp
///

#include "presenter/SpectrumPresenter.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "model/MainModel.h"

namespace {

///
/// dB 変換の下限 (表示レンジの下限に一致させる).
///
constexpr float kFloorDb = -100.0F;

///
/// フルスケール正規化係数 (振幅 1.0 のサイン波のピークが 0 dB となるようにする).
///
constexpr float kFullScale = static_cast<float>(FrameSyncProcess::audio_frame_length) / 2.0F;

///
/// 正規化振幅を dB へ変換する.
///
auto ToDb(float magnitude) -> float
{
    const float normalized = magnitude / kFullScale;
    if (normalized <= 0.0F) {
        return kFloorDb;
    }
    constexpr float kDbFactor = 20.0F;
    return std::max(kDbFactor * std::log10(normalized), kFloorDb);
}

}  // namespace

SpectrumPresenter::SpectrumPresenter(MainModel* model, const TickRegistrar& registrar,
                                     RenderFn render)
    : model_(model),
      render_(std::move(render))
{
    registrar([this] {
        OnTick();
    });
}

void SpectrumPresenter::OnTick()
{
    model_->Process().GetResult(&result_);

    // arm_rfft_fast_f32 のパック形式: [DC, Nyquist, re1, im1, re2, im2, ...]
    const auto& fft = result_.fft_frame;
    magnitude_db_.at(0) = ToDb(std::abs(fft[0]));
    magnitude_db_.at(kNumBins - 1) = ToDb(std::abs(fft[1]));
    for (std::size_t bin = 1; bin < kNumBins - 1; ++bin) {
        const float real = fft[2 * bin];
        const float imag = fft[(2 * bin) + 1];
        magnitude_db_.at(bin) = ToDb(std::hypot(real, imag));
    }

    render_(std::span<const float>{magnitude_db_.data(), magnitude_db_.size()});
}
