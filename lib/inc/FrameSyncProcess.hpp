///
/// @file FrameSyncProcess.hpp
///
#pragma once
#include <cstddef>
#include <array>
#include <cstdint>
#include <algorithm>

#include <etl/delegate.h>


// NumSamples: テンプレートによる可変なサンプル数
// SampleType: オーディオデータの型（デフォルトは float）
template <std::size_t NumSamples, typename SampleType = float>
class AudioFrameTemplate {
public:
    // コンストラクタ
    constexpr explicit AudioFrameTemplate(uint32_t sample_rate = 44100) noexcept
        : sample_rate_{sample_rate}, data_{} {}

    // 【値セマンティクスの要】
    // メンバが std::array と組み込み型のみなので、
    // 特殊メンバ関数はすべてコンパイラ生成のデフォルトで完璧に機能します（Rule of Zero）。
    AudioFrameTemplate(const AudioFrameTemplate&) = default;
    AudioFrameTemplate& operator=(const AudioFrameTemplate&) = default;
    AudioFrameTemplate(AudioFrameTemplate&&) = default;
    AudioFrameTemplate& operator=(AudioFrameTemplate&&) = default;
    ~AudioFrameTemplate() = default;

    // プロパティへのアクセス
    constexpr uint32_t sample_rate() const noexcept { return sample_rate_; }
    constexpr void set_sample_rate(uint32_t rate) noexcept { sample_rate_ = rate; }
    constexpr std::size_t size() const noexcept { return NumSamples; }

    // データへのアクセス
    constexpr SampleType* data() noexcept { return data_.data(); }
    constexpr const SampleType* data() const noexcept { return data_.data(); }

    // イテレータのサポート（標準アルゴリズムとの連携用）
    constexpr auto begin() noexcept { return data_.begin(); }
    constexpr auto end() noexcept { return data_.end(); }
    constexpr auto begin() const noexcept { return data_.begin(); }
    constexpr auto end() const noexcept { return data_.end(); }

    // 【値セマンティクスの要】等価演算子
    // 状態（サンプリング周波数と全オーディオデータ）が完全に一致するかで評価します。
    friend constexpr bool operator==(const AudioFrameTemplate& lhs, const AudioFrameTemplate& rhs) noexcept {
        return lhs.sample_rate_ == rhs.sample_rate_ && lhs.data_ == rhs.data_;
    }

    friend constexpr bool operator!=(const AudioFrameTemplate& lhs, const AudioFrameTemplate& rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    uint32_t sample_rate_;
    std::array<SampleType, NumSamples> data_; // 生ポインタや vector ではなく array を使用
};


class FrameSyncProcess
{
public:

///
/// @name オーディオフレーム.
/// {@

    ///
    /// オーディオフレームの 1 フレーム辺りのサンプル数.
    ///
    /// TODO: サンプル数は後で再検討する.
    ///
    static constexpr std::size_t audio_frame_length = 256;

    ///
    /// オーディオフレーム.
    ///
    using AudioFrame =
        AudioFrameTemplate<audio_frame_length, double>;

/// @}

///
/// @name タグ型.
/// {@
    template<typename T>
    struct tag_t {
        explicit tag_t() = default;
    };

    struct Aquire {};
    struct PreProcess {};
    struct Infer {};
    struct Output {};

    using AquireTag     = tag_t<Aquire>;
    using PreProcessTag = tag_t<PreProcess>;
    using InferTag      = tag_t<Infer>;
    using OutputTag     = tag_t<Output>;
/// @}

///
/// @name Strategy 型.
/// {@
    ///
    /// オーディオフレーム獲得.
    ///
    using AudioAquireStrategy = 
        etl::delegate< AudioFrame( void ) >;

    ///
    /// オーディオ前処理.
    ///
    using PreProcessStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// 推論.
    ///
    using InferStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// オーディオ出力.
    ///
    using AudioOutputStrategy = 
        etl::delegate< void( AudioFrame &&frame ) >;
/// @}

///
/// @name ctor, dtor.
/// {@
    FrameSyncProcess();
    ~FrameSyncProcess();

    FrameSyncProcess(FrameSyncProcess&&);
    FrameSyncProcess& operator=(FrameSyncProcess&&);

    FrameSyncProcess(const FrameSyncProcess&) = delete;
    FrameSyncProcess& operator=(const FrameSyncProcess&) = delete;
/// @}

///
/// @name 公開関数.
/// {@
    ///
    /// Obverser 登録.
    ///
    void Attach(void);

    ///
    /// Obverser 解除.
    ///
    void Detach(void);

    ///
    /// 処理設定.
    ///
    void SetConfig();

    ///
    /// 1 フレーム分の処理を実行.
    ///
    void ProcessFrame(void);
/// @}

private:

    ///
    /// Impl サイズの上限.
    ///
    static constexpr std::size_t kImplSize  = 1024;
    static constexpr std::size_t kImplAlign = alignof(std::max_align_t);

    alignas(kImplAlign) std::byte storage_[kImplSize];

    ///
    /// Impl へのキャスト用.
    ///
    struct Impl;
    Impl* ImplPtr();
    const Impl* ImplPtr() const;

    ///
    /// @brief Impl サイズのコンパイル時検証.
    ///
    static void CheckImpl();

};
