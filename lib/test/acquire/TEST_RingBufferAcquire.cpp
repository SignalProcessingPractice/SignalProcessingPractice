#include <span>
#include <stop_token>
#include <vector>

#include <gtest/gtest.h>

#include "Strategies/RingBufferAcquire.hpp"

namespace {

///
/// @brief RingBufferModel Concept を満たすモックリングバッファ.
///
class MockRingBuffer {
public:
    auto Push(std::span<const float> samples) -> bool
    {
        push_called_ = true;
        pushed_samples_.assign(samples.begin(), samples.end());
        return push_return_;
    }

    auto PopHop(FrameSyncProcess::AudioHop* hop) -> void
    {
        pop_hop_called_ = true;
        for (std::size_t i = 0; i < hop->size(); ++i) {
            (*hop)[i] = static_cast<float>(i) * fill_scale_;
        }
    }

    auto WaitForHop([[maybe_unused]] const std::stop_token& token) -> bool
    {
        wait_for_hop_called_ = true;
        return wait_return_;
    }

    auto Clear() -> void
    {
        clear_called_ = true;
    }

    auto SetPushReturn(bool value) -> void
    {
        push_return_ = value;
    }
    auto SetWaitReturn(bool value) -> void
    {
        wait_return_ = value;
    }

    [[nodiscard]] auto push_called() const -> bool
    {
        return push_called_;
    }
    [[nodiscard]] auto pop_hop_called() const -> bool
    {
        return pop_hop_called_;
    }
    [[nodiscard]] auto wait_for_hop_called() const -> bool
    {
        return wait_for_hop_called_;
    }
    [[nodiscard]] auto clear_called() const -> bool
    {
        return clear_called_;
    }
    [[nodiscard]] auto pushed_samples() const -> const std::vector<float>&
    {
        return pushed_samples_;
    }
    [[nodiscard]] auto fill_scale() const -> float
    {
        return fill_scale_;
    }

private:
    bool push_return_ = true;
    bool wait_return_ = true;
    float fill_scale_ = 0.001F;

    bool push_called_ = false;
    bool pop_hop_called_ = false;
    bool wait_for_hop_called_ = false;
    bool clear_called_ = false;

    std::vector<float> pushed_samples_;
};

}  // namespace

TEST(RingBufferAcquireExec, DelegatesToPopHop)
{
    MockRingBuffer mock;
    RingBufferAcquire acquire{&mock};

    const auto hop = acquire.Exec();

    EXPECT_TRUE(mock.pop_hop_called());
    for (std::size_t i = 0; i < hop.size(); ++i) {
        EXPECT_FLOAT_EQ(hop[i], static_cast<float>(i) * mock.fill_scale());
    }
}

TEST(RingBufferAcquireReset, DelegatesToClear)
{
    MockRingBuffer mock;
    RingBufferAcquire acquire{&mock};

    acquire.Reset();

    EXPECT_TRUE(mock.clear_called());
}

TEST(RingBufferAcquirePush, DelegatesToPushAndReturnsResult)
{
    MockRingBuffer mock;
    mock.SetPushReturn(false);
    RingBufferAcquire acquire{&mock};

    const std::vector<float> samples{1.0F, 2.0F, 3.0F};
    const bool result = acquire.Push(samples);

    EXPECT_TRUE(mock.push_called());
    EXPECT_EQ(mock.pushed_samples(), samples);
    EXPECT_FALSE(result);
}

TEST(RingBufferAcquireWaitForHop, DelegatesToWaitForHopAndReturnsResult)
{
    MockRingBuffer mock;
    mock.SetWaitReturn(false);
    RingBufferAcquire acquire{&mock};

    const bool result = acquire.WaitForHop(std::stop_token{});

    EXPECT_TRUE(mock.wait_for_hop_called());
    EXPECT_FALSE(result);
}
