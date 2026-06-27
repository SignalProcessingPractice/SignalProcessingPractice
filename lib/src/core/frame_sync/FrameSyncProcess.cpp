///
/// @file FrameSyncProcess.cpp
///
#include "FrameSyncProcess.hpp"

#include <bit>
#include <memory>
#include <new>

#include "../pipeline/PipelineContext.hpp"
#include "FrameSyncProcessConfig.hpp"

///
/// @name Impl.
/// @{
///

///
/// @brief Impl 定義.
///
struct FrameSyncProcess::Impl {
public:
    Impl() = default;

    explicit Impl(const FrameSyncProcessConfig& config) : pipeline_(config) {
    }

    Impl(const Impl&) = default;
    auto operator=(const Impl&) -> Impl& = default;

    Impl(Impl&&) = default;
    auto operator=(Impl&&) -> Impl& = default;

    ~Impl() = default;

    PipelineContext pipeline_;
};

///
/// @brief サイズ・アラインメント検証.
///
void FrameSyncProcess::CheckImpl() {
    static_assert(sizeof(Impl) <= kImplSize, "Impl is too large for static storage");

    static_assert(alignof(Impl) <= kImplAlign, "Impl alignment exceeds storage");
}

///
/// @brief Impl キャスト.
///
auto FrameSyncProcess::ImplPtr() -> Impl* {
    return std::launder(std::bit_cast<Impl*>(storage_.data()));
}

auto FrameSyncProcess::ImplPtr() const -> const Impl* {
    return std::launder(std::bit_cast<const Impl*>(storage_.data()));
}
///
/// @}
///

///
/// @name コンストラクタ / デストラクタ.
/// @{
///
FrameSyncProcess::FrameSyncProcess() : storage_{} {
    std::construct_at(ImplPtr());
}

FrameSyncProcess::FrameSyncProcess(const FrameSyncProcessConfig& config) : storage_{} {
    std::construct_at(ImplPtr(), config);
}

FrameSyncProcess::~FrameSyncProcess() {
    std::destroy_at(ImplPtr());
}

FrameSyncProcess::FrameSyncProcess(FrameSyncProcess&& other) noexcept : storage_{} {
    std::construct_at(ImplPtr(), *other.ImplPtr());
}

auto FrameSyncProcess::operator=(FrameSyncProcess&& other) noexcept -> FrameSyncProcess& {
    if (this != &other) {
        std::destroy_at(ImplPtr());
        std::construct_at(ImplPtr(), *other.ImplPtr());
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
void FrameSyncProcess::Attach() {
    ///
    /// TODO: 実装.
    ///
}

void FrameSyncProcess::Detach() {
    ///
    /// TODO: 実装.
    ///
}

void FrameSyncProcess::SetConfig([[maybe_unused]] AquireTag tag, AudioAquireStrategy strategy) {
    ImplPtr()->pipeline_.SetAquireStrategy(strategy);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] PreProcessTag tag, PreProcessStrategy strategy) {
    ImplPtr()->pipeline_.SetPreProcessStrategy(strategy);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] InferTag tag, InferStrategy strategy) {
    ImplPtr()->pipeline_.SetInferStrategy(strategy);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OutputTag tag, AudioOutputStrategy strategy) {
    ImplPtr()->pipeline_.SetOutputStrategy(strategy);
}

void FrameSyncProcess::ProcessFrame() {
    ImplPtr()->pipeline_.exec();
}
///
/// @}
///
