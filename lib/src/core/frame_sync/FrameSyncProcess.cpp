///
/// @file FrameSyncProcess.cpp
///
#include "FrameSyncProcess.hpp"

#include <memory>
#include <utility>

#include "FrameSyncProcessConfig.hpp"
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
    Impl() : pipeline_() {};

    Impl(const FrameSyncProcessConfig &config) : pipeline_(config) {}

    Impl(const Impl&) = default;
    Impl& operator=(const Impl&) = default;

    Impl(Impl&&) = default;
    Impl& operator=(Impl&&) = default;


    PipelineContext pipeline_;
    
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

FrameSyncProcess::Impl* FrameSyncProcess::ImplPtr(const FrameSyncProcessConfig &config)
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

FrameSyncProcess::FrameSyncProcess(const FrameSyncProcessConfig &config)
{
    std::construct_at(ImplPtr(config));
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


///
/// @brief SetConfig 共通処理.
///
template<typename Tag, typename Strategy>
    void SetConfigImpl(
        FrameSyncProcess::Impl* impl, 
        Tag tag, 
        Strategy strategy
    )
{
    impl->pipeline_.SetConfig(tag, strategy);
}

void 
    FrameSyncProcess::SetConfig(
        AquireTag tag, 
        AudioAquireStrategy strategy
    )
{
    SetConfigImpl(ImplPtr(), tag, strategy);
}
void 
    FrameSyncProcess::SetConfig(
        PreProcessTag tag, 
        PreProcessStrategy strategy
    )
{
    SetConfigImpl(ImplPtr(), tag, strategy);
}
void 
    FrameSyncProcess::SetConfig(
        InferTag tag, 
        InferStrategy strategy
    )
{
    SetConfigImpl(ImplPtr(), tag, strategy);
}
void 
    FrameSyncProcess::SetConfig(
        OutputTag tag, 
        AudioOutputStrategy strategy
    )
{
    SetConfigImpl(ImplPtr(), tag, strategy);
}


void FrameSyncProcess::ProcessFrame(void)
{
    ImplPtr()->pipeline_.exec();
}
///
/// @}
///
