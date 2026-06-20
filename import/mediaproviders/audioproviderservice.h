/*
 * Copyright 2023 by Aditya Mehra <aix.m@outlook.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef AUDIOPROVIDERSERVICE_H
#define AUDIOPROVIDERSERVICE_H

#include <QObject>
#include <QUrl>
#include <QAudioSink>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QEventLoop>
#include "audiostreamdevice.h"

class AudioProviderService : public QObject
{
    Q_OBJECT

public:
    enum PlaybackState {
        PlayingState = 1,
        PausedState = 2,
        StoppedState = 0
    };

    enum MediaState {
        NoMedia = 0,
        LoadingMedia = 1,
        LoadedMedia = 2,
        StalledMedia = 3,
        BufferingMedia = 4,
        BufferedMedia = 5,
        EndOfMedia = 6,
        InvalidMedia = 7
    };

    explicit AudioProviderService(QObject *parent = nullptr);
    ~AudioProviderService();
    void syncStates();
    void mediaPlay(const QUrl &url);
    void mediaStop();
    void mediaPause();
    void mediaContinue();
    void mediaRestart();
    void mediaSeek(qint64 seekValue);
    qint64 position() const { return m_position; }

public Q_SLOTS:
    void notifyBufferingMedia();
    void notifyBufferedMedia();
    void notifyStalledMedia();
    void notifyEndOfMedia();
    void notifyInvalidMedia();
    void spectrumUpdated(QVector<double> spectrum);
    void durationUpdated(qint64 duration);
    void positionUpdated(qint64 position);

Q_SIGNALS:
    int levels(double left, double right);
    void spectrumChanged(QVector<double> spectrum);
    void playBackStateChanged(AudioProviderService::PlaybackState playbackState);
    void mediaStateChanged(AudioProviderService::MediaState mediaState);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void destroyedService();

private:
    AudioStreamDevice *m_audioStreamDevice;
    AudioProviderService::PlaybackState m_currentPlaybackState;
    AudioProviderService::MediaState m_currentMediaState;
    QAudioSink *m_audioSink;
    QAudioFormat m_format;
    QUrl m_currentMediaUrl;
    QVector<double> m_spectrum;
    qint64 m_position = 0;
};

#endif //AUDIOPROVIDERSERVICE_H
