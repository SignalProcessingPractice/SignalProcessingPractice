///
/// @file   main.c
/// @brief  Windows App のメイン関数.
/// @todo   要リファクタリング
///

#include <RtAudio.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <span>
#include <thread>
#include <vector>

static constexpr size_t kRingBufferSize = 48000U;
static constexpr unsigned int kSampleRate = 48000U;
static constexpr unsigned int kBufferFrames = 256U;
static constexpr size_t kSenderBufferSize = 1024U;

// ロックフリーなSPSCリングバッファ
class LockFreeRingBuffer {
private:
    mutable std::vector<float> buffer_;
    mutable std::atomic<size_t> write_idx_{0};
    mutable std::atomic<size_t> read_idx_{0};
    size_t capacity_{0};

public:
    explicit LockFreeRingBuffer(size_t capacity) : buffer_(capacity + 1), capacity_(capacity + 1) {
    }

    auto write(const float* data, size_t count) const -> size_t {
        const auto write_pos = write_idx_.load(std::memory_order_relaxed);
        const auto read_pos = read_idx_.load(std::memory_order_acquire);

        const size_t available = (read_pos + capacity_ - write_pos - 1) % capacity_;
        const size_t to_write = std::min(count, available);

        const auto data_span = std::span<const float>(data, count);
        for (size_t i = 0; i < to_write; ++i) {
            buffer_[(write_pos + i) % capacity_] = data_span[i];
        }

        write_idx_.store((write_pos + to_write) % capacity_, std::memory_order_release);
        return to_write;
    }

    auto read(float* data, size_t count) const -> size_t {
        const auto read_pos = read_idx_.load(std::memory_order_relaxed);
        const auto write_pos = write_idx_.load(std::memory_order_acquire);

        const size_t available = (write_pos + capacity_ - read_pos) % capacity_;
        const size_t to_read = std::min(count, available);

        const auto data_span = std::span<float>(data, count);
        for (size_t i = 0; i < to_read; ++i) {
            data_span[i] = buffer_[(read_pos + i) % capacity_];
        }

        read_idx_.store((read_pos + to_read) % capacity_, std::memory_order_release);
        return to_read;
    }
};

struct AppContext {
    LockFreeRingBuffer in_buffer{kRingBufferSize};
    LockFreeRingBuffer out_buffer{kRingBufferSize};
    std::atomic<bool> running{true};
};

//==============================
// 入力コールバック（Producer）
//==============================
auto inputCallback(void* /*outputBuffer*/, void* inputBuffer, unsigned int nFrames,
                   double /*streamTime*/, RtAudioStreamStatus status, void* userData) -> int {
    if (status != 0U) {
        std::cerr << "Stream underflow/overflow\n";
    }

    auto* input_ptr = static_cast<float*>(inputBuffer);
    if (input_ptr == nullptr) {
        return 0;
    }

    auto* ctx = static_cast<AppContext*>(userData);
    ctx->in_buffer.write(input_ptr, nFrames);
    return 0;
}

//==============================
// 出力コールバック（Consumer）
//==============================
auto outputCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nFrames,
                    double /*streamTime*/, RtAudioStreamStatus /*status*/, void* userData) -> int {
    auto* output_ptr = static_cast<float*>(outputBuffer);
    auto* ctx = static_cast<AppContext*>(userData);

    const auto output_span = std::span<float>(output_ptr, nFrames);
    const size_t readCount = ctx->out_buffer.read(output_ptr, nFrames);

    if (readCount < nFrames) {
        for (size_t i = readCount; i < nFrames; ++i) {
            output_span[i] = 0.0F;
        }
    }
    return 0;
}

//==============================
// 中継スレッド
//==============================
auto senderThread(AppContext* ctx) -> void {
    std::vector<float> tempBuffer(kSenderBufferSize);

    while (ctx->running) {
        size_t readCount = ctx->in_buffer.read(tempBuffer.data(), tempBuffer.size());

        if (readCount > 0) {
            ctx->out_buffer.write(tempBuffer.data(), readCount);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

//==============================
// main
//==============================
auto main() -> int {
    std::cout << "HELLO!" << "\n";

    RtAudio adc;

    std::cout << "getDeviceCount=" << adc.getDeviceCount() << "\n";

    if (adc.getDeviceCount() < 1) {
        std::cerr << "No audio devices found\n";
        return 1;
    }

    AppContext ctx;

    RtAudio::StreamParameters iParams;
    RtAudio::StreamParameters oParams;

    iParams.deviceId = adc.getDefaultInputDevice();
    iParams.nChannels = 1;
    iParams.firstChannel = 0;

    oParams.deviceId = adc.getDefaultOutputDevice();
    oParams.nChannels = 1;
    oParams.firstChannel = 0;

    unsigned int sampleRate = kSampleRate;
    unsigned int bufferFrames = kBufferFrames;

    try {
        adc.openStream(nullptr, &iParams, RTAUDIO_FLOAT32, sampleRate, &bufferFrames,
                       &inputCallback, &ctx);
        adc.startStream();

        RtAudio dac;
        dac.openStream(&oParams, nullptr, RTAUDIO_FLOAT32, sampleRate, &bufferFrames,
                       &outputCallback, &ctx);
        dac.startStream();

        std::thread sender_thread(senderThread, &ctx);

        std::cout << "Running... press Enter to stop\n";
        std::cin.get();

        ctx.running = false;
        sender_thread.join();

        adc.stopStream();
        dac.stopStream();

        adc.closeStream();
        dac.closeStream();
    } catch (...) {
        std::cerr << "RtAudio error\n";
        return 1;
    }

    return 0;
}
