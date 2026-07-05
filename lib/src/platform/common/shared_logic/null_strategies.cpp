///
/// @file null_strategies.cpp
///
#include "Strategies/null_strategies.hpp"

#include "FrameSyncProcess.hpp"

auto NullInput::Exec() -> FrameSyncProcess::AudioHop {
    return FrameSyncProcess::AudioHop{};
}
auto NullInput::Reset() -> void {
}

auto ThroughPreProcess::Exec(const FrameSyncProcess::AudioHop& frame)
        -> FrameSyncProcess::AudioHop {
    return frame;
}
auto ThroughPreProcess::Reset() -> void {
}

auto ThroughPostProcess::Exec(const FrameSyncProcess::AudioFrame& frame)
        -> FrameSyncProcess::AudioFrame {
    return frame;
}
auto ThroughPostProcess::Reset() -> void {
}

auto NullOverlap::Exec([[maybe_unused]] const FrameSyncProcess::AudioHop& frame)
        -> FrameSyncProcess::AudioFrame {
    return FrameSyncProcess::AudioFrame{};
}
auto NullOverlap::Reset() -> void {
}

auto NullWindow::Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame {
    return frame;
}
auto NullWindow::Reset() -> void {
}

auto NullFft::Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame {
    return frame;
}
auto NullFft::Reset() -> void {
}

auto ThroughInfer::Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame {
    return frame;
}
auto ThroughInfer::Reset() -> void {
}

auto NullInfer::Exec([[maybe_unused]] const FrameSyncProcess::AudioFrame& frame)
        -> FrameSyncProcess::AudioFrame {
    return FrameSyncProcess::AudioFrame{};
}
auto NullInfer::Reset() -> void {
}

auto NullOverlapAdd::Exec([[maybe_unused]] const FrameSyncProcess::AudioFrame& frame)
        -> FrameSyncProcess::AudioHop {
    return FrameSyncProcess::AudioHop{};
}
auto NullOverlapAdd::Reset() -> void {
}

auto NullOutput::Exec([[maybe_unused]] const FrameSyncProcess::AudioHop& frame) -> void {
}
auto NullOutput::Reset() -> void {
}
