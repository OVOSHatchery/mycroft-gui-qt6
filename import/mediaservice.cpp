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

#include "mediaservice.h"

MediaService::MediaService(QObject *parent)
    : QObject(parent),
    m_controller(GuiBusClient::instance())
{
    m_controller->setMediaService(this);
    m_currentPlaybackState = MediaService::StoppedState;
    m_currentMediaState = MediaService::NoMedia;
    m_selectedProviderService = MediaService::NoProvider;

    // NOTE: intentRecevied signal was removed (dead code from legacy).
    // MediaService intent handling is done via mycroft.events.triggered instead.
}

void MediaService::unloadAudioProvider(MediaService::UnloadStateReason reason)
{
    m_unloadingAudioService = true;
    m_unloadReason = reason;
    if (m_audioProviderService) {
        QObject::disconnect(m_audioProviderService, nullptr, this, nullptr);
        m_audioProviderService->mediaStop();
        m_audioProviderService->deleteLater();
        m_audioProviderService = nullptr;
    }
    
    if(m_unloadReason == MediaService::MediaFinished) {
        emitEndOfMedia();
    }
    if(m_unloadReason == MediaService::ServiceUnloaded){
        m_selectedProviderService = MediaService::NoProvider;
    }
    if(m_unloadReason == MediaService::MediaStopped){
        emit positionChanged(0);
        emit durationChanged(0);
    }
    m_audioServiceProviderInitialized = false;
    emit audioProviderUnloaded();
    m_unloadingAudioService = false;
}

void MediaService::unloadVideoProvider(MediaService::UnloadStateReason reason)
{
    if(!(m_currentPlaybackState == MediaService::StoppedState)){
         m_videoProviderService->mediaStop();
    }
    setVideoOutput(nullptr);
    setVideoSink(nullptr);
    m_unloadingVideoService = true;
    m_unloadReason = MediaService::ServiceUnloaded;
    emit videoProviderUnloaded();
    m_unloadingVideoService = false;
}

void MediaService::initializeAudioProvider()
{
    m_audioProviderService = new AudioProviderService(this);
    QObject::connect(m_audioProviderService, &AudioProviderService::mediaStateChanged, this, &MediaService::updateMediaStateAudioProvider);
    QObject::connect(m_audioProviderService, &AudioProviderService::playBackStateChanged, this, &MediaService::updatePlaybackStateAudioProvider);
    QObject::connect(m_audioProviderService, &AudioProviderService::spectrumChanged, this, &MediaService::updateSpectrum);
    QObject::connect(m_audioProviderService, &AudioProviderService::durationChanged, this, &MediaService::updateDuration);
    QObject::connect(m_audioProviderService, &AudioProviderService::positionChanged, this, &MediaService::updatePosition);
    m_audioServiceProviderInitialized = true;
    emit audioProviderInitialized();
}

void MediaService::initializeVideoProvider()
{
    m_videoProviderService = new VideoProviderService(this);
    m_videoServiceProviderInitialized = true;
    emit videoProviderInitialized();
}


void MediaService::syncStates()
{
    if(m_selectedProviderService == MediaService::AudioProvider) {
        m_audioProviderService->syncStates();
    }
    if(m_selectedProviderService == MediaService::VideoProvider) {
        m_videoProviderService->syncStates();
    }
}

MediaService::PlaybackState MediaService::servicePlayBackState() const
{
    return m_currentPlaybackState;
}

MediaService::MediaState MediaService::serviceMediaState() const
{
    return m_currentMediaState;
}

void MediaService::mediaLoadUrl(const QString &url, MediaService::ProviderServiceType serviceType)
{
    if(serviceType == MediaService::AudioProvider){
        QUrl mediaUrl(url);

        if (!mediaUrl.isValid()) {
            handleInvalidMedia();
            return;
        }

        if (mediaUrl.isLocalFile()) {
            continueMediaLoadUrl(url, serviceType);
            return;
        }

        QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
        QNetworkRequest request(mediaUrl);
        QNetworkReply *reply = networkManager->head(request);

        QObject::connect(reply, &QNetworkReply::finished, this, [this, url, serviceType, reply, networkManager]() {
            bool isMediaUrl = (reply->error() == QNetworkReply::NoError);
            reply->deleteLater();
            networkManager->deleteLater();

            if (!isMediaUrl) {
                handleInvalidMedia();
            } else {
                continueMediaLoadUrl(url, serviceType);
            }
        });
        return;
    }
    continueMediaLoadUrl(url, serviceType);
}

