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

#include "guibusclient.h"

#include <QQuickItem>
#include <QPointer>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

class NamespaceModel;
class GuiNamespace;
class GuiPage;
class NamespaceDataMap;
class QTranslator;

class GuiNamespace: public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(GuiBusClient::Status status READ status NOTIFY statusChanged)

    Q_PROPERTY(NamespaceModel *activeNamespaces READ activeNamespaces CONSTANT)

public:
    enum CustomFocusReasons {
        ServerEventFocusReason = Qt::OtherFocusReason
    };

    GuiNamespace(QQuickItem *parent = nullptr);
    ~GuiNamespace();

    GuiBusClient::Status status() const;

    QString id() const;
    void setId(const QString &id);

    NamespaceModel *activeNamespaces() const;

    Q_INVOKABLE NamespaceDataMap *namespaceDataForNamespace(const QString &namespace_id);

    Q_INVOKABLE void triggerGuiEvent(const QString &eventName, const QVariantMap &parameters);

Q_SIGNALS:
    void statusChanged();
    void closed();

public Q_SLOTS:
    void handleIncomingMessage(const QString &message);

private:
    void onGuiSocketMessageReceived(const QString &message);

    QString m_id;
    GuiBusClient *m_controller;
    NamespaceModel *m_activeNamespacesModel;

    QHash<QString, NamespaceDataMap *> m_namespaceDataMaps;
    QHash<QString, QTranslator *> m_translatorsForNamespace;

    QTimer m_trimComponentsTimer;
};
