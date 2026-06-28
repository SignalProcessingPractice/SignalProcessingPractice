///
/// @file null_strategies.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

struct NullInput {
    static auto Exec() -> FrameSyncProcess::AudioHop;
    static auto Reset() -> void;
};

struct ThroughPreProcess {
    static auto Exec(const FrameSyncProcess::AudioHop& frame) -> FrameSyncProcess::AudioHop;
    static auto Reset() -> void;
};

struct ThroughPostProcess {
    static auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    static auto Reset() -> void;
};

struct NullOverlap {
    static auto Exec(const FrameSyncProcess::AudioHop& frame) -> FrameSyncProcess::AudioFrame;
    static auto Reset() -> void;
};

struct NullWindow {
    static auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    static auto Reset() -> void;
};

struct NullFft {
    static auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    static auto Reset() -> void;
};

struct ThroughInfer {
    static auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    static auto Reset() -> void;
};

struct NullInfer {
    static auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    static auto Reset() -> void;
};

struct NullOverlapAdd {
    static auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioHop;
    static auto Reset() -> void;
};

struct NullOutput {
    static auto Exec(const FrameSyncProcess::AudioHop& frame) -> void;
    static auto Reset() -> void;
};
