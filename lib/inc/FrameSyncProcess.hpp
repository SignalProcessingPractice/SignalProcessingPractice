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
