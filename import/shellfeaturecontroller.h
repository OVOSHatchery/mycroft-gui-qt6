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

#pragma once

#include <QObject>
#include <QVariantMap>
#include <QQmlEngine>

/**
 * @class ShellFeatureController
 * @brief Manages shell features (brightness, colors, notifications, widgets, config UI)
 *
 * This class handles the new shell feature protocol extensions defined in
 * ovos-legacy-mycroft-gui-plugin/docs/PROTOCOL_EXTENSIONS.md
 *
 * Features managed:
 * - Brightness control (auto-dim, night mode)
 * - Color scheme management
 * - Notifications and alerts
 * - Custom widgets
 * - Configuration UI
 *
 * All features communicate via the unified WebSocket protocol.
 */
class ShellFeatureController : public QObject
{
    Q_OBJECT
    QML_SINGLETON

    // ==================== BRIGHTNESS PROPERTIES ====================
    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(bool autoDimEnabled READ autoDimEnabled WRITE setAutoDimEnabled NOTIFY autoDimEnabledChanged)
    Q_PROPERTY(int autoDimTimeout READ autoDimTimeout WRITE setAutoDimTimeout NOTIFY autoDimTimeoutChanged)
    Q_PROPERTY(bool nightModeEnabled READ nightModeEnabled WRITE setNightModeEnabled NOTIFY nightModeEnabledChanged)

    // ==================== COLOR PROPERTIES ====================
    Q_PROPERTY(QVariantMap currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)

public:
    static ShellFeatureController* create(QQmlEngine *engine, QJSEngine *scriptEngine);
    static ShellFeatureController* instance();

    explicit ShellFeatureController(QObject *parent = nullptr);

    // ==================== BRIGHTNESS CONTROL ====================

    int brightness() const { return m_brightness; }
    void setBrightness(int value);

    bool autoDimEnabled() const { return m_autoDimEnabled; }
    void setAutoDimEnabled(bool enabled);

    int autoDimTimeout() const { return m_autoDimTimeout; }
    void setAutoDimTimeout(int seconds);

    bool nightModeEnabled() const { return m_nightModeEnabled; }
    void setNightModeEnabled(bool enabled);

    // ==================== COLOR SCHEME ====================

    QVariantMap currentTheme() const { return m_currentTheme; }
    void setCurrentTheme(const QVariantMap &theme);

    Q_INVOKABLE void requestCurrentTheme();
    Q_INVOKABLE void setColorScheme(const QString &themeName,
                                    const QString &primaryColor,
                                    const QString &secondaryColor,
                                    const QString &textColor,
                                    const QString &accentColor = QString());

    // ==================== NOTIFICATIONS ====================

    Q_INVOKABLE void showNotification(const QString &title,
                                      const QString &body,
                                      const QString &icon = QString(),
                                      int timeoutMs = 5000);

    Q_INVOKABLE void dismissNotification(const QString &notificationId);

    // ==================== WIDGETS ====================

    Q_INVOKABLE void displayWidget(const QString &widgetId,
                                   const QString &widgetType,
                                   const QVariantMap &data);

    Q_INVOKABLE void removeWidget(const QString &widgetId);

    // ==================== CONFIGURATION UI ====================

    Q_INVOKABLE void requestConfigGroups();

    Q_INVOKABLE void requestConfigForGroup(const QString &groupName);

    Q_INVOKABLE void setConfigValue(const QString &groupName,
                                    const QVariantMap &values);

    // ==================== PROTOCOL MESSAGE HANDLERS ====================

    /// Handle incoming protocol extension messages from adapter
    void handleProtocolMessage(const QString &msgType, const QVariantMap &data);

Q_SIGNALS:
    // ==================== BRIGHTNESS SIGNALS ====================
    void brightnessChanged(int brightness);
    void autoDimEnabledChanged(bool enabled);
    void autoDimTimeoutChanged(int seconds);
    void nightModeEnabledChanged(bool enabled);

    // ==================== COLOR SIGNALS ====================
    void currentThemeChanged(const QVariantMap &theme);
    void colorSchemeUpdated(const QString &themeName);

    // ==================== NOTIFICATION SIGNALS ====================
    void notificationRequested(const QString &title,
                               const QString &body,
                               const QString &icon,
                               const QString &notificationId);

    // ==================== WIDGET SIGNALS ====================
    void widgetDisplayRequested(const QString &widgetId,
                                const QString &widgetType,
                                const QVariantMap &data);
    void widgetRemoveRequested(const QString &widgetId);

    // ==================== CONFIGURATION SIGNALS ====================
    void configGroupsReceived(const QVariantList &groups);
    void configForGroupReceived(const QString &groupName, const QVariantMap &config);

private:
    // ==================== BRIGHTNESS MEMBERS ====================
    int m_brightness = 100;
    bool m_autoDimEnabled = false;
    int m_autoDimTimeout = 60;
    bool m_nightModeEnabled = false;

    // ==================== COLOR MEMBERS ====================
    QVariantMap m_currentTheme;

    // ==================== PRIVATE HELPER METHODS ====================
    void sendProtocolMessage(const QString &msgType, const QVariantMap &data);
};
