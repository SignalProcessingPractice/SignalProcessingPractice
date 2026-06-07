///
/// @file FrameSyncProcess.hpp
///
#pragma once
#include <cstddef>

#include <etl/delegate.h>

#include "AudioFrame.hpp"

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
    static constexpr std::size_t audio_frame_length = 1024;
    static constexpr std::size_t audio_hop_length   = 512;

    ///
    /// オーディオホップ.
    ///
    using AudioHop =
        AudioFrameTemplate<audio_hop_length, float>;

    ///
    /// オーディオフレーム.
    ///
    using AudioFrame =
        AudioFrameTemplate<audio_frame_length, float>;

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
    struct Overlap {};
    struct Window {};
    struct Fft {};
    struct Infer {};
    struct PostProcess {};
    struct OverlapAdd {};
    struct Output {};

    using AquireTag         = tag_t<Aquire>;
    using PreProcessTag     = tag_t<PreProcess>;
    using OverlapTag        = tag_t<Overlap>;
    using WindowTag         = tag_t<Window>;
    using FftTag            = tag_t<Fft>;
    using InferTag          = tag_t<Infer>;
    using PostProcessTag    = tag_t<PostProcess>;
    using OverlapAddTag     = tag_t<OverlapAdd>;
    using OutputTag         = tag_t<Output>;
/// @}

///
/// @name Strategy 型.
/// {@
    ///
    /// オーディオフレーム獲得.
    ///
    using AudioAquireStrategy = 
        etl::delegate< AudioHop( void ) >;

    ///
    /// オーディオ前処理.
    ///
    using PreProcessStrategy = 
        etl::delegate< AudioHop( AudioHop &&frame ) >;

    ///
    /// オーバーラッピング.
    ///
    using OverlapStrategy = 
        etl::delegate< AudioFrame( AudioHop &&frame ) >;

    ///
    /// 窓関数の積算.
    ///
    using WindowStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// FFT.
    ///
    using FftStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// 推論.
    ///
    using InferStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// オーディオ後処理.
    ///
    using PostProcessStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// Overlap-Add.
    ///
    using OverlapAddStrategy = 
        etl::delegate< AudioHop( AudioFrame &&frame ) >;

    ///
    /// オーディオ出力.
    ///
    using AudioOutputStrategy = 
        etl::delegate< void( AudioHop &&frame ) >;
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
    void SetConfig(AquireTag tag, AudioAquireStrategy strategy);
    void SetConfig(PreProcessTag tag, PreProcessStrategy strategy);
    void SetConfig(OverlapTag tag, OverlapStrategy strategy);
    void SetConfig(WindowTag tag, WindowStrategy strategy);
    void SetConfig(FftTag tag, FftStrategy strategy);
    void SetConfig(InferTag tag, InferStrategy strategy);
    void SetConfig(PostProcessTag tag, PostProcessStrategy strategy);
    void SetConfig(OverlapAddTag tag, OverlapAddStrategy strategy);
    void SetConfig(OutputTag tag, AudioOutputStrategy strategy);

    ///
    /// 1 フレーム分の処理を実行.
    ///
    void ProcessFrame(void);
/// @}

    ///
    /// Impl 前方宣言.
    ///
    struct Impl;

private:

    ///
    /// Impl サイズの上限.
    ///
    static constexpr std::size_t kImplSize  = 16384; // 16KB
    static constexpr std::size_t kImplAlign = alignof(std::max_align_t);

    alignas(kImplAlign) std::byte storage_[kImplSize];

    ///
    /// Impl へのキャスト用.
    ///
    Impl* ImplPtr();
    const Impl* ImplPtr() const;

    ///
    /// @brief Impl サイズのコンパイル時検証.
    ///
    static void CheckImpl();

};
