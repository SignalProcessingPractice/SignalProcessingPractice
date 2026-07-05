///
/// @file DeviceInput.h
///
#pragma once

#include <QAudioFormat>
#include <QAudioSource>
#include <QIODevice>
#include <memory>
#include <vector>

class AudioInputBuffer;

///
/// @brief QAudioSource (push モード) の書き込み先として AudioInputBuffer へ転送する QIODevice.
///
/// writeData() で受信したサンプルを float へ変換し, AudioInputBuffer::Push() する.
/// push モードを使用することで, シグナル (readyRead) の購読なしにデータを受け取る.
///
class AudioPushDevice : public QIODevice {
    Q_OBJECT

public:
    explicit AudioPushDevice(AudioInputBuffer* buffer);

    ///
    /// 受信データのサンプルフォーマットを設定する (Float / Int16 のみ対応).
    ///
    void SetSampleFormat(QAudioFormat::SampleFormat format);

protected:
    auto readData(char* data, qint64 max_size) -> qint64 override;
    auto writeData(const char* data, qint64 size) -> qint64 override;

private:
    AudioInputBuffer* buffer_;
    QAudioFormat::SampleFormat sample_format_{QAudioFormat::Float};

    ///
    /// Int16 → float 変換用の作業バッファ.
    ///
    std::vector<float> convert_buffer_;
};

///
/// @brief デフォルト入力デバイスからの音声キャプチャ (Producer).
///
/// Qt Multimedia (QAudioSource) を使用する. Qt イベントループ上で動作するため,
/// AudioInputBuffer への Push はメインスレッドから行われる.
/// InputSource とは排他的に動作させること (SPSC の Producer は常に 1 つ).
///
class DeviceInput {
public:
    explicit DeviceInput(AudioInputBuffer* buffer);
    ~DeviceInput();

    DeviceInput(const DeviceInput&) = delete;
    auto operator=(const DeviceInput&) -> DeviceInput& = delete;
    DeviceInput(DeviceInput&&) = delete;
    auto operator=(DeviceInput&&) -> DeviceInput& = delete;

    ///
    /// デフォルト入力デバイスでキャプチャを開始する.
    ///
    /// @return 開始できれば true. デバイスなし・フォーマット非対応・開始失敗時は
    ///         警告を stderr へ出力して false.
    ///
    auto Start() -> bool;

    ///
    /// キャプチャを停止する.
    ///
    void Stop();

private:
    AudioPushDevice push_device_;
    std::unique_ptr<QAudioSource> audio_source_;
};
