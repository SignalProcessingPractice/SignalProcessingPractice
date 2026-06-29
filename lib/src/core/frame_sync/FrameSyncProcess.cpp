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
          observer_count_(other.observer_count_),
          pending_acquire_(other.pending_acquire_),
          pending_pre_process_(other.pending_pre_process_),
          pending_overlap_(other.pending_overlap_),
          pending_window_(other.pending_window_),
          pending_fft_(other.pending_fft_),
          pending_infer_(other.pending_infer_),
          pending_post_process_(other.pending_post_process_),
          pending_overlap_add_(other.pending_overlap_add_),
          pending_output_(other.pending_output_),
          pending_acquire_flag_(other.pending_acquire_flag_.load(std::memory_order_relaxed)),
          pending_pre_process_flag_(
              other.pending_pre_process_flag_.load(std::memory_order_relaxed)),
          pending_overlap_flag_(other.pending_overlap_flag_.load(std::memory_order_relaxed)),
          pending_window_flag_(other.pending_window_flag_.load(std::memory_order_relaxed)),
          pending_fft_flag_(other.pending_fft_flag_.load(std::memory_order_relaxed)),
          pending_infer_flag_(other.pending_infer_flag_.load(std::memory_order_relaxed)),
          pending_post_process_flag_(
              other.pending_post_process_flag_.load(std::memory_order_relaxed)),
          pending_overlap_add_flag_(
              other.pending_overlap_add_flag_.load(std::memory_order_relaxed)),
          pending_output_flag_(other.pending_output_flag_.load(std::memory_order_relaxed)) {}

    auto operator=(const Impl& other) noexcept -> Impl& {
        if (this != &other) {
            pipeline_ = other.pipeline_;
            result_buffer_ = other.result_buffer_;
            seq_.store(other.seq_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            observers_ = other.observers_;
            observer_count_ = other.observer_count_;
            pending_acquire_ = other.pending_acquire_;
            pending_pre_process_ = other.pending_pre_process_;
            pending_overlap_ = other.pending_overlap_;
            pending_window_ = other.pending_window_;
            pending_fft_ = other.pending_fft_;
            pending_infer_ = other.pending_infer_;
            pending_post_process_ = other.pending_post_process_;
            pending_overlap_add_ = other.pending_overlap_add_;
            pending_output_ = other.pending_output_;
            pending_acquire_flag_.store(
                other.pending_acquire_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_pre_process_flag_.store(
                other.pending_pre_process_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_overlap_flag_.store(
                other.pending_overlap_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_window_flag_.store(
                other.pending_window_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_fft_flag_.store(
                other.pending_fft_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_infer_flag_.store(
                other.pending_infer_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_post_process_flag_.store(
                other.pending_post_process_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_overlap_add_flag_.store(
                other.pending_overlap_add_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
            pending_output_flag_.store(
                other.pending_output_flag_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
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
    [[nodiscard]] auto observers() -> std::array<ObserverDelegate, kMaxObservers>& {
        return observers_;
    }
    [[nodiscard]] auto observer_count() -> std::size_t& { return observer_count_; }
    [[nodiscard]] auto observer_count() const -> std::size_t { return observer_count_; }

    [[nodiscard]] auto pending_acquire() -> AudioAquireStrategy& { return pending_acquire_; }
    [[nodiscard]] auto pending_pre_process() -> PreProcessStrategy& {
        return pending_pre_process_;
    }
    [[nodiscard]] auto pending_overlap() -> OverlapStrategy& { return pending_overlap_; }
    [[nodiscard]] auto pending_window() -> WindowStrategy& { return pending_window_; }
    [[nodiscard]] auto pending_fft() -> FftStrategy& { return pending_fft_; }
    [[nodiscard]] auto pending_infer() -> InferStrategy& { return pending_infer_; }
    [[nodiscard]] auto pending_post_process() -> PostProcessStrategy& {
        return pending_post_process_;
    }
    [[nodiscard]] auto pending_overlap_add() -> OverlapAddStrategy& {
        return pending_overlap_add_;
    }
    [[nodiscard]] auto pending_output() -> AudioOutputStrategy& { return pending_output_; }

    [[nodiscard]] auto pending_acquire_flag() -> std::atomic<bool>& {
        return pending_acquire_flag_;
    }
    [[nodiscard]] auto pending_pre_process_flag() -> std::atomic<bool>& {
        return pending_pre_process_flag_;
    }
    [[nodiscard]] auto pending_overlap_flag() -> std::atomic<bool>& {
        return pending_overlap_flag_;
    }
    [[nodiscard]] auto pending_window_flag() -> std::atomic<bool>& { return pending_window_flag_; }
    [[nodiscard]] auto pending_fft_flag() -> std::atomic<bool>& { return pending_fft_flag_; }
    [[nodiscard]] auto pending_infer_flag() -> std::atomic<bool>& { return pending_infer_flag_; }
    [[nodiscard]] auto pending_post_process_flag() -> std::atomic<bool>& {
        return pending_post_process_flag_;
    }
    [[nodiscard]] auto pending_overlap_add_flag() -> std::atomic<bool>& {
        return pending_overlap_add_flag_;
    }
    [[nodiscard]] auto pending_output_flag() -> std::atomic<bool>& { return pending_output_flag_; }

private:
    PipelineContext pipeline_;
    PipelineResult result_buffer_;
    std::atomic<uint32_t> seq_{0};
    std::array<ObserverDelegate, kMaxObservers> observers_{};
    std::size_t observer_count_{0};

    ///
    /// @name ペンディングスロット.
    ///
    /// SetConfig() から書き込まれ, ProcessFrame() の先頭でフレーム境界に適用される.
    /// @{
    AudioAquireStrategy pending_acquire_;
    PreProcessStrategy  pending_pre_process_;
    OverlapStrategy     pending_overlap_;
    WindowStrategy      pending_window_;
    FftStrategy         pending_fft_;
    InferStrategy       pending_infer_;
    PostProcessStrategy pending_post_process_;
    OverlapAddStrategy  pending_overlap_add_;
    AudioOutputStrategy pending_output_;
    /// @}

    ///
    /// @name 適用フラグ.
    ///
    /// SetConfig() が release で true に設定し, ProcessFrame() が acquire で exchange する.
    /// @{
    std::atomic<bool> pending_acquire_flag_{false};
    std::atomic<bool> pending_pre_process_flag_{false};
    std::atomic<bool> pending_overlap_flag_{false};
    std::atomic<bool> pending_window_flag_{false};
    std::atomic<bool> pending_fft_flag_{false};
    std::atomic<bool> pending_infer_flag_{false};
    std::atomic<bool> pending_post_process_flag_{false};
    std::atomic<bool> pending_overlap_add_flag_{false};
    std::atomic<bool> pending_output_flag_{false};
    /// @}
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
void FrameSyncProcess::Attach(ObserverDelegate delegate) {
    auto& impl = *ImplPtr();
    if (impl.observer_count() < kMaxObservers) {
        impl.observers().at(impl.observer_count()) = delegate;
        ++impl.observer_count();
    }
}

void FrameSyncProcess::Detach(ObserverDelegate delegate) {
    auto& impl = *ImplPtr();
    const auto count = impl.observer_count();
    for (std::size_t i = 0; i < count; ++i) {
        if (impl.observers().at(i) == delegate) {
            for (std::size_t j = i; j < count - 1U; ++j) {
                impl.observers().at(j) = impl.observers().at(j + 1U);
            }
            impl.observers().at(count - 1U) = ObserverDelegate{};
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
    auto& impl = *ImplPtr();
    impl.pending_acquire() = std::move(strategy);
    impl.pending_acquire_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] PreProcessTag tag,
                                  PreProcessStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_pre_process() = std::move(strategy);
    impl.pending_pre_process_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OverlapTag tag, OverlapStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_overlap() = std::move(strategy);
    impl.pending_overlap_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] WindowTag tag, WindowStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_window() = std::move(strategy);
    impl.pending_window_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] FftTag tag, FftStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_fft() = std::move(strategy);
    impl.pending_fft_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] InferTag tag, InferStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_infer() = std::move(strategy);
    impl.pending_infer_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] PostProcessTag tag,
                                  PostProcessStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_post_process() = std::move(strategy);
    impl.pending_post_process_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OverlapAddTag tag, OverlapAddStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_overlap_add() = std::move(strategy);
    impl.pending_overlap_add_flag().store(true, std::memory_order_release);
}
void FrameSyncProcess::SetConfig([[maybe_unused]] OutputTag tag, AudioOutputStrategy strategy) {
    auto& impl = *ImplPtr();
    impl.pending_output() = std::move(strategy);
    impl.pending_output_flag().store(true, std::memory_order_release);
}

void FrameSyncProcess::ProcessFrame() {
    auto& impl = *ImplPtr();

    bool any_applied = false;
    if (impl.pending_acquire_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetAquireStrategy(std::move(impl.pending_acquire()));
        any_applied = true;
    }
    if (impl.pending_pre_process_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetPreProcessStrategy(std::move(impl.pending_pre_process()));
        any_applied = true;
    }
    if (impl.pending_overlap_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetOverlapStrategy(std::move(impl.pending_overlap()));
        any_applied = true;
    }
    if (impl.pending_window_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetWindowStrategy(std::move(impl.pending_window()));
        any_applied = true;
    }
    if (impl.pending_fft_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetFftStrategy(std::move(impl.pending_fft()));
        any_applied = true;
    }
    if (impl.pending_infer_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetInferStrategy(std::move(impl.pending_infer()));
        any_applied = true;
    }
    if (impl.pending_post_process_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetPostProcessStrategy(std::move(impl.pending_post_process()));
        any_applied = true;
    }
    if (impl.pending_overlap_add_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetOverlapAddStrategy(std::move(impl.pending_overlap_add()));
        any_applied = true;
    }
    if (impl.pending_output_flag().exchange(false, std::memory_order_acquire)) {
        impl.pipeline().SetOutputStrategy(std::move(impl.pending_output()));
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