void MediaService::handleInvalidMedia()
{
    m_currentMediaState = MediaService::InvalidMedia;
    emit mediaStateChanged(m_currentMediaState);
    m_currentMediaState = MediaService::NoMedia;
    emit mediaStateChanged(m_currentMediaState);

    m_mediaStateSync.clear();
    m_mediaStateSync.insert(QStringLiteral("status"), m_currentMediaState);
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.current.media.status"), m_mediaStateSync);
}

void MediaService::continueMediaLoadUrl(const QString &url, MediaService::ProviderServiceType serviceType)
{
    m_loadedUrl = url;
    changeProvider(serviceType);
    QTimer::singleShot(1000, this, [this](){
        if(m_selectedProviderService == MediaService::AudioProvider) {
            if(!m_unloadingAudioService) {
                if(m_audioServiceProviderInitialized && (!(m_unloadReason == MediaService::MediaFinished) || !(m_unloadReason == MediaService::MediaStopped))) {
                    unloadAudioProvider(MediaService::MediaChanged);
                }
                initializeAudioProvider();
                QUrl audioUrl = QUrl::fromUserInput(m_loadedUrl);
                m_audioProviderService->mediaPlay(audioUrl);
            }
        }
        if(m_selectedProviderService == MediaService::VideoProvider) {
            if(!m_videoServiceProviderInitialized){
                initializeVideoProvider();
            }
            QObject::connect(m_videoProviderService, &VideoProviderService::mediaStateChanged, this, &MediaService::updateMediaStateVideoProvider);
            QObject::connect(m_videoProviderService, &VideoProviderService::playBackStateChanged, this, &MediaService::updatePlaybackStateVideoProvider);
            QObject::connect(m_videoProviderService, &VideoProviderService::durationChanged, this, &MediaService::updateDuration);
            QObject::connect(m_videoProviderService, &VideoProviderService::positionChanged, this, &MediaService::updatePosition);
            m_videoProviderService->setVideoSink(m_videoSink);
            m_videoProviderService->setVideoOutput(m_videoOutput);
            QUrl videoUrl = QUrl::fromUserInput(m_loadedUrl);
            m_videoProviderService->mediaPlay(videoUrl);
        }
    });
}


void MediaService::mediaStop()
{
    // Depending on selected service provider call the ServiceProvider.mediaStop() method
    if(!m_unloadingAudioService) {
        if(m_selectedProviderService == MediaService::AudioProvider) {
            if(m_currentPlaybackState != MediaService::StoppedState){
                mediaPause();
                unloadAudioProvider(MediaService::MediaStopped);
                m_currentPlaybackState = MediaService::StoppedState;
                emit playbackStateChanged(m_currentPlaybackState);
                m_playbackStateSync.clear();
                m_playbackStateSync.insert(QStringLiteral("state"), m_currentPlaybackState);
                m_controller->sendRequest(QStringLiteral("gui.player.media.service.sync.status"), m_playbackStateSync);
            }
        }
    }
    if(m_selectedProviderService == MediaService::VideoProvider) {
        m_videoProviderService->mediaStop();
    }
}

void MediaService::mediaPause()
{
    if(m_selectedProviderService == MediaService::AudioProvider) {
        if(!m_unloadingAudioService) {
            m_audioProviderService->mediaPause();
        }
    }
    if(m_selectedProviderService == MediaService::VideoProvider) {
        m_videoProviderService->mediaPause();
    }
}

void MediaService::mediaContinue()
{
    if(m_selectedProviderService == MediaService::AudioProvider) {
        if(!m_unloadingAudioService) {
            m_audioProviderService->mediaContinue();
        }
    }
    if(m_selectedProviderService == MediaService::VideoProvider) {
        m_videoProviderService->mediaContinue();
    }
}

