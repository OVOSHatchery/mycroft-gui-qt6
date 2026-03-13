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

#include "guibusmessages.h"

class GlobalSettings;
class QQmlPropertyMap;
class ActiveSkillsModel;
class AbstractSkillView;

class OVOSController : public QObject
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
    static OVOSController* instance();

    bool isSpeaking() const;
    bool isListening() const;
    bool serverReady() const;
    bool useTls() const;
    QString authToken() const;
    Status status() const;
    QString sessionId() const;
    QString siteId() const;

    //Public API NOT to be used with QML
    void registerView(AbstractSkillView *view);
    void deregisterView(AbstractSkillView *view);
    void setSessionId(const QString &sessionId);
    void setSiteId(const QString &siteId);

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

    void utteranceManagedBySkill(const QString &skill);
    void skillTimeoutReceived(const QString &skillidleid);

public Q_SLOTS:
    void start();
    void disconnectSocket();
    void reconnect();
    void sendRequest(const QString &type, const QVariantMap &data, const QVariantMap &context = QVariantMap({}));
    void sendBinary(const QString &type, const QJsonObject &data, const QVariantMap &context = QVariantMap({}));
    void sendText(const QString &message);

private:
    explicit OVOSController(QObject *parent = nullptr);
    void onMainSocketMessageReceived(const QString &message);
    void handleAssistantEvent(GuiBusMessages::AssistantEvent evt, const QJsonDocument &doc);

    QWebSocket m_mainWebSocket;

    QTimer m_reconnectTimer;

    GlobalSettings *m_appSettingObj;

    QHash<QString, AbstractSkillView *> m_views;

    QHash<QString, QQmlPropertyMap*> m_skillData;

    bool m_isSpeaking = false;
    bool m_isListening = false;
    bool m_serverReady = false;
    bool m_useTls = false;
    QString m_authToken;

    // Session and site identifiers for multi-screen deployments
    QString m_sessionId = QStringLiteral("default");
    QString m_siteId = QStringLiteral("default");
};
