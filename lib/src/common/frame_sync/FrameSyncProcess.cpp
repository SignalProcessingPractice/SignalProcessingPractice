///
/// @file FrameSyncProcess.cpp
///
#include "FrameSyncProcess.hpp"

#include <memory>
#include <utility>

#include "../pipeline/PipelineContext.hpp"

///
/// @name Impl.
/// @{
///

///
/// @brief Impl 定義.
///
struct FrameSyncProcess::Impl
{

public:
    Impl() = default;

    Impl(const Impl&) = default;
    Impl& operator=(const Impl&) = default;

    Impl(Impl&&) = default;
    Impl& operator=(Impl&&) = default;

    ///
    /// @todo pipeline_ の, 各 Strategy 初期値の設定方法を検討する.
    ///
    PipelineContext pipeline_{ };

};

///
/// @brief サイズ・アラインメント検証.
///
void FrameSyncProcess::CheckImpl()
{
    static_assert(sizeof(Impl) <= kImplSize,
                  "Impl is too large for static storage");

    static_assert(alignof(Impl) <= kImplAlign,
                  "Impl alignment exceeds storage");
}

///
/// @brief Impl キャスト.
///
FrameSyncProcess::Impl* FrameSyncProcess::ImplPtr()
{
    return reinterpret_cast<Impl*>(&storage_);
}

const FrameSyncProcess::Impl* FrameSyncProcess::ImplPtr() const
{
    return reinterpret_cast<const Impl*>(&storage_);
}
///
/// @}
///

///
/// @name コンストラクタ / デストラクタ.
/// @{
///
FrameSyncProcess::FrameSyncProcess()
{
    std::construct_at(ImplPtr());
}

FrameSyncProcess::~FrameSyncProcess()
{
    std::destroy_at(ImplPtr());
}

FrameSyncProcess::FrameSyncProcess(FrameSyncProcess&& other)
{
    std::construct_at(ImplPtr(), std::move(*other.ImplPtr()));
}

FrameSyncProcess& FrameSyncProcess::operator=(FrameSyncProcess&& other)
{
    if (this != &other)
    {
        std::destroy_at(ImplPtr());
        std::construct_at(ImplPtr(), std::move(*other.ImplPtr()));
    }
    return *this;
}
///
/// @}
///

///
/// @name 公開 API.
/// @{
///
void FrameSyncProcess::Attach(void)
{
    ///
    /// TODO: 実装.
    ///
}

void FrameSyncProcess::Detach(void)
{
    ///
    /// TODO: 実装.
    ///
}

void FrameSyncProcess::SetConfig(void)
{
    ///
    /// TODO: 実装.
    ///
}

void FrameSyncProcess::ProcessFrame(void)
{
    ImplPtr()->pipeline_.exec();
}
///
/// @}
///