void MediaService::mediaRestart()
{
    if(m_selectedProviderService == MediaService::AudioProvider) {
        if(!m_unloadingAudioService) {
          mediaLoadUrl(m_loadedUrl, MediaService::AudioProvider);
        }
    }
    if(m_selectedProviderService == MediaService::VideoProvider) {
        m_videoProviderService->mediaRestart();
    }
}

void MediaService::mediaNext()
{
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.get.next"), m_emptyData);
}

void MediaService::mediaPrevious()
{
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.get.previous"), m_emptyData);
}

void MediaService::mediaRepeat()
{
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.get.repeat"), m_emptyData);
}

void MediaService::mediaShuffle()
{
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.get.shuffle"), m_emptyData);
}

void MediaService::mediaSeek(qint64 seekValue)
{
    if(m_selectedProviderService == MediaService::AudioProvider) {
        if(!m_unloadingAudioService) {
            if (m_audioServiceProviderInitialized){
                mediaContinue();
                m_audioProviderService->mediaSeek(seekValue);
            }
        }
    }
    if(m_selectedProviderService == MediaService::VideoProvider) {
        m_videoProviderService->mediaSeek(seekValue);
    }
}

QVariant MediaService::requestServiceInfo(QString &serviceInfoType)
{
    if(serviceInfoType == QStringLiteral("loadedUrl")){
        return m_loadedUrl;
    }
    if(serviceInfoType == QStringLiteral("receivedUrl")){
        return m_receivedUrl;
    }
    if(serviceInfoType == QStringLiteral("artist")){
        return m_artist;
    }
    if(serviceInfoType == QStringLiteral("album")){
        return m_album;
    }
    if(serviceInfoType == QStringLiteral("title")){
        return m_title;
    }
    if(serviceInfoType == QStringLiteral("thumbnail")){
        return m_thumbnail;
    }
    if(serviceInfoType == QStringLiteral("repeat")){
        return m_repeat;
    }
    return false;
}

QVariantMap MediaService::requestServiceMetaData()
{
    return m_serviceMetaData;
}

QVariantMap MediaService::requestCommonPlayMetaData()
{
    QVariantMap cpsMap;
    if(!m_artist.isEmpty()){
        cpsMap.insert(QStringLiteral("artist"), m_artist);
    };
    if(!m_title.isEmpty()){
        cpsMap.insert(QStringLiteral("title"), m_title);
    };
    if(!m_album.isEmpty()){
        cpsMap.insert(QStringLiteral("album"), m_album);
    };
    if(!m_thumbnail.isEmpty()){
        cpsMap.insert(QStringLiteral("thumbnail"), m_thumbnail);
    };
    return cpsMap;
}

void MediaService::updatePlaybackStateAudioProvider(AudioProviderService::PlaybackState playbackState)
{
    switch(playbackState) {
        case AudioProviderService::PlayingState:
            m_currentPlaybackState = MediaService::PlayingState;
            break;
        case AudioProviderService::PausedState:
            m_currentPlaybackState = MediaService::PausedState;
            break;
        case AudioProviderService::StoppedState:
            m_currentPlaybackState = MediaService::StoppedState;
            break;
    }
    emit playbackStateChanged(m_currentPlaybackState);

    m_playbackStateSync.clear();
    m_playbackStateSync.insert(QStringLiteral("state"), m_currentPlaybackState);
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.sync.status"), m_playbackStateSync);
}

void MediaService::updateMediaStateAudioProvider(AudioProviderService::MediaState mediaState)
{
    switch(mediaState) {
        case AudioProviderService::NoMedia:
            m_currentMediaState = MediaService::NoMedia;
            break;
        case AudioProviderService::LoadingMedia:
            m_currentMediaState = MediaService::LoadingMedia;
            break;
        case AudioProviderService::LoadedMedia:
            m_currentMediaState = MediaService::LoadedMedia;
            break;
        case AudioProviderService::StalledMedia:
            m_currentMediaState = MediaService::StalledMedia;
            break;
        case AudioProviderService::BufferingMedia:
            m_currentMediaState = MediaService::BufferingMedia;
            break;
        case AudioProviderService::BufferedMedia:
            m_currentMediaState = MediaService::BufferedMedia;
            break;
        case AudioProviderService::EndOfMedia:
            audioServiceEndOfMedia();
            break;
        case AudioProviderService::InvalidMedia:
            m_currentMediaState = MediaService::InvalidMedia;
            break;
    }
    emit mediaStateChanged(m_currentMediaState);

    m_mediaStateSync.clear();
    m_mediaStateSync.insert(QStringLiteral("status"), m_currentMediaState);
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.current.media.status"), m_mediaStateSync);
}

