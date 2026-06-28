///
/// @file FrameSyncProcess.cpp
///
#include "FrameSyncProcess.hpp"

#include <array>
#include <atomic>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <utility>

#include "../pipeline/PipelineContext.hpp"
#include "FrameSyncProcessConfig.hpp"
#include "IFrameSyncObserver.hpp"
#include "PipelineResult.hpp"

///
/// @name Impl.
/// @{
///

static constexpr std::size_t kMaxObservers = 8;

///
/// @brief Impl 定義.
///
struct FrameSyncProcess::Impl {
public:
    Impl() = default;

    explicit Impl(const FrameSyncProcessConfig& config) : pipeline_(config) {}

    Impl(const Impl& other) noexcept
        : pipeline_(other.pipeline_),
          result_buffer_(other.result_buffer_),
          seq_(other.seq_.load(std::memory_order_relaxed)),
          observers_(other.observers_),
          observer_count_(other.observer_count_) {}

    auto operator=(const Impl& other) noexcept -> Impl& {
        if (this != &other) {
            pipeline_ = other.pipeline_;
            result_buffer_ = other.result_buffer_;
            seq_.store(other.seq_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            observers_ = other.observers_;
            observer_count_ = other.observer_count_;
        }
        return *this;
    }

    Impl(Impl&&) = delete;
    auto operator=(Impl&&) -> Impl& = delete;
    ~Impl() = default;

    [[nodiscard]] auto pipeline() -> PipelineContext& { return pipeline_; }
    [[nodiscard]] auto seq() -> std::atomic<uint32_t>& { return seq_; }
    [[nodiscard]] auto seq() const -> const std::atomic<uint32_t>& { return seq_; }
    [[nodiscard]] auto result_buffer() -> PipelineResult& { return result_buffer_; }
    [[nodiscard]] auto result_buffer() const -> const PipelineResult& { return result_buffer_; }
    [[nodiscard]] auto observers() -> std::array<IFrameSyncObserver*, kMaxObservers>& {
        return observers_;
    }
    [[nodiscard]] auto observer_count() -> std::size_t& { return observer_count_; }
    [[nodiscard]] auto observer_count() const -> std::size_t { return observer_count_; }

private:
    PipelineContext pipeline_;
    PipelineResult result_buffer_;
    std::atomic<uint32_t> seq_{0};
    std::array<IFrameSyncObserver*, kMaxObservers> observers_{};
    std::size_t observer_count_{0};
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
void FrameSyncProcess::Attach(IFrameSyncObserver* observer) {
    auto& impl = *ImplPtr();
    if (impl.observer_count() < kMaxObservers) {
        impl.observers().at(impl.observer_count()) = observer;
        ++impl.observer_count();
    }
}

void FrameSyncProcess::Detach(IFrameSyncObserver* observer) {
    auto& impl = *ImplPtr();
    const auto count = impl.observer_count();
    for (std::size_t i = 0; i < count; ++i) {
        if (impl.observers().at(i) == observer) {
            for (std::size_t j = i; j < count - 1U; ++j) {
                impl.observers().at(j) = impl.observers().at(j + 1U);
            }
            impl.observers().at(count - 1U) = nullptr;
            --impl.observer_count();
            return;
        }
    }
}

void FrameSyncProcess::GetResult(PipelineResult* out) const {
    const auto& impl = *ImplPtr();
    while (true) {
        const auto seq1 = impl.seq().load(std::memory_order_seq_cst);
        if ((seq1 & 1U) != 0U) {
            continue;
        }
        *out = impl.result_buffer();
        const auto seq2 = impl.seq().load(std::memory_order_seq_cst);
        if (seq1 == seq2) {
            return;
        }
    }
}

void FrameSyncProcess::SetConfig([[maybe_unused]] AquireTag tag, AudioAquireStrategy strategy) {
    ImplPtr()->pipeline().SetAquireStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] PreProcessTag tag, PreProcessStrategy strategy) {
    ImplPtr()->pipeline().SetPreProcessStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OverlapTag tag, OverlapStrategy strategy) {
    ImplPtr()->pipeline().SetOverlapStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] WindowTag tag, WindowStrategy strategy) {
    ImplPtr()->pipeline().SetWindowStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] FftTag tag, FftStrategy strategy) {
    ImplPtr()->pipeline().SetFftStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] InferTag tag, InferStrategy strategy) {
    ImplPtr()->pipeline().SetInferStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] PostProcessTag tag, PostProcessStrategy strategy) {
    ImplPtr()->pipeline().SetPostProcessStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OverlapAddTag tag, OverlapAddStrategy strategy) {
    ImplPtr()->pipeline().SetOverlapAddStrategy(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OutputTag tag, AudioOutputStrategy strategy) {
    ImplPtr()->pipeline().SetOutputStrategy(std::move(strategy));
}

void FrameSyncProcess::ProcessFrame() {
    auto& impl = *ImplPtr();
    impl.seq().fetch_add(1U, std::memory_order_seq_cst);
    impl.pipeline().exec(&impl.result_buffer());
    impl.seq().fetch_add(1U, std::memory_order_seq_cst);
    const auto count = impl.observer_count();
    for (std::size_t i = 0; i < count; ++i) {
        impl.observers().at(i)->OnProcessFrame(impl.result_buffer());
    }
}
///
/// @}
///
