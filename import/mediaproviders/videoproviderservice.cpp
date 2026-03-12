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

#include "videoproviderservice.h"
#include <QDebug>

VideoProviderService::VideoProviderService(QObject *parent)
    : QObject(parent)
{
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(QMediaDevices::defaultAudioOutput(), this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    m_format = info.preferredFormat();

    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoProviderService::durationUpdated);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoProviderService::positionUpdated);
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged , this, &VideoProviderService::updatePlaybackState);
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &VideoProviderService::updateMediaState);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, &VideoProviderService::errorOccurred);
}

VideoProviderService::~VideoProviderService()
{
    m_mediaPlayer->deleteLater();
    m_audioOutput->deleteLater();
    emit destroyedService();
}

QVideoSink *VideoProviderService::videoSink() const
{
    return m_videoSink;
}

void VideoProviderService::setVideoSink(QVideoSink *videoSink)
{
    if (videoSink != m_videoSink) {
        m_videoSink = videoSink;
        emit signalVideoSinkChanged();
    }
}

QObject *VideoProviderService::videoOutput() const
{
    return m_videoOutput;
}

void VideoProviderService::setVideoOutput(QObject *videoOutput)
{
    if (videoOutput != m_videoOutput) {
        m_videoOutput = videoOutput;
        emit signalVideoOutputChanged();
    }
}

void VideoProviderService::syncStates()
{
    emit playBackStateChanged(m_currentPlaybackState);
    emit mediaStateChanged(m_currentMediaState);
}

void VideoProviderService::mediaPlay(const QUrl &url)
{
    m_currentMediaUrl = url;
    m_mediaPlayer->setVideoSink(m_videoSink);
    m_mediaPlayer->setVideoOutput(m_videoOutput);
    m_mediaPlayer->setSource(url);
    m_mediaPlayer->play();
}

void VideoProviderService::mediaPause()
{
    m_mediaPlayer->pause();
}

void VideoProviderService::mediaStop()
{
    m_mediaPlayer->stop();
}

void VideoProviderService::mediaContinue()
{
    m_mediaPlayer->play();
}

void VideoProviderService::mediaRestart()
{
    m_mediaPlayer->setPosition(0);
}

void VideoProviderService::mediaSeek(qint64 seekValue)
{
    m_mediaPlayer->setPosition(seekValue);
}

void VideoProviderService::updatePlaybackState(QMediaPlayer::PlaybackState state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        m_currentPlaybackState = PlayingState;
        break;
    case QMediaPlayer::PausedState:
        m_currentPlaybackState = PausedState;
        break;
    case QMediaPlayer::StoppedState:
        m_currentPlaybackState = StoppedState;
        break;
    default:
        break;
    }
    emit playBackStateChanged(m_currentPlaybackState);
}

void VideoProviderService::updateMediaState(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::NoMedia:
        m_currentMediaState = NoMedia;
        break;
    case QMediaPlayer::LoadingMedia:
        m_currentMediaState = LoadingMedia;
        break;
    case QMediaPlayer::LoadedMedia:
        m_currentMediaState = LoadedMedia;
        break;
    case QMediaPlayer::StalledMedia:
        m_currentMediaState = StalledMedia;
        break;
    case QMediaPlayer::BufferingMedia:
        m_currentMediaState = BufferingMedia;
        break;
    case QMediaPlayer::BufferedMedia:
        m_currentMediaState = BufferedMedia;
        break;
    case QMediaPlayer::EndOfMedia:
        m_currentMediaState = EndOfMedia;
        break;
    case QMediaPlayer::InvalidMedia:
        m_currentMediaState = InvalidMedia;
        break;
    default:
        break;
    }
    emit mediaStateChanged(m_currentMediaState);
}

void VideoProviderService::durationUpdated(qint64 duration)
{
    emit durationChanged(duration);
}

void VideoProviderService::positionUpdated(qint64 position)
{
    emit positionChanged(position);
}

void VideoProviderService::errorOccurred(QMediaPlayer::Error error)
{
    qDebug() << "VideoProviderService::errorOccurred" << error;
}