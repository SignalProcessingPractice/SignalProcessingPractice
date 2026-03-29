#include <RtAudio.h>

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

struct AudioBuffer {
    std::vector<float> data;
};

// 共有キュー
std::queue<AudioBuffer> g_queue;
std::mutex g_mutex;
std::condition_variable g_cv;
std::atomic<bool> g_running{true};

// コールバック（入力専用）
int inputCallback(void* /*outputBuffer*/,
                  void* inputBuffer,
                  unsigned int nFrames,
                  double /*streamTime*/,
                  RtAudioStreamStatus status,
                  void* /*userData*/)
{
    if (status) {
        std::cerr << "Stream underflow/overflow\n";
    }

    float* in = static_cast<float*>(inputBuffer);
    if (!in) return 0;

    AudioBuffer buf;
    buf.data.assign(in, in + nFrames); // モノラル前提

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_queue.push(std::move(buf));
    }
    g_cv.notify_one();

    return 0;
}

// 送信スレッド
void senderThread()
{
    while (g_running) {
        std::unique_lock<std::mutex> lock(g_mutex);
        g_cv.wait(lock, [] {
            return !g_queue.empty() || !g_running;
        });

        while (!g_queue.empty()) {
            auto buf = std::move(g_queue.front());
            g_queue.pop();
            lock.unlock();

            // ===== ここで送信処理 =====
            // 例：そのまま出力
            // std::cout << "Send " << buf.data.size() << " samples\n";

            lock.lock();
        }
    }
}

int main()
{
    RtAudio adc;

    if (adc.getDeviceCount() < 1) {
        std::cerr << "No audio devices found\n";
        return 1;
    }

    RtAudio::StreamParameters iParams;
    iParams.deviceId = adc.getDefaultInputDevice();
    iParams.nChannels = 1;
    iParams.firstChannel = 0;

    unsigned int sampleRate = 48000;
    unsigned int bufferFrames = 256;

    try {
        adc.openStream(
            nullptr,                // outputなし
            &iParams,
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            &inputCallback
        );

        adc.startStream();
    }
    catch (...) {
        std::cerr << "RtAudio error" << std::endl;
        return 1;
    }

    std::thread th(senderThread);

    std::cout << "Running... press Enter to stop\n";
    std::cin.get();

    g_running = false;
    g_cv.notify_all();
    th.join();

    try {
        adc.stopStream();
    }
    catch (...) {}

    if (adc.isStreamOpen()) {
        adc.closeStream();
    }

    return 0;
}