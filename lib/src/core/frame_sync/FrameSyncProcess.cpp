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
#include "PipelineResult.hpp"

///
/// @brief Strategy のペンディングスロット.
///
/// 非 RT スレッドからの書き込みと DSP スレッドでの読み取り・適用を
/// release/acquire ペアで安全に受け渡すためのラッパ.
///
template <typename Strategy>
struct PendingSlot {
    PendingSlot() = default;

    PendingSlot(const PendingSlot& other) noexcept
        : value_(other.value_),
          flag_(other.flag_.load(std::memory_order_relaxed))
    {
    }

    auto operator=(const PendingSlot& other) noexcept -> PendingSlot&
    {
        if (this != &other) {
            value_ = other.value_;
            flag_.store(other.flag_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }

    PendingSlot(PendingSlot&&) = delete;
    auto operator=(PendingSlot&&) -> PendingSlot& = delete;
    ~PendingSlot() = default;

    auto set(Strategy strategy) -> void
    {
        value_ = std::move(strategy);
        flag_.store(true, std::memory_order_release);
    }

    template <typename Fn>
    [[nodiscard]] auto consume(Fn&& func) -> bool
    {
        if (flag_.exchange(false, std::memory_order_acquire)) {
            std::forward<Fn>(func)(std::move(value_));
            return true;
        }
        return false;
    }

private:
    Strategy value_;
    std::atomic<bool> flag_{false};
};

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
    explicit Impl(const FrameSyncProcessConfig& config)
        : pipeline_(config)
    {
    }

    Impl(const Impl& other) noexcept
        : pipeline_(other.pipeline_),
          result_buffer_(other.result_buffer_),
          seq_(other.seq_.load(std::memory_order_relaxed)),
          observers_(other.observers_),
          observer_count_(other.observer_count_),
          pending_observers_(other.pending_observers_),
          pending_observer_count_(other.pending_observer_count_),
          pending_observers_flag_(other.pending_observers_flag_.load(std::memory_order_relaxed)),
          pending_acquire_(other.pending_acquire_),
          pending_pre_process_(other.pending_pre_process_),
          pending_overlap_(other.pending_overlap_),
          pending_window_(other.pending_window_),
          pending_fft_(other.pending_fft_),
          pending_infer_(other.pending_infer_),
          pending_post_process_(other.pending_post_process_),
          pending_overlap_add_(other.pending_overlap_add_),
          pending_output_(other.pending_output_)
    {
    }

    auto operator=(const Impl& other) noexcept -> Impl&
    {
        if (this != &other) {
            pipeline_ = other.pipeline_;
            result_buffer_ = other.result_buffer_;
            seq_.store(other.seq_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            observers_ = other.observers_;
            observer_count_ = other.observer_count_;
            pending_observers_ = other.pending_observers_;
            pending_observer_count_ = other.pending_observer_count_;
            pending_observers_flag_.store(
                    other.pending_observers_flag_.load(std::memory_order_relaxed),
                    std::memory_order_relaxed);
            pending_acquire_ = other.pending_acquire_;
            pending_pre_process_ = other.pending_pre_process_;
            pending_overlap_ = other.pending_overlap_;
            pending_window_ = other.pending_window_;
            pending_fft_ = other.pending_fft_;
            pending_infer_ = other.pending_infer_;
            pending_post_process_ = other.pending_post_process_;
            pending_overlap_add_ = other.pending_overlap_add_;
            pending_output_ = other.pending_output_;
        }
        return *this;
    }

    Impl(Impl&&) = delete;
    auto operator=(Impl&&) -> Impl& = delete;
    ~Impl() = default;

    [[nodiscard]] auto pipeline() -> PipelineContext&
    {
        return pipeline_;
    }
    [[nodiscard]] auto seq() -> std::atomic<uint32_t>&
    {
        return seq_;
    }
    [[nodiscard]] auto seq() const -> const std::atomic<uint32_t>&
    {
        return seq_;
    }
    [[nodiscard]] auto result_buffer() -> PipelineResult&
    {
        return result_buffer_;
    }
    [[nodiscard]] auto result_buffer() const -> const PipelineResult&
    {
        return result_buffer_;
    }
    [[nodiscard]] auto observers() -> std::array<ProcessCompleteObserver, kMaxObservers>&
    {
        return observers_;
    }
    [[nodiscard]] auto observer_count() -> std::size_t&
    {
        return observer_count_;
    }
    [[nodiscard]] auto observer_count() const -> std::size_t
    {
        return observer_count_;
    }

    [[nodiscard]] auto pending_observers() -> std::array<ProcessCompleteObserver, kMaxObservers>&
    {
        return pending_observers_;
    }
    [[nodiscard]] auto pending_observer_count() -> std::size_t&
    {
        return pending_observer_count_;
    }
    [[nodiscard]] auto pending_observers_flag() -> std::atomic<bool>&
    {
        return pending_observers_flag_;
    }

    [[nodiscard]] auto pending_acquire() -> PendingSlot<AudioAcquireStrategy>&
    {
        return pending_acquire_;
    }
    [[nodiscard]] auto pending_pre_process() -> PendingSlot<PreProcessStrategy>&
    {
        return pending_pre_process_;
    }
    [[nodiscard]] auto pending_overlap() -> PendingSlot<OverlapStrategy>&
    {
        return pending_overlap_;
    }
    [[nodiscard]] auto pending_window() -> PendingSlot<WindowStrategy>&
    {
        return pending_window_;
    }
    [[nodiscard]] auto pending_fft() -> PendingSlot<FftStrategy>&
    {
        return pending_fft_;
    }
    [[nodiscard]] auto pending_infer() -> PendingSlot<InferStrategy>&
    {
        return pending_infer_;
    }
    [[nodiscard]] auto pending_post_process() -> PendingSlot<PostProcessStrategy>&
    {
        return pending_post_process_;
    }
    [[nodiscard]] auto pending_overlap_add() -> PendingSlot<OverlapAddStrategy>&
    {
        return pending_overlap_add_;
    }
    [[nodiscard]] auto pending_output() -> PendingSlot<AudioOutputStrategy>&
    {
        return pending_output_;
    }

private:
    PipelineContext pipeline_;
    PipelineResult result_buffer_;
    std::atomic<uint32_t> seq_{0};
    std::array<ProcessCompleteObserver, kMaxObservers> observers_{};
    std::size_t observer_count_{0};

    ///
    /// @name Observer ペンディングスロット.
    ///
    /// Attach()/Detach() から書き込まれ, ProcessFrame() の先頭でフレーム境界に適用される.
    /// @{
    std::array<ProcessCompleteObserver, kMaxObservers> pending_observers_{};
    std::size_t pending_observer_count_{0};
    std::atomic<bool> pending_observers_flag_{false};
    /// @}

    ///
    /// @name Strategy ペンディングスロット.
    ///
    /// SetConfig() から書き込まれ, ProcessFrame() の先頭でフレーム境界に適用される.
    /// @{
    PendingSlot<AudioAcquireStrategy> pending_acquire_;
    PendingSlot<PreProcessStrategy> pending_pre_process_;
    PendingSlot<OverlapStrategy> pending_overlap_;
    PendingSlot<WindowStrategy> pending_window_;
    PendingSlot<FftStrategy> pending_fft_;
    PendingSlot<InferStrategy> pending_infer_;
    PendingSlot<PostProcessStrategy> pending_post_process_;
    PendingSlot<OverlapAddStrategy> pending_overlap_add_;
    PendingSlot<AudioOutputStrategy> pending_output_;
    /// @}
};

///
/// @brief サイズ・アラインメント検証.
///
void FrameSyncProcess::CheckImpl()
{
    static_assert(sizeof(Impl) <= kImplSize, "Impl is too large for static storage");

    static_assert(alignof(Impl) <= kImplAlign, "Impl alignment exceeds storage");
}

///
/// @brief Impl キャスト.
///
auto FrameSyncProcess::ImplPtr() -> Impl*
{
    return std::launder(std::bit_cast<Impl*>(storage_.data()));
}

auto FrameSyncProcess::ImplPtr() const -> const Impl*
{
    return std::launder(std::bit_cast<const Impl*>(storage_.data()));
}
///
/// @}
///

///
/// @name コンストラクタ / デストラクタ.
/// @{
///
FrameSyncProcess::FrameSyncProcess()
    : storage_{}
{
    // 全 StrategySlot が未バインドの状態を作らないよう, 既定 Config で構築する.
    std::construct_at(ImplPtr(), FrameSyncProcessConfig{});
}

FrameSyncProcess::FrameSyncProcess(const FrameSyncProcessConfig& config)
    : storage_{}
{
    std::construct_at(ImplPtr(), config);
}

FrameSyncProcess::~FrameSyncProcess()
{
    std::destroy_at(ImplPtr());
}

FrameSyncProcess::FrameSyncProcess(FrameSyncProcess&& other) noexcept
    : storage_{}
{
    std::construct_at(ImplPtr(), *other.ImplPtr());
}

auto FrameSyncProcess::operator=(FrameSyncProcess&& other) noexcept -> FrameSyncProcess&
{
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
void FrameSyncProcess::Attach(ProcessCompleteObserver delegate)
{
    auto& impl = *ImplPtr();
    if (impl.pending_observer_count() < kMaxObservers) {
        impl.pending_observers().at(impl.pending_observer_count()) = delegate;
        ++impl.pending_observer_count();
        impl.pending_observers_flag().store(true, std::memory_order_release);
    }
}

void FrameSyncProcess::Detach(ProcessCompleteObserver delegate)
{
    auto& impl = *ImplPtr();
    const auto count = impl.pending_observer_count();
    for (std::size_t i = 0; i < count; ++i) {
        if (impl.pending_observers().at(i) == delegate) {
            for (std::size_t j = i; j < count - 1U; ++j) {
                impl.pending_observers().at(j) = impl.pending_observers().at(j + 1U);
            }
            impl.pending_observers().at(count - 1U) = ProcessCompleteObserver{};
            --impl.pending_observer_count();
            impl.pending_observers_flag().store(true, std::memory_order_release);
            return;
        }
    }
}

void FrameSyncProcess::GetResult(PipelineResult* out) const
{
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

void FrameSyncProcess::SetConfig([[maybe_unused]] AcquireTag tag, AudioAcquireStrategy strategy)
{
    ImplPtr()->pending_acquire().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] PreProcessTag tag, PreProcessStrategy strategy)
{
    ImplPtr()->pending_pre_process().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OverlapTag tag, OverlapStrategy strategy)
{
    ImplPtr()->pending_overlap().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] WindowTag tag, WindowStrategy strategy)
{
    ImplPtr()->pending_window().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] FftTag tag, FftStrategy strategy)
{
    ImplPtr()->pending_fft().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] InferTag tag, InferStrategy strategy)
{
    ImplPtr()->pending_infer().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] PostProcessTag tag, PostProcessStrategy strategy)
{
    ImplPtr()->pending_post_process().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OverlapAddTag tag, OverlapAddStrategy strategy)
{
    ImplPtr()->pending_overlap_add().set(std::move(strategy));
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OutputTag tag, AudioOutputStrategy strategy)
{
    ImplPtr()->pending_output().set(std::move(strategy));
}

