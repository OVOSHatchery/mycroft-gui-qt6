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

#include "audioproviderservice.h"

AudioProviderService::AudioProviderService(QObject *parent)
    : QObject(parent),
      m_audioStreamDevice(nullptr),
      m_audioSink(nullptr)
{
    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    m_format = info.preferredFormat();
    m_currentMediaState = AudioProviderService::NoMedia;
    m_currentPlaybackState = AudioProviderService::StoppedState;
}

AudioProviderService::~AudioProviderService()
{
    if (m_audioSink) {
        m_audioSink->deleteLater();
    }
    if (m_audioStreamDevice) {
        m_audioStreamDevice->deleteLater();
    }
    emit destroyedService();
}

void AudioProviderService::syncStates()
{
    if (!m_audioSink) return;
    switch (m_audioSink->state()) {
    case QAudio::ActiveState:
        m_currentPlaybackState = AudioProviderService::PlayingState;
        break;
    case QAudio::SuspendedState:
        m_currentPlaybackState = AudioProviderService::PausedState;
        break;
    case QAudio::StoppedState:
        m_currentPlaybackState = AudioProviderService::StoppedState;
        break;
    default:
        break;
    }
    emit playBackStateChanged(m_currentPlaybackState);
    emit mediaStateChanged(m_currentMediaState);
}

void AudioProviderService::mediaPlay(const QUrl &url)
{
    if (m_audioSink) {
        m_audioSink->stop();
        m_audioSink->deleteLater();
        m_audioSink = nullptr;
    }
    if (m_audioStreamDevice) {
        m_audioStreamDevice->stop();
        m_audioStreamDevice->deleteLater();
        m_audioStreamDevice = nullptr;
    }

    m_audioStreamDevice = new AudioStreamDevice(this);
    emit mediaStateChanged(m_currentMediaState);
    emit playBackStateChanged(m_currentPlaybackState);
    m_currentMediaState = AudioProviderService::LoadingMedia;
    emit mediaStateChanged(m_currentMediaState);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::bufferingMedia, this, &AudioProviderService::notifyBufferingMedia);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::bufferedMedia, this, &AudioProviderService::notifyBufferedMedia);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::stalledMedia, this, &AudioProviderService::notifyStalledMedia);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::endOfMedia, this, &AudioProviderService::notifyEndOfMedia);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::invalidMedia, this, &AudioProviderService::notifyInvalidMedia);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::spectrumChanged, this, &AudioProviderService::spectrumUpdated);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::positionChanged, this, &AudioProviderService::positionUpdated);
    QObject::connect(m_audioStreamDevice, &AudioStreamDevice::durationChanged, this, &AudioProviderService::durationUpdated);

    if (!m_audioStreamDevice->init(m_format)) {
        qWarning() << "AudioStreamDevice Could Not Be Initialized";
    }
    m_audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), m_format, this);
    m_currentMediaUrl = url;
    m_audioSink->start(m_audioStreamDevice);
    m_audioStreamDevice->play(url);
    if (m_audioSink->state() == QAudio::ActiveState) {
        m_currentPlaybackState = AudioProviderService::PlayingState;
        emit playBackStateChanged(m_currentPlaybackState);
        m_currentMediaState = AudioProviderService::LoadedMedia;
        emit mediaStateChanged(m_currentMediaState);
    }
}

void AudioProviderService::mediaStop()
{
    if (m_audioStreamDevice) {
        m_audioStreamDevice->stop();
        m_audioStreamDevice->deleteLater();
        m_audioStreamDevice = nullptr;
    }
    if (m_audioSink) {
        m_audioSink->stop();
        m_audioSink->deleteLater();
        m_audioSink = nullptr;
    }
    m_currentPlaybackState = AudioProviderService::StoppedState;
    emit playBackStateChanged(m_currentPlaybackState);
    m_currentMediaState = AudioProviderService::NoMedia;
    emit mediaStateChanged(m_currentMediaState);
}

void AudioProviderService::mediaPause()
{
    if (m_audioSink) {
        m_audioSink->suspend();
        m_currentPlaybackState = AudioProviderService::PausedState;
        emit playBackStateChanged(m_currentPlaybackState);
    }
}

void AudioProviderService::mediaContinue()
{
    if (m_audioSink) {
        m_audioSink->resume();
        m_currentPlaybackState = AudioProviderService::PlayingState;
        emit playBackStateChanged(m_currentPlaybackState);
    }
}

void AudioProviderService::mediaRestart()
{
    mediaPlay(m_currentMediaUrl);
}

void AudioProviderService::mediaSeek(qint64 seekValue)
{
    if (m_audioSink && m_audioStreamDevice) {
        seekValue = seekValue - (seekValue % 1000);
        m_audioSink->suspend();
        m_audioStreamDevice->seek(seekValue);
        m_audioStreamDevice->seekFrom(seekValue);
        m_audioSink->resume();
    }
}

void AudioProviderService::notifyBufferingMedia()
{
    m_currentMediaState = AudioProviderService::BufferingMedia;
    emit mediaStateChanged(m_currentMediaState);
}

void AudioProviderService::notifyBufferedMedia()
{
    if(m_currentMediaState != AudioProviderService::NoMedia){
        m_currentMediaState = AudioProviderService::BufferedMedia;
        emit mediaStateChanged(m_currentMediaState);
    }
}

void AudioProviderService::notifyStalledMedia()
{
    m_currentMediaState = AudioProviderService::StalledMedia;
    emit mediaStateChanged(m_currentMediaState);
}

void AudioProviderService::notifyEndOfMedia()
{
    m_currentMediaState = AudioProviderService::EndOfMedia;
    emit mediaStateChanged(m_currentMediaState);
}

void AudioProviderService::notifyInvalidMedia()
{
    if (m_audioSink) {
        m_audioSink->suspend();
    }
    QTimer::singleShot(2000, this, [this](){
        if (m_audioStreamDevice) {
            m_audioStreamDevice->stop();
        }
        if (m_audioSink) {
            m_audioSink->stop();
        }
        m_currentPlaybackState = AudioProviderService::StoppedState;
        emit playBackStateChanged(m_currentPlaybackState);
    });
    m_currentMediaState = AudioProviderService::InvalidMedia;
    emit mediaStateChanged(m_currentMediaState);
}

void AudioProviderService::spectrumUpdated(QVector<double> spectrum)
{
    emit spectrumChanged(spectrum);
}

void AudioProviderService::durationUpdated(qint64 duration)
{
    emit durationChanged(duration);
}

void AudioProviderService::positionUpdated(qint64 position)
{
    m_position = position;
    emit positionChanged(position);
}