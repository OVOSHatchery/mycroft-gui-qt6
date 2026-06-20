/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
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
#include "pagemodel.h"
#include "guipage.h"

#include <QTimer>
#include <QDebug>


PageModel::PageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

PageModel::~PageModel()
{
}

void PageModel::insertPageLoaders(int position, const QList<PageLoader *> &pageLoaders)
{
    if (position < 0 || position > m_pageLoaders.count()) {
        return;
    }

    beginInsertRows(QModelIndex(), position, position + pageLoaders.count() - 1);

    int i = 0;
    for (auto *loader : pageLoaders) {
        m_pageLoaders.insert(position + i, loader);
        connect(loader, &PageLoader::pageItemChanged, this, [this, loader]() {
            int row = m_pageLoaders.indexOf(loader);
            emit dataChanged(index(row, 0), index(row, 0), {PageUi});
        });
        connect(loader, &QObject::destroyed, this, [this](QObject *obj) {
            const int index = m_pageLoaders.indexOf(qobject_cast<PageLoader *>(obj));
            if (index > -1) {
                removeRows(index, 1, QModelIndex());
            }
        });
        ++i;
    }

    endInsertRows();
}

QList<GuiPage *> PageModel::pages() const
{
    QList<GuiPage *> pages;

    for (auto loader : m_pageLoaders) {
        GuiPage *page = qobject_cast<GuiPage *>(loader->pageItem());
        if (page) {
            pages << page;
        }
    }

    return pages;
}

bool PageModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent.isValid() || destinationParent.isValid()) {
        return false;
    }

    if (count <= 0 || sourceRow == destinationChild || sourceRow < 0 || sourceRow >= m_pageLoaders.count() ||
        destinationChild < 0 || destinationChild >= m_pageLoaders.count() || count - destinationChild > m_pageLoaders.count() - sourceRow) {
        return false;
    }
    const int sourceLast = sourceRow + count - 1;

    if (!beginMoveRows(sourceParent, sourceRow, sourceLast, destinationParent, destinationChild)) {
        return false;
    }

    if (sourceRow < destinationChild) {
        for (int i = count - 1; i >= 0; --i) {
            m_pageLoaders.move(sourceRow + i, qMin(destinationChild + i, m_pageLoaders.count() - 1));
        }
    } else {
        for (int i = 0; i < count; ++i) {
            m_pageLoaders.move(sourceRow + i, destinationChild + i);
        }
    }

    endMoveRows();
    return true;
}

bool PageModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count <= 0 || row + count > m_pageLoaders.count() || parent.isValid()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_pageLoaders[row + i]->deleteLater();
    }
    m_pageLoaders.erase(m_pageLoaders.begin() + row, m_pageLoaders.begin() + row + count);

    endRemoveRows();
    return true;
}


int PageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_pageLoaders.count();
}

QVariant PageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.parent().isValid()) {
        return QVariant();
    }
    const int row = index.row();

    if (row < 0 || row >= m_pageLoaders.count()) {
        return QVariant();
    }

    if (role == PageUi) {
        return QVariant::fromValue(m_pageLoaders[row]->pageItem());
    } else if (role == NamespaceId) {
        return m_pageLoaders[row]->namespaceId();
    } else if (role == PageUrl) {
        return m_pageLoaders[row]->url();
    }

    return QVariant();
}

QHash<int, QByteArray> PageModel::roleNames() const
{
    return {
        {PageUi, "pageUi"},
        {NamespaceId, "namespaceId"},
        {PageUrl, "pageUrl"}
    };
}

int PageModel::currentIndex() const
{
    return m_currentIndex;
}

void PageModel::setCurrentIndex(int index)
{
    if (m_currentIndex == index || index < 0 || index >= m_pageLoaders.count()) {
        return;
    }
    m_currentIndex = index;
    emit currentIndexChanged();
}

QList<PageLoader *> PageModel::pageLoaders() const
{
    return m_pageLoaders;
}

#include "moc_pagemodel.cpp"
