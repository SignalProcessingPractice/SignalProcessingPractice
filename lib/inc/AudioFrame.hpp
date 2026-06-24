///
/// @file AudioFrame.hpp
///
#pragma once
#include <array>
#include <cstdint>

#include "AudioFrame.hpp"

// NumSamples: テンプレートによる可変なサンプル数
// SampleType: オーディオデータの型（デフォルトは float）
template <std::size_t NumSamples, typename SampleType = float>
class AudioFrameTemplate {
public:
    // コンストラクタ
    constexpr explicit AudioFrameTemplate(uint32_t sample_rate = 44100) noexcept
        : sample_rate_{sample_rate}, data_{} {
    }

    // 【値セマンティクスの要】
    // メンバが std::array と組み込み型のみなので、
    // 特殊メンバ関数はすべてコンパイラ生成のデフォルトで完璧に機能します（Rule of Zero）。
    AudioFrameTemplate(const AudioFrameTemplate&) = default;
    AudioFrameTemplate& operator=(const AudioFrameTemplate&) = default;
    AudioFrameTemplate(AudioFrameTemplate&&) = default;
    AudioFrameTemplate& operator=(AudioFrameTemplate&&) = default;
    ~AudioFrameTemplate() = default;

    // プロパティへのアクセス
    constexpr uint32_t sample_rate() const noexcept {
        return sample_rate_;
    }
    constexpr void set_sample_rate(uint32_t rate) noexcept {
        sample_rate_ = rate;
    }
    constexpr std::size_t size() const noexcept {
        return NumSamples;
    }

    // インデクサで特定のサンプルへアクセス
    constexpr SampleType& operator[](std::size_t index) noexcept {
        return data_[index];
    }
    constexpr const SampleType& operator[](std::size_t index) const noexcept {
        return data_[index];
    }

    // データへのアクセス
    constexpr SampleType* data() noexcept {
        return data_.data();
    }
    constexpr const SampleType* data() const noexcept {
        return data_.data();
    }

    // イテレータのサポート（標準アルゴリズムとの連携用）
    constexpr auto begin() noexcept {
        return data_.begin();
    }
    constexpr auto end() noexcept {
        return data_.end();
    }
    constexpr auto begin() const noexcept {
        return data_.begin();
    }
    constexpr auto end() const noexcept {
        return data_.end();
    }

    // 【値セマンティクスの要】等価演算子
    // 状態（サンプリング周波数と全オーディオデータ）が完全に一致するかで評価します。
    friend constexpr bool operator==(const AudioFrameTemplate& lhs,
                                     const AudioFrameTemplate& rhs) noexcept {
        return lhs.sample_rate_ == rhs.sample_rate_ && lhs.data_ == rhs.data_;
    }

    friend constexpr bool operator!=(const AudioFrameTemplate& lhs,
                                     const AudioFrameTemplate& rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    uint32_t sample_rate_;
    std::array<SampleType, NumSamples> data_;  // 生ポインタや vector ではなく array を使用
};
