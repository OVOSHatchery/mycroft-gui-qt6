/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
 * Copyright 2018 David Edmundson <davidedmundson@kde.org>
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

#pragma once

#include <QObject>
#include <QWebSocket>
#include <QPointer>
#include <QQuickItem>
#include <QQmlEngine>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

#include "guibusmessages.h"

class GlobalSettings;
class QQmlPropertyMap;
class NamespaceModel;
class GuiNamespace;
class MediaService;

class GuiBusClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(Status status READ status NOTIFY socketStatusChanged)
    Q_PROPERTY(bool speaking READ isSpeaking NOTIFY isSpeakingChanged)
    Q_PROPERTY(bool listening READ isListening NOTIFY isListeningChanged)

    Q_PROPERTY(bool serverReady READ serverReady NOTIFY serverReadyChanged)

    Q_PROPERTY(bool useTls READ useTls NOTIFY useTlsChanged)
    Q_PROPERTY(QString authToken READ authToken NOTIFY authTokenChanged)

    Q_PROPERTY(QString sessionId READ sessionId NOTIFY sessionIdChanged)
    Q_PROPERTY(QString siteId READ siteId NOTIFY siteIdChanged)

public:
    enum Status {
        Connecting,
        Open,
        Closing,
        Closed,
        Error
    };
    Q_ENUM(Status)
    static GuiBusClient* instance();

    bool isSpeaking() const;
    bool isListening() const;
    bool serverReady() const;
    bool useTls() const;
    QString authToken() const;
    Status status() const;
    QString sessionId() const;
    QString siteId() const;
    NamespaceModel *activeNamespaces() const;

    //Public API NOT to be used with QML
    void registerNamespace(GuiNamespace *namespaceObj);
    void deregisterNamespace(GuiNamespace *namespaceObj);
    void setSessionId(const QString &sessionId);
    void setSiteId(const QString &siteId);

    void setMediaService(MediaService *service);
    MediaService *mediaService() const;

    void onMainSocketMessageReceived(const QString &message);

Q_SIGNALS:
    //socket stuff
    void socketStatusChanged();
    void closed();

    //OVOS state
    void isSpeakingChanged();
    void isListeningChanged();
    void stopped();
    void notUnderstood();
    void serverReadyChanged();
    void useTlsChanged();
    void authTokenChanged();

    void sessionIdChanged();
    void siteIdChanged();

    void utteranceManagedByNamespace(const QString &namespace_id);
    void namespaceTimeoutReceived(const QString &namespace_id);

public Q_SLOTS:
    void start();
    void disconnectSocket();
    void reconnect();
    void sendRequest(const QString &type, const QVariantMap &data, const QVariantMap &context = QVariantMap({}));
    void sendBinary(const QString &type, const QJsonObject &data, const QVariantMap &context = QVariantMap({}));
    void sendText(const QString &message);
    void sendConfirmResponse(bool confirmed);
    void sendSelectResponse(const QVariant &selection);

private:
    explicit GuiBusClient(QObject *parent = nullptr);
    QString buildWebSocketUrl() const;
    void handleAssistantEvent(GuiBusMessages::AssistantEvent evt, const QJsonDocument &doc);

    QWebSocket m_mainWebSocket;

    QTimer m_reconnectTimer;

    GlobalSettings *m_appSettingObj;

    QHash<QString, GuiNamespace *> m_namespaces;

    QHash<QString, QQmlPropertyMap*> m_namespaceData;

    bool m_isSpeaking = false;
    bool m_isListening = false;
    bool m_serverReady = false;
    bool m_useTls = false;
    QString m_authToken;

    // Session and site identifiers for multi-screen deployments
    QString m_sessionId = QStringLiteral("default");
    QString m_siteId = QStringLiteral("default");

    MediaService *m_mediaService = nullptr;
};
