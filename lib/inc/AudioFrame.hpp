///
/// @file AudioFrame.hpp
///
#pragma once
#include <array>
#include <cstdint>

// NumSamples: テンプレートによる可変なサンプル数
// SampleType: オーディオデータの型（デフォルトは float）
template <std::size_t NumSamples, typename SampleType = float>
class AudioFrameTemplate {
public:
    // コンストラクタ
    static constexpr uint32_t kDefaultSampleRate = 44100U;

    constexpr explicit AudioFrameTemplate(uint32_t sample_rate = kDefaultSampleRate) noexcept
        : sample_rate_{sample_rate}
    {
    }

    // 【値セマンティクスの要】
    // メンバが std::array と組み込み型のみなので、
    // 特殊メンバ関数はすべてコンパイラ生成のデフォルトで完璧に機能します（Rule of Zero）。
    AudioFrameTemplate(const AudioFrameTemplate&) = default;
    auto operator=(const AudioFrameTemplate&) -> AudioFrameTemplate& = default;
    AudioFrameTemplate(AudioFrameTemplate&&) = default;
    auto operator=(AudioFrameTemplate&&) -> AudioFrameTemplate& = default;
    ~AudioFrameTemplate() = default;

    // プロパティへのアクセス
    [[nodiscard]] constexpr auto sample_rate() const noexcept -> uint32_t
    {
        return sample_rate_;
    }
    constexpr auto set_sample_rate(uint32_t rate) noexcept -> void
    {
        sample_rate_ = rate;
    }
    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
    {
        return NumSamples;
    }

    // インデクサで特定のサンプルへアクセス
    constexpr auto operator[](std::size_t index) -> SampleType&
    {
        return data_.at(index);
    }
    constexpr auto operator[](std::size_t index) const -> const SampleType&
    {
        return data_.at(index);
    }

    // データへのアクセス
    [[nodiscard]] constexpr auto data() noexcept -> SampleType*
    {
        return data_.data();
    }
    [[nodiscard]] constexpr auto data() const noexcept -> const SampleType*
    {
        return data_.data();
    }

    // イテレータのサポート（標準アルゴリズムとの連携用）
    [[nodiscard]] constexpr auto begin() noexcept
    {
        return data_.begin();
    }
    [[nodiscard]] constexpr auto end() noexcept
    {
        return data_.end();
    }
    [[nodiscard]] constexpr auto begin() const noexcept
    {
        return data_.begin();
    }
    [[nodiscard]] constexpr auto end() const noexcept
    {
        return data_.end();
    }

    // 【値セマンティクスの要】等価演算子
    // 状態（サンプリング周波数と全オーディオデータ）が完全に一致するかで評価します。
    friend constexpr auto operator==(const AudioFrameTemplate& lhs,
                                     const AudioFrameTemplate& rhs) noexcept -> bool
    {
        return lhs.sample_rate_ == rhs.sample_rate_ && lhs.data_ == rhs.data_;
    }

    friend constexpr auto operator!=(const AudioFrameTemplate& lhs,
                                     const AudioFrameTemplate& rhs) noexcept -> bool
    {
        return !(lhs == rhs);
    }

private:
    uint32_t sample_rate_{kDefaultSampleRate};
    std::array<SampleType, NumSamples> data_{};  // 生ポインタや vector ではなく array を使用
};
