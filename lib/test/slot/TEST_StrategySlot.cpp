#include <gtest/gtest.h>

#include "StrategySlot.hpp"

class ResetSpy {
public:
    static auto Exec(int val) -> int
    {
        return val;
    }
    auto Reset() -> void
    {
        ++reset_count_;
    }
    [[nodiscard]] auto reset_count() const -> int
    {
        return reset_count_;
    }

private:
    int reset_count_{0};
};

TEST(StrategySlotReset, DispatchesToBoundStrategy)
{
    ResetSpy spy;
    StrategySlot<int(int)> slot{&spy};

    slot.reset();
    EXPECT_EQ(spy.reset_count(), 1);

    slot.reset();
    EXPECT_EQ(spy.reset_count(), 2);
}

TEST(StrategySlotReset, DoesNotCrashWhenUnbound)
{
    StrategySlot<int(int)> slot;
    EXPECT_NO_FATAL_FAILURE(slot.reset());
}

TEST(StrategySlotExec, DispatchesToBoundStrategy)
{
    ResetSpy spy;
    StrategySlot<int(int)> slot{&spy};
    EXPECT_EQ(slot(42), 42);
}
