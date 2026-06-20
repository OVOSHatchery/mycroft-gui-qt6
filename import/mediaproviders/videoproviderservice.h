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

#ifndef VIDEOPROVIDERSERVICE_H
#define VIDEOPROVIDERSERVICE_H

#include <QObject>
#include <QUrl>
#include <QMediaPlayer>
#include <QMediaDevices>
#include <QAudioOutput>
#include <QAudioDevice>
#include <QVideoSink>
#include <QAudioFormat>


class VideoProviderService : public QObject
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

    explicit VideoProviderService(QObject *parent = nullptr);
    ~VideoProviderService();
    void syncStates();
    void mediaPlay(const QUrl &url);
    void mediaStop();
    void mediaPause();
    void mediaContinue();
    void mediaRestart();
    void mediaSeek(qint64 seekValue);
    qint64 position() const { return m_position; }
    QVideoSink *videoSink() const;
    QObject *videoOutput() const;
    void setVideoSink(QVideoSink *videoSink);
    void setVideoOutput(QObject *videoOutput);


public Q_SLOTS:
    void updatePlaybackState(QMediaPlayer::PlaybackState state);
    void updateMediaState(QMediaPlayer::MediaStatus status);
    void durationUpdated(qint64 duration);
    void positionUpdated(qint64 position);
    void errorOccurred(QMediaPlayer::Error error);

signals:
    void signalVideoSinkChanged();
    void signalVideoOutputChanged();

Q_SIGNALS:
    void playBackStateChanged(VideoProviderService::PlaybackState playbackState);
    void mediaStateChanged(VideoProviderService::MediaState mediaState);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void destroyedService();

private:
    VideoProviderService::PlaybackState m_currentPlaybackState;
    VideoProviderService::MediaState m_currentMediaState;
    QVideoSink *m_videoSink;
    QObject *m_videoOutput;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QUrl m_currentMediaUrl;
    QAudioFormat m_format;
    qint64 m_position = 0;
};

#endif //AUDIOPROVIDERSERVICE_H