void FrameSyncProcess::ProcessFrame()
{
    auto& impl = *ImplPtr();

    bool any_applied = false;
    any_applied |= impl.pending_acquire().consume([&impl](AudioAcquireStrategy strategy) {
        impl.pipeline().SetAcquireStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_pre_process().consume([&impl](PreProcessStrategy strategy) {
        impl.pipeline().SetPreProcessStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_overlap().consume([&impl](OverlapStrategy strategy) {
        impl.pipeline().SetOverlapStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_window().consume([&impl](WindowStrategy strategy) {
        impl.pipeline().SetWindowStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_fft().consume([&impl](FftStrategy strategy) {
        impl.pipeline().SetFftStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_infer().consume([&impl](InferStrategy strategy) {
        impl.pipeline().SetInferStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_post_process().consume([&impl](PostProcessStrategy strategy) {
        impl.pipeline().SetPostProcessStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_overlap_add().consume([&impl](OverlapAddStrategy strategy) {
        impl.pipeline().SetOverlapAddStrategy(std::move(strategy));
    });
    any_applied |= impl.pending_output().consume([&impl](AudioOutputStrategy strategy) {
        impl.pipeline().SetOutputStrategy(std::move(strategy));
    });
    if (impl.pending_observers_flag().exchange(false, std::memory_order_acquire)) {
        impl.observers() = impl.pending_observers();
        impl.observer_count() = impl.pending_observer_count();
        any_applied = true;
    }
    if (any_applied) {
        impl.pipeline().reset();
    }

    impl.seq().fetch_add(1U, std::memory_order_seq_cst);
    impl.pipeline().exec(&impl.result_buffer());
    impl.seq().fetch_add(1U, std::memory_order_seq_cst);
    const auto count = impl.observer_count();
    for (std::size_t i = 0; i < count; ++i) {
        impl.observers().at(i)(impl.result_buffer());
    }
}
///
/// @}
///
