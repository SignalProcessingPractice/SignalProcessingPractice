#include <RtAudio.h>

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>

struct AudioBuffer {
    std::vector<float> data;
};

// 入力→処理キュー
std::queue<AudioBuffer> g_inQueue;
// 処理→出力キュー
std::queue<AudioBuffer> g_outQueue;

std::mutex g_mutex;
std::condition_variable g_cv;
std::atomic<bool> g_running{true};

//==============================
// 入力コールバック（producer）
//==============================
int inputCallback(void*,
                  void* inputBuffer,
                  unsigned int nFrames,
                  double,
                  RtAudioStreamStatus status,
                  void*)
{
    if (status) {
        std::cerr << "Stream underflow/overflow\n";
    }

    float* in = static_cast<float*>(inputBuffer);
    if (!in) return 0;

    AudioBuffer buf;
    buf.data.assign(in, in + nFrames);

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_inQueue.push(std::move(buf));
    }
    g_cv.notify_one();

    return 0;
}

//==============================
// 出力コールバック（consumer側の最終段）
//==============================
int outputCallback(void* outputBuffer,
                   void*,
                   unsigned int nFrames,
                   double,
                   RtAudioStreamStatus,
                   void*)
{
    float* out = static_cast<float*>(outputBuffer);

    std::lock_guard<std::mutex> lock(g_mutex);

    if (!g_outQueue.empty()) {
        auto& buf = g_outQueue.front();

        size_t copySize = std::min<size_t>(buf.data.size(), nFrames);
        std::memcpy(out, buf.data.data(), sizeof(float) * copySize);

        if (copySize < nFrames) {
            std::memset(out + copySize, 0, sizeof(float) * (nFrames - copySize));
        }

        g_outQueue.pop();
    } else {
        std::memset(out, 0, sizeof(float) * nFrames);
    }

    return 0;
}

//==============================
// 中継スレッド（consumer）
//==============================
void senderThread()
{
    while (g_running) {
        std::unique_lock<std::mutex> lock(g_mutex);

        g_cv.wait(lock, [] {
            return !g_inQueue.empty() || !g_running;
        });

        while (!g_inQueue.empty()) {
            auto buf = std::move(g_inQueue.front());
            g_inQueue.pop();

            // ===== ここで処理可能 =====
            // 例：そのまま通す

            g_outQueue.push(std::move(buf));
        }
    }
}

//==============================
// main
//==============================
int main()
{
    RtAudio adc;

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
        g_cv.notify_all();
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