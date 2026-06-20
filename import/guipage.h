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

#include <QQuickItem>
#include <QQmlParserStatus>
#include <QQmlPropertyMap>
#include <QPointer>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

#include "namespacedatamap.h"
#include "guinamespace.h"

class GuiBusClient;

/**
 * PageLoader: Internal utility to load QML pages from the server
 */
class PageLoader : public QObject {
    Q_OBJECT

public:
    explicit PageLoader(GuiNamespace *parent);
    ~PageLoader();

    void init(const QString &namespace_id, const QUrl &url);

    QString namespaceId() const;
    QUrl url() const;
    QUrl translationsUrl() const;

    QQuickItem *pageItem() const;
    void setFocus(bool focus);

Q_SIGNALS:
    void pageItemChanged();

private:
    QString m_namespaceId;
    QUrl m_url;
    QPointer<QQuickItem> m_pageItem;
    GuiNamespace *m_namespaceObj;
};


/**
 * GuiPage: Base class for all OVOS GUI pages
 */
class GuiPage : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QQmlPropertyMap *namespaceData READ namespaceData NOTIFY namespaceDataChanged)
    Q_PROPERTY(QString namespaceId READ namespaceId NOTIFY namespaceIdChanged)

public:
    GuiPage(QQuickItem *parent = nullptr);
    ~GuiPage();

    QQmlPropertyMap *namespaceData() const;
    QString namespaceId() const;

    Q_INVOKABLE QString skillId() const { return namespaceId(); }
    Q_INVOKABLE QUrl qmlUrl() const;

    Q_INVOKABLE void triggerGuiEvent(const QString &eventName, const QVariantMap &parameters);

Q_SIGNALS:
    void namespaceDataChanged();
    void namespaceIdChanged();
    void guiEvent(const QString &eventName, const QVariantMap &data);

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    NamespaceDataMap *m_namespaceDataMap = nullptr;
    GuiBusClient *m_controller;
};
