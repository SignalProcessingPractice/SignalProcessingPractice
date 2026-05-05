///
/// @file FrameSyncProcess.hpp
///
#pragma once
#include <cstddef>

class FrameSyncProcess
{
public:

    FrameSyncProcess();
    ~FrameSyncProcess();

    FrameSyncProcess(FrameSyncProcess&&);
    FrameSyncProcess& operator=(FrameSyncProcess&&);

    FrameSyncProcess(const FrameSyncProcess&) = delete;
    FrameSyncProcess& operator=(const FrameSyncProcess&) = delete;

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
    void SetConfig(void);

    ///
    /// 1 フレーム分の処理を実行.
    ///
    void ProcessFrame(void);

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