void MediaService::updatePlaybackStateVideoProvider(VideoProviderService::PlaybackState playbackState)
{
    switch(playbackState) {
        case VideoProviderService::PlayingState:
            m_currentPlaybackState = MediaService::PlayingState;
            break;
        case VideoProviderService::PausedState:
            m_currentPlaybackState = MediaService::PausedState;
            break;
        case VideoProviderService::StoppedState:
            m_currentPlaybackState = MediaService::StoppedState;
            break;
    }
    emit playbackStateChanged(m_currentPlaybackState);

    m_playbackStateSync.clear();
    m_playbackStateSync.insert(QStringLiteral("state"), m_currentPlaybackState);
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.sync.status"), m_playbackStateSync);
}

void MediaService::updateMediaStateVideoProvider(VideoProviderService::MediaState mediaState)
{
    switch(mediaState) {
        case VideoProviderService::NoMedia:
            m_currentMediaState = MediaService::NoMedia;
            break;
        case VideoProviderService::LoadingMedia:
            m_currentMediaState = MediaService::LoadingMedia;
            break;
        case VideoProviderService::LoadedMedia:
            m_currentMediaState = MediaService::LoadedMedia;
            break;
        case VideoProviderService::StalledMedia:
            m_currentMediaState = MediaService::StalledMedia;
            break;
        case VideoProviderService::BufferingMedia:
            m_currentMediaState = MediaService::BufferingMedia;
            break;
        case VideoProviderService::BufferedMedia:
            m_currentMediaState = MediaService::BufferedMedia;
            break;
        case VideoProviderService::EndOfMedia:
            mediaStop();
            QTimer::singleShot(1000, this, SLOT(videoServiceEndOfMedia()));
            break;
        case VideoProviderService::InvalidMedia:
            m_currentMediaState = MediaService::InvalidMedia;
            break;
    }
    emit mediaStateChanged(m_currentMediaState);

    m_mediaStateSync.clear();
    m_mediaStateSync.insert(QStringLiteral("status"), m_currentMediaState);
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.current.media.status"), m_mediaStateSync);
}

QVector<double> MediaService::spectrum() const
{
    return m_spectrum;
}

void MediaService::updateSpectrum(QVector<double> spectrum)
{
    m_spectrum = spectrum;
    emit spectrumChanged(m_spectrum);
}

void MediaService::updatePosition(qint64 position)
{
    emit positionChanged(position);
}

void MediaService::updateDuration(qint64 duration)
{
    emit durationChanged(duration);
}

void MediaService::audioServiceEndOfMedia()
{
    if(!m_unloadingAudioService) {
        unloadAudioProvider(MediaService::MediaFinished);
    }
}

void MediaService::videoServiceEndOfMedia()
{
    m_currentMediaState = MediaService::EndOfMedia;
    emit mediaStateChanged(m_currentMediaState);
    
    m_mediaStateSync.clear();
    m_mediaStateSync.insert(QStringLiteral("status"), m_currentMediaState);
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.current.media.status"), m_mediaStateSync);
}

void MediaService::emitEndOfMedia()
{
    m_currentMediaState = MediaService::EndOfMedia;
    emit mediaStateChanged(m_currentMediaState);
    
    m_mediaStateSync.clear();
    m_mediaStateSync.insert(QStringLiteral("status"), m_currentMediaState);
    m_controller->sendRequest(QStringLiteral("gui.player.media.service.current.media.status"), m_mediaStateSync);
}

void MediaService::changeProvider(MediaService::ProviderServiceType serviceType)
{
    if (m_selectedProviderService == serviceType) {
        return;
    }
    if(m_selectedProviderService == MediaService::VideoProvider){
        unloadVideoProvider(MediaService::ServiceUnloaded);
    }

    if(m_selectedProviderService == MediaService::AudioProvider){
        unloadAudioProvider(MediaService::ServiceUnloaded);
    }

    m_selectedProviderService = serviceType;
    emit providerServiceTypeChanged(m_selectedProviderService);
}

