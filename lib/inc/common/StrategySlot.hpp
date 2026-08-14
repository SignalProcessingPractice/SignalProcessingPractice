///
/// @file StrategySlot.hpp
///
#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <memory>

///
/// @brief Exec() と Reset() を実装する型を制約する Concept.
///
template <typename T, typename R, typename... Args>
concept StrategyModel = requires(T& obj, Args... args) {
    { obj.Exec(args...) } -> std::same_as<R>;
    { obj.Reset() } -> std::same_as<void>;
};

///
/// @brief ヒープフリーな Type Erasure Strategy ラッパー.
///
/// Concept/Model パターンにより, Exec() と Reset() を持つ任意の型を型消去する.
/// Model を in-place storage に配置するため動的メモリ確保を行わない.
///
template <typename Signature>
class StrategySlot;

template <typename R, typename... Args>
class StrategySlot<R(Args...)> {
    ///
    /// @brief 抽象インターフェース (Concept).
    ///
    struct IModel {
        IModel() = default;
        IModel(const IModel&) = delete;
        auto operator=(const IModel&) -> IModel& = delete;
        IModel(IModel&&) = delete;
        auto operator=(IModel&&) -> IModel& = delete;
        virtual ~IModel() = default;

        virtual auto exec(Args... args) -> R = 0;
        virtual auto reset() -> void = 0;
        virtual auto copy_to(std::byte* dest) const noexcept -> void = 0;
    };

    ///
    /// @brief 具象ラッパー (Model).
    ///
    template <typename T>
    struct Model final : IModel {
        explicit Model(T* obj) noexcept
            : obj_(obj)
        {
        }

        auto exec(Args... args) -> R override
        {
            return obj_->Exec(args...);
        }
        auto reset() -> void override
        {
            obj_->Reset();
        }
        auto copy_to(std::byte* dest) const noexcept -> void override
        {
            std::construct_at(std::bit_cast<Model*>(dest), obj_);
        }

    private:
        T* obj_;
    };

    static constexpr std::size_t kStorageSize = sizeof(void*) * 4;
    alignas(alignof(std::max_align_t)) std::array<std::byte, kStorageSize> storage_{};
    IModel* model_{nullptr};

    auto copy_from(const StrategySlot& other) noexcept -> void
    {
        if (other.model_ != nullptr) {
            other.model_->copy_to(storage_.data());
            model_ = std::launder(std::bit_cast<IModel*>(storage_.data()));
        }
    }

public:
    StrategySlot() = default;

    template <typename T>
        requires StrategyModel<T, R, Args...>
    explicit StrategySlot(T* obj)
    {
        bind(obj);
    }

    StrategySlot(const StrategySlot& other) noexcept
    {
        copy_from(other);
    }

    StrategySlot(StrategySlot&& other) noexcept
    {
        copy_from(other);
    }

    auto operator=(const StrategySlot& other) noexcept -> StrategySlot&
    {
        if (this != &other) {
            if (model_ != nullptr) {
                std::destroy_at(model_);
                model_ = nullptr;
            }
            copy_from(other);
        }
        return *this;
    }

    auto operator=(StrategySlot&& other) noexcept -> StrategySlot&
    {
        if (this != &other) {
            if (model_ != nullptr) {
                std::destroy_at(model_);
                model_ = nullptr;
            }
            copy_from(other);
        }
        return *this;
    }

    ~StrategySlot()
    {
        if (model_ != nullptr) {
            std::destroy_at(model_);
        }
    }

    ///
    /// @brief 具象 Strategy をバインドする.
    ///
    template <typename T>
        requires StrategyModel<T, R, Args...>
    auto bind(T* obj) -> void
    {
        static_assert(sizeof(Model<T>) <= kStorageSize, "Model<T> exceeds StrategySlot storage");
        if (model_ != nullptr) {
            std::destroy_at(model_);
            model_ = nullptr;
        }
        model_ = std::construct_at(std::bit_cast<Model<T>*>(storage_.data()), obj);
    }

    ///
    /// @brief Strategy を実行する.
    ///
    auto operator()(Args... args) -> R
    {
        return model_->exec(args...);
    }

    ///
    /// @brief Strategy の内部状態をリセットする.
    ///
    auto reset() -> void
    {
        if (model_ != nullptr) {
            model_->reset();
        }
    }
};
