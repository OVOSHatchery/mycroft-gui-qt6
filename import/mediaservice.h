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

#ifndef MEDIASERVICE_H
#define MEDIASERVICE_H

#include <QObject>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

#include <QVideoSink>
#include <QQmlEngine>

#include "ovoscontroller.h"
#include "mediaproviders/audioproviderservice.h"
#include "mediaproviders/videoproviderservice.h"

class MediaService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MediaService::MediaState mediaState READ serviceMediaState NOTIFY mediaStateChanged)
    Q_PROPERTY(MediaService::PlaybackState playbackState READ servicePlayBackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QVector<double> spectrum READ spectrum NOTIFY spectrumChanged)

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

    enum ProviderServiceType {
        AudioProvider = 102,
        VideoProvider = 101,
        NoProvider = 100
    };

    enum UnloadStateReason {
        MediaFinished = 0,
        MediaChanged = 1,
        MediaStopped = 2,
        ServiceUnloaded = 3
    };

    Q_ENUM(PlaybackState)
    Q_ENUM(MediaState)
    Q_ENUM(ProviderServiceType)
    Q_ENUM(UnloadStateReason)

    explicit MediaService(QObject *parent = nullptr);
    void initializeAudioProvider();
    void initializeVideoProvider();
    void unloadAudioProvider(MediaService::UnloadStateReason reason);
    void unloadVideoProvider(MediaService::UnloadStateReason reason);
    void changeProvider(MediaService::ProviderServiceType serviceType);
    void syncStates();
    bool evaluateUrl(const QString &url);
    Q_INVOKABLE MediaService::PlaybackState servicePlayBackState() const;
    Q_INVOKABLE MediaService::MediaState serviceMediaState() const;
    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink *newVideoSink);
    QObject* videoOutput() const;
    void setVideoOutput(QObject *newVideoOutput);

    // AudioProvider Specific Expose To QML
    QVector<double> spectrum() const;

public Q_SLOTS:
    void mediaLoadUrl(const QString &url, MediaService::ProviderServiceType serviceType);
    void mediaStop();
    void mediaPause();
    void mediaContinue();
    void mediaRestart();
    void mediaNext();
    void mediaPrevious();
    void mediaShuffle();
    void mediaRepeat();
    void mediaSeek(qint64 seekValue);
    QVariant requestServiceInfo(QString &serviceInfoType);
    QVariantMap requestServiceMetaData();
    QVariantMap requestCommonPlayMetaData();
    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);

    // AudioProvider Specific Expose To QML
    void updateSpectrum(QVector<double> spectrum);
    void audioServiceEndOfMedia();
    void emitEndOfMedia();

    // VideoProvider Specific Expose To QML
    void videoServiceEndOfMedia();

signals:
    void videoSinkChanged();
    void videoOutputChanged();

Q_SIGNALS:
    void playbackStateChanged(MediaService::PlaybackState playbackState);
    void mediaStateChanged(MediaService::MediaState mediaState);
    void providerServiceTypeChanged(MediaService::ProviderServiceType serviceType);
    void mediaLoadUrlRequested();
    void mediaPauseRequested();
    void mediaStopRequested();
    void mediaContinueRequested();
    void mediaRestartRequested();
    void mediaNextRequested();
    void mediaPreviousRequested();
    void mediaShuffleRequested();
    void mediaRepeatRequested();
    void metaDataReceived();
    void metaDataUpdated();
    void audioProviderInitialized();
    void videoProviderInitialized();
    void audioProviderUnloaded();
    void videoProviderUnloaded();
    void durationChanged(qint64 dur);
    void positionChanged(qint64 pos);

    // AudioProvider Specific Expose To QML
    void spectrumChanged(QVector<double> spectrum);

private slots:
    void updatePlaybackStateAudioProvider(AudioProviderService::PlaybackState playbackState);
    void updateMediaStateAudioProvider(AudioProviderService::MediaState mediaState);
    void updatePlaybackStateVideoProvider(VideoProviderService::PlaybackState playbackState);
    void updateMediaStateVideoProvider(VideoProviderService::MediaState mediaState);

private:
    ProviderServiceType m_selectedProviderService;
    PlaybackState m_currentPlaybackState;
    MediaState m_currentMediaState;
    AudioProviderService *m_audioProviderService;
    VideoProviderService *m_videoProviderService;
    UnloadStateReason m_unloadReason;
    QPointer<QVideoSink> m_videoSink;
    QPointer<QObject> m_videoOutput;

    OVOSController *m_controller;
    void onMainSocketIntentReceived(const QString &type, const QVariantMap &data);

    QString m_loadedUrl;
    QString m_receivedUrl;
    QString m_artist;
    QString m_album;
    QString m_title;
    QString m_thumbnail;
    bool m_repeat;
    QVariantMap m_emptyData;
    QVariantMap m_serviceMetaData;
    QVariantMap m_playbackStateSync;
    QVariantMap m_mediaStateSync;

    // AudioProvider Specific Expose To QML
    QVector<double> m_spectrum;

    bool m_audioServiceProviderInitialized;
    bool m_videoServiceProviderInitialized;
    bool m_unloadingAudioService;
    bool m_unloadingVideoService;
};

#endif // MEDIASERVICE_H
