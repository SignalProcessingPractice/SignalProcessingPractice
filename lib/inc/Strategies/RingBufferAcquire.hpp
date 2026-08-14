///
/// @file RingBufferAcquire.hpp
///
#pragma once

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <memory>
#include <span>
#include <stop_token>

#include "FrameSyncProcess.hpp"

///
/// @brief リングバッファの実体が満たすべき Concept.
///
template <typename T>
concept RingBufferModel = requires(T& obj, std::span<const float> samples,
                                   FrameSyncProcess::AudioHop* hop, std::stop_token token) {
    { obj.Push(samples) } -> std::same_as<bool>;
    { obj.PopHop(hop) } -> std::same_as<void>;
    { obj.WaitForHop(token) } -> std::same_as<bool>;
    { obj.Clear() } -> std::same_as<void>;
};

///
/// @brief 外部のリングバッファから 1 ホップ読み出す Acquire Strategy.
///
/// リングバッファの実体は Concept/Model パターンによる型消去で保持する.
/// Push / PopHop / WaitForHop / Clear は, 外部から呼び出し可能な本クラスの公開関数を
/// 経由してのみ実行できる. リングバッファの実体を直接公開しないことで,
/// カプセル化された単一の窓口を提供する.
///
class RingBufferAcquire {
public:
    ///
    /// @brief リングバッファの実体を bind して構築する.
    ///
    /// obj は本クラスより長く生存すること (非所有ポインタ).
    ///
    template <typename T>
        requires RingBufferModel<T>
    explicit RingBufferAcquire(T* obj)
    {
        bind(obj);
    }

    ~RingBufferAcquire()
    {
        std::destroy_at(model_);
    }

    RingBufferAcquire(const RingBufferAcquire&) = delete;
    auto operator=(const RingBufferAcquire&) -> RingBufferAcquire& = delete;
    RingBufferAcquire(RingBufferAcquire&&) = delete;
    auto operator=(RingBufferAcquire&&) -> RingBufferAcquire& = delete;

    ///
    /// @name AudioAcquireStrategy 用 (StrategyModel Concept 準拠).
    /// {@
    auto Exec() -> FrameSyncProcess::AudioHop;
    auto Reset() -> void;
    /// @}

    ///
    /// @name Producer からの唯一の窓口.
    /// {@
    ///
    /// サンプル列を書き込む.
    ///
    auto Push(std::span<const float> samples) -> bool;

    ///
    /// 1 ホップ分溜まるまで待機する.
    ///
    auto WaitForHop(std::stop_token stop_token) -> bool;
    /// @}

private:
    ///
    /// @brief 具象 Model を in-place storage へ配置する.
    ///
    template <typename T>
    auto bind(T* obj) -> void
    {
        static_assert(sizeof(Model<T>) <= kStorageSize,
                      "Model<T> exceeds RingBufferAcquire storage");
        model_ = std::construct_at(std::bit_cast<Model<T>*>(storage_.data()), obj);
    }

    ///
    /// @brief 抽象インターフェース (Concept).
    ///
    struct IRingBuffer {
        IRingBuffer() = default;
        IRingBuffer(const IRingBuffer&) = delete;
        auto operator=(const IRingBuffer&) -> IRingBuffer& = delete;
        IRingBuffer(IRingBuffer&&) = delete;
        auto operator=(IRingBuffer&&) -> IRingBuffer& = delete;
        virtual ~IRingBuffer() = default;

        virtual auto push(std::span<const float> samples) -> bool = 0;
        virtual auto pop_hop(FrameSyncProcess::AudioHop* hop) -> void = 0;
        virtual auto wait_for_hop(std::stop_token token) -> bool = 0;
        virtual auto clear() -> void = 0;
    };

    ///
    /// @brief 具象ラッパー (Model).
    ///
    template <typename T>
    struct Model final : IRingBuffer {
        explicit Model(T* obj) noexcept
            : obj_(obj)
        {
        }

        auto push(std::span<const float> samples) -> bool override
        {
            return obj_->Push(samples);
        }
        auto pop_hop(FrameSyncProcess::AudioHop* hop) -> void override
        {
            obj_->PopHop(hop);
        }
        auto wait_for_hop(std::stop_token token) -> bool override
        {
            return obj_->WaitForHop(std::move(token));
        }
        auto clear() -> void override
        {
            obj_->Clear();
        }

    private:
        T* obj_;
    };

    static constexpr std::size_t kStorageSize = sizeof(void*) * 2;
    alignas(alignof(std::max_align_t)) std::array<std::byte, kStorageSize> storage_{};
    IRingBuffer* model_{nullptr};
};
