/*
 *   Copyright 2018 by Marco Martin <mart@kde.org>
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

#include "guipage.h"
#include "guibusclient.h"

#include <QQmlEngine>
#include <QQmlContext>


PageLoader::PageLoader(GuiNamespace *parent)
    : QObject(parent),
      m_namespaceObj(parent)
{}

PageLoader::~PageLoader()
{
    if (m_pageItem) {
        m_pageItem->deleteLater();
    }
}

void PageLoader::init(const QString &namespace_id, const QUrl &url)
{
    if (!m_namespaceId.isEmpty()) {
        qWarning() << "Init already called";
    }

    m_namespaceId = namespace_id;
    m_url = url;
    QQmlEngine *engine = qmlEngine(m_namespaceObj);
    // Normally this object is created from QML and therefore has an engine.
    // In headless / non-QML contexts (e.g. tests, or a backend with no QML
    // surface) there is no engine: the namespace/page model state is still
    // tracked, but there is nothing to render. Bail out gracefully instead of
    // constructing a QQmlComponent with a null engine, which crashes.
    if (!engine) {
        qWarning() << "GuiPage::init: no QML engine for" << url
                   << "- skipping page item creation (headless/non-QML context)";
        return;
    }

    QQmlComponent *component = new QQmlComponent(engine, url, m_namespaceObj);

    auto createObject = [this, component]() {
        QQmlContext *context = QQmlEngine::contextForObject(m_namespaceObj);
        if (!context) {
            qWarning() << "GuiPage::init: no QML context for" << m_url
                       << "- skipping page item creation";
            return;
        }

        QObject *guiObject = component->beginCreate(context);
        m_pageItem = qobject_cast<QQuickItem *>(guiObject);
        
        if (component->isError()) {
            qWarning() << "ERROR Loading QML file" << m_url;
            for (auto err : component->errors()) {
                qWarning() << err.toString();
            }
            return;
        }

        if (!m_pageItem) {
            qWarning()<<"ERROR: QML gui" << guiObject << "not an item";
            guiObject->deleteLater();
            return;
        }

        connect(m_pageItem, &QObject::destroyed, this, &QObject::deleteLater);

        // If the item is a GuiPage, inject its data
        GuiPage *page = qobject_cast<GuiPage *>(m_pageItem);
        if (page) {
            // Internal setters for GuiPage would go here if we kept the same pattern,
            // but for a clean refactor we use the property system or public setters.
        }

        component->completeCreate();
        emit pageItemChanged();
    };

    switch(component->status()) {
    case QQmlComponent::Error:
        qWarning() << "ERROR Loading QML file" << url;
        for (auto err : component->errors()) {
            qWarning() << err.toString();
        }
        break;
    case QQmlComponent::Ready:
        createObject();
        break;
    case QQmlComponent::Loading:
        connect(component, &QQmlComponent::statusChanged, this, createObject);
        break;
    default:
        break;
    }
}

QString PageLoader::namespaceId() const
{
    return m_namespaceId;
}

QUrl PageLoader::url() const
{
    return m_url;
}

QUrl PageLoader::translationsUrl() const
{
    QUrl url(m_url);
    url.setPath(m_url.path().mid(0, m_url.path().indexOf(QStringLiteral("/ui/")) + 4) + QStringLiteral("translations"));

    return url;
}

QQuickItem *PageLoader::pageItem() const
{
    return m_pageItem;
}

void PageLoader::setFocus(bool focus)
{
    if (m_pageItem && focus) {
        m_pageItem->forceActiveFocus((Qt::FocusReason)GuiNamespace::ServerEventFocusReason);
    } else if (m_pageItem) {
        m_pageItem->setFocus(false);
    }
}

//////////////////////////////////////////

GuiPage::GuiPage(QQuickItem *parent)
    : QQuickItem(parent),
      m_controller(GuiBusClient::instance())
{
    setFiltersChildMouseEvents(true);
    setFlags(QQuickItem::ItemIsFocusScope);
    setAcceptedMouseButtons(Qt::LeftButton);
}

GuiPage::~GuiPage()
{
}

QQmlPropertyMap *GuiPage::namespaceData() const
{
    return m_namespaceDataMap;
}

QString GuiPage::namespaceId() const
{
    // Implementation would track which namespace this page belongs to
    return QString(); 
}

QUrl GuiPage::qmlUrl() const
{
    // Implementation would track the URL this page was loaded from
    return QUrl();
}

void GuiPage::triggerGuiEvent(const QString &eventName, const QVariantMap &parameters)
{
    // In monolithic app, we route events via the singleton bus client
    m_controller->sendRequest(QStringLiteral("mycroft.events.triggered"), parameters);
}

void GuiPage::classBegin()
{
    QQuickItem::classBegin();
}

void GuiPage::componentComplete()
{
    QQuickItem::componentComplete();
}

#include "moc_guipage.cpp"