void MediaService::onMainSocketIntentReceived(const QString &type, const QVariantMap &data)
{

    if(type == QStringLiteral("gui.player.play") || type == QStringLiteral("gui.player.media.service.play")) {
        if (data.contains(QStringLiteral("uri"))) {
            m_receivedUrl = data[QStringLiteral("uri")].toString();
        } else {
            m_receivedUrl = data[QStringLiteral("track")].toString();
        }

        m_repeat = data[QStringLiteral("repeat")].toBool();

        QString mediaType = data[QStringLiteral("type")].toString();
        if (mediaType == QStringLiteral("video")) {
            m_selectedProviderService = MediaService::VideoProvider;
        } else {
            m_selectedProviderService = MediaService::AudioProvider;
        }

        mediaLoadUrl(m_receivedUrl, m_selectedProviderService);
        emit mediaLoadUrlRequested();
    }

    if(type == QStringLiteral("gui.player.pause") || type == QStringLiteral("gui.player.media.service.pause")) {
        mediaPause();
        emit mediaPauseRequested();
    }

    if(type == QStringLiteral("gui.player.stop") || type == QStringLiteral("gui.player.media.service.stop")) {
        mediaStop();
        emit mediaStopRequested();
    }

    if(type == QStringLiteral("gui.player.resume") || type == QStringLiteral("gui.player.media.service.resume")) {
        mediaContinue();
        emit mediaContinueRequested();
    }

    if(type == QStringLiteral("gui.player.seek")) {
        qint64 delta = data[QStringLiteral("delta")].toLongLong();
        if (m_selectedProviderService == MediaService::AudioProvider && m_audioProviderService) {
            mediaSeek(m_audioProviderService->position() + delta);
        } else if (m_selectedProviderService == MediaService::VideoProvider && m_videoProviderService) {
            mediaSeek(m_videoProviderService->position() + delta);
        }
    }

    if(type == QStringLiteral("gui.player.media.service.set.meta") || (type == QStringLiteral("gui.player.play") && data.contains(QStringLiteral("meta")))) {
        QVariantMap meta;
        if (type == QStringLiteral("gui.player.play")) {
            meta = data[QStringLiteral("meta")].toMap();
        } else {
            meta = data;
        }
        
        QString metaVal;

        if(meta.contains(QStringLiteral("artist"))){
            metaVal = meta[QStringLiteral("artist")].toString();
            if(!metaVal.isEmpty()){
                m_artist = metaVal;
            }
        }

        if(meta.contains(QStringLiteral("album"))){
            metaVal = meta[QStringLiteral("album")].toString();
            if(!metaVal.isEmpty()){
                m_album = metaVal;
            }
        }

        if(meta.contains(QStringLiteral("title"))){
            metaVal = meta[QStringLiteral("title")].toString();
            if(!metaVal.isEmpty()){
                m_title = metaVal;
            }
        }

        if(meta.contains(QStringLiteral("track"))){
            metaVal = meta[QStringLiteral("track")].toString();
            if(!metaVal.isEmpty()){
                m_title = metaVal;
            }
        }

        if(meta.contains(QStringLiteral("image"))){
            metaVal = meta[QStringLiteral("image")].toString();
            if(!metaVal.isEmpty()){
                m_thumbnail = metaVal;
            }
        }
        emit metaDataReceived();
    }
}

QVideoSink* MediaService::videoSink() const
{
    return m_videoSink;
}

void MediaService::setVideoSink(QVideoSink * newVideoSink)
{
	if(m_videoSink == newVideoSink)
		return;

	m_videoSink = newVideoSink;

	emit videoSinkChanged();
}

QObject* MediaService::videoOutput() const
{
    return m_videoOutput;
}

void MediaService::setVideoOutput(QObject * newVideoOutput)
{
    if(m_videoOutput == newVideoOutput)
        return;

    m_videoOutput = newVideoOutput;

    emit videoOutputChanged();
}