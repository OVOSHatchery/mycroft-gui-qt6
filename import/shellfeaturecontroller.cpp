/*
 * Copyright 2026 OpenVoiceOS Contributors
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
 */

#include "shellfeaturecontroller.h"
#include "mycroftcontroller.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QUuid>

ShellFeatureController* ShellFeatureController::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return new ShellFeatureController();
}

ShellFeatureController* ShellFeatureController::instance()
{
    static ShellFeatureController controller;
    return &controller;
}

ShellFeatureController::ShellFeatureController(QObject *parent)
    : QObject(parent)
{
    // Initialize default theme
    m_currentTheme = QVariantMap({
        {"name", "Default"},
        {"primaryColor", "#0066CC"},
        {"secondaryColor", "#00CCFF"},
        {"textColor", "#FFFFFF"}
    });
}

// ==================== BRIGHTNESS CONTROL ====================

void ShellFeatureController::setBrightness(int value)
{
    if (value < 0) value = 0;
    if (value > 100) value = 100;

    if (m_brightness != value) {
        m_brightness = value;
        sendProtocolMessage("gui.brightness.set", {{"brightness", value}});
        Q_EMIT brightnessChanged(m_brightness);
    }
}

void ShellFeatureController::setAutoDimEnabled(bool enabled)
{
    if (m_autoDimEnabled != enabled) {
        m_autoDimEnabled = enabled;
        sendProtocolMessage("gui.brightness.auto_dim.set", {{"auto_dim", enabled}});
        Q_EMIT autoDimEnabledChanged(enabled);
    }
}

void ShellFeatureController::setAutoDimTimeout(int seconds)
{
    if (seconds < 10) seconds = 10;  // Minimum 10 seconds
    if (seconds > 3600) seconds = 3600;  // Maximum 1 hour

    if (m_autoDimTimeout != seconds) {
        m_autoDimTimeout = seconds;
        Q_EMIT autoDimTimeoutChanged(seconds);
    }
}

void ShellFeatureController::setNightModeEnabled(bool enabled)
{
    if (m_nightModeEnabled != enabled) {
        m_nightModeEnabled = enabled;
        sendProtocolMessage("gui.brightness.night_mode.set", {{"auto_nightmode", enabled}});
        Q_EMIT nightModeEnabledChanged(enabled);
    }
}

// ==================== COLOR SCHEME ====================

void ShellFeatureController::setCurrentTheme(const QVariantMap &theme)
{
    m_currentTheme = theme;
    Q_EMIT currentThemeChanged(theme);
}

void ShellFeatureController::requestCurrentTheme()
{
    sendProtocolMessage("gui.color_scheme.get", {});
}

void ShellFeatureController::setColorScheme(const QString &themeName,
                                            const QString &primaryColor,
                                            const QString &secondaryColor,
                                            const QString &textColor,
                                            const QString &accentColor)
{
    QVariantMap data{
        {"theme_name", themeName},
        {"primaryColor", primaryColor},
        {"secondaryColor", secondaryColor},
        {"textColor", textColor}
    };

    if (!accentColor.isEmpty()) {
        data["accentColor"] = accentColor;
    }

    sendProtocolMessage("gui.color_scheme.set", data);
    Q_EMIT colorSchemeUpdated(themeName);
}

// ==================== NOTIFICATIONS ====================

void ShellFeatureController::showNotification(const QString &title,
                                              const QString &body,
                                              const QString &icon,
                                              int timeoutMs)
{
    QString notificationId = QUuid::createUuid().toString();

    Q_EMIT notificationRequested(title, body, icon, notificationId);

    // Optional: Could also send to server if needed
    // sendProtocolMessage("gui.notification.display", {...});
}

void ShellFeatureController::dismissNotification(const QString &notificationId)
{
    sendProtocolMessage("gui.notification.clear", {{"notification_id", notificationId}});
}

// ==================== WIDGETS ====================

void ShellFeatureController::displayWidget(const QString &widgetId,
                                           const QString &widgetType,
                                           const QVariantMap &data)
{
    QVariantMap message{
        {"widget_id", widgetId},
        {"widget_type", widgetType},
        {"data", data}
    };

    Q_EMIT widgetDisplayRequested(widgetId, widgetType, data);

    // Optional: Could also send to server
    // sendProtocolMessage("gui.widget.display", message);
}

void ShellFeatureController::removeWidget(const QString &widgetId)
{
    Q_EMIT widgetRemoveRequested(widgetId);
    sendProtocolMessage("gui.widget.remove", {{"widget_id", widgetId}});
}

// ==================== CONFIGURATION UI ====================

void ShellFeatureController::requestConfigGroups()
{
    sendProtocolMessage("gui.config.list.get", {});
}

void ShellFeatureController::requestConfigForGroup(const QString &groupName)
{
    sendProtocolMessage("gui.config.get", {{"group_name", groupName}});
}

void ShellFeatureController::setConfigValue(const QString &groupName,
                                            const QVariantMap &values)
{
    QVariantMap data{
        {"group_name", groupName},
        {"values", values}
    };

    sendProtocolMessage("gui.config.set", data);
}

// ==================== PROTOCOL MESSAGE HANDLERS ====================

void ShellFeatureController::handleProtocolMessage(const QString &msgType, const QVariantMap &data)
{
    // Handle responses from adapter/core

    if (msgType == "gui.brightness.get.response") {
        int brightness = data.value("brightness", 100).toInt();
        m_brightness = brightness;
        Q_EMIT brightnessChanged(brightness);
    }
    else if (msgType == "gui.color_scheme.get.response") {
        setCurrentTheme(data);
    }
    else if (msgType == "gui.notification.set") {
        QString title = data.value("title", "Notification").toString();
        QString body = data.value("body", "").toString();
        QString icon = data.value("icon", "").toString();
        QString notificationId = data.value("notification_id", QUuid::createUuid().toString()).toString();

        Q_EMIT notificationRequested(title, body, icon, notificationId);
    }
    else if (msgType == "gui.widget.display") {
        QString widgetId = data.value("widget_id", "").toString();
        QString widgetType = data.value("widget_type", "").toString();
        QVariantMap widgetData = data.value("data", QVariantMap()).toMap();

        Q_EMIT widgetDisplayRequested(widgetId, widgetType, widgetData);
    }
    else if (msgType == "gui.config.list.get.response") {
        QVariantList groups = data.value("groups", QVariantList()).toList();
        Q_EMIT configGroupsReceived(groups);
    }
    else if (msgType == "gui.config.get.response") {
        QString groupName = data.value("group_name", "").toString();
        QVariantMap config = data.value("settings_metadata", QVariantMap()).toMap();

        Q_EMIT configForGroupReceived(groupName, config);
    }
    else {
        qDebug() << "Unhandled shell feature protocol message:" << msgType;
    }
}

// ==================== PRIVATE HELPER METHODS ====================

void ShellFeatureController::sendProtocolMessage(const QString &msgType, const QVariantMap &data)
{
    // Send message to MycroftController to forward to adapter via WebSocket
    MycroftController::instance()->sendRequest(msgType, data);
}
