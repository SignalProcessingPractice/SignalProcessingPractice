///
/// @file   main.c
/// @brief  Windows App のメイン関数.
/// @todo   要リファクタリング
///

#include <RtAudio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <cstring>
#include <chrono>
#include <algorithm>

// ロックフリーなSPSCリングバッファ
class LockFreeRingBuffer {
private:
    std::vector<float> buffer_;
    std::atomic<size_t> write_idx_{0};
    std::atomic<size_t> read_idx_{0};
    size_t capacity_;

public:
    LockFreeRingBuffer(size_t capacity) : buffer_(capacity + 1), capacity_(capacity + 1) {}

    size_t write(const float* data, size_t count) {
        size_t write_pos = write_idx_.load(std::memory_order_relaxed);
        size_t read_pos = read_idx_.load(std::memory_order_acquire);
        
        size_t available = (read_pos + capacity_ - write_pos - 1) % capacity_;
        size_t to_write = std::min(count, available);

        // 簡略化のため1サンプルずつコピー（実運用ではラップアラウンドを考慮したmemcpyが高速です）
        for (size_t i = 0; i < to_write; ++i) {
            buffer_[(write_pos + i) % capacity_] = data[i];
        }

        write_idx_.store((write_pos + to_write) % capacity_, std::memory_order_release);
        return to_write;
    }

    size_t read(float* data, size_t count) {
        size_t read_pos = read_idx_.load(std::memory_order_relaxed);
        size_t write_pos = write_idx_.load(std::memory_order_acquire);

        size_t available = (write_pos + capacity_ - read_pos) % capacity_;
        size_t to_read = std::min(count, available);

        for (size_t i = 0; i < to_read; ++i) {
            data[i] = buffer_[(read_pos + i) % capacity_];
        }

        read_idx_.store((read_pos + to_read) % capacity_, std::memory_order_release);
        return to_read;
    }
};

// バッファサイズは余裕を持たせて確保（例: 48000サンプル = 1秒分）
LockFreeRingBuffer g_inBuffer(48000);
LockFreeRingBuffer g_outBuffer(48000);

std::atomic<bool> g_running{true};

//==============================
// 入力コールバック（Producer）
//==============================
int inputCallback(void*, void* inputBuffer, unsigned int nFrames, double, RtAudioStreamStatus status, void*) {
    if (status) std::cerr << "Stream underflow/overflow\n";

    float* in = static_cast<float*>(inputBuffer);
    if (!in) return 0;

    // ロックなし、アロケーションなしで直接書き込む
    g_inBuffer.write(in, nFrames);
    return 0;
}

//==============================
// 出力コールバック（Consumer）
//==============================
int outputCallback(void* outputBuffer, void*, unsigned int nFrames, double, RtAudioStreamStatus, void*) {
    float* out = static_cast<float*>(outputBuffer);

    // 読み込める分だけ読み込む
    size_t readCount = g_outBuffer.read(out, nFrames);

    // 足りない分は0埋め（無音を出力してノイズを防ぐ）
    if (readCount < nFrames) {
        std::memset(out + readCount, 0, sizeof(float) * (nFrames - readCount));
    }
    return 0;
}

//==============================
// 中継スレッド
//==============================
void senderThread() {
    std::vector<float> tempBuffer(1024); // 一時的な作業用バッファ
    
    while (g_running) {
        size_t readCount = g_inBuffer.read(tempBuffer.data(), tempBuffer.size());
        
        if (readCount > 0) {
            // ===== ここで音声処理を行う =====
            // 例：そのまま通す
            g_outBuffer.write(tempBuffer.data(), readCount);
        } else {
            // CPUを100%専有しないように、データがない時は少しスリープ
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

//==============================
// main
//==============================
int main()
{
    std::cout << "HELLO!" << std::endl;

    RtAudio adc;

    std::cout << "getDeviceCount=" << adc.getDeviceCount() << std::endl;

    if (adc.getDeviceCount() < 1) {

        std::cerr << "No audio devices found\n";
        return 1;
    }

    RtAudio::StreamParameters iParams, oParams;

    iParams.deviceId = adc.getDefaultInputDevice();
    iParams.nChannels = 1;
    iParams.firstChannel = 0;

    oParams.deviceId = adc.getDefaultOutputDevice();
    oParams.nChannels = 1;
    oParams.firstChannel = 0;

    unsigned int sampleRate = 48000;
    unsigned int bufferFrames = 256;

    try {
        // 入力ストリーム
        adc.openStream(
            nullptr,
            &iParams,
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            &inputCallback
        );
        adc.startStream();

        // 出力ストリーム（別）
        RtAudio dac;
        dac.openStream(
            &oParams,
            nullptr,
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            &outputCallback
        );
        dac.startStream();

        std::thread th(senderThread);

        std::cout << "Running... press Enter to stop\n";
        std::cin.get();

        g_running = false;
        th.join();

        adc.stopStream();
        dac.stopStream();

        adc.closeStream();
        dac.closeStream();
    }
    catch (...) {
        std::cerr << "RtAudio error\n";
        return 1;
    }

    return 0;
}