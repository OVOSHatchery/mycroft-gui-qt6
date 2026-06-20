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

#include "namespacemodel.h"
#include "pagemodel.h"
#include "guipage.h"

#include <QDebug>


NamespaceModel::NamespaceModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

NamespaceModel::~NamespaceModel()
{
}

QStringList NamespaceModel::whiteList() const
{
    return m_whiteList;
}

void NamespaceModel::setWhiteList(const QStringList &whiteList)
{
    if (m_whiteList != whiteList) {
        m_whiteList = whiteList;
        emit whiteListChanged();
    }
}

void NamespaceModel::checkGuiActivation(const QString &namespace_id)
{
    // Implementation for GUI activation logic
}

void NamespaceModel::insertNamespaces(int position, const QStringList &namespaceList)
{
    if (position < 0 || position > m_namespaces.count()) {
        return;
    }

    QStringList filteredList;

    std::copy_if(namespaceList.begin(), namespaceList.end(),
                 std::back_inserter(filteredList),
                 [this](const QString &val)
                 {
                     if (!m_whiteList.isEmpty() && !m_whiteList.contains(val)) {
                         return false;
                     }
                     return !m_namespaces.contains(val);
                 });

    if (filteredList.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), position, position + filteredList.count() - 1);

    int i = 0;
    for (const auto &namespace_id : filteredList) {
        m_namespaces.insert(position + i, namespace_id);
        // Each active namespace owns a PageModel that backs its GUI pages.
        // Without this, pageModelForNamespace() (and therefore every
        // mycroft.gui.list.* operation) would have nothing to operate on.
        if (!m_pageModels.contains(namespace_id)) {
            m_pageModels[namespace_id] = new PageModel(this);
        }
        ++i;
    }
    endInsertRows();
}

QList<PageModel *> NamespaceModel::pageModels() const
{
    return m_pageModels.values();
}

QStringList NamespaceModel::activeNamespaces() const
{
    return m_namespaces;
}

PageModel *NamespaceModel::pageModelForNamespace(const QString &namespace_id) const
{
    if (namespace_id.isEmpty() || !m_namespaces.contains(namespace_id)) {
        return nullptr;
    }

    return m_pageModels.value(namespace_id);
}

bool NamespaceModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent.isValid() || destinationParent.isValid()) {
        return false;
    }

    if (count <= 0 || sourceRow == destinationChild || sourceRow < 0 || sourceRow >= m_namespaces.count() ||
        destinationChild < 0 || destinationChild >= m_namespaces.count() || count - destinationChild > m_namespaces.count() - sourceRow) {
        return false;
    }
    const int sourceLast = sourceRow + count - 1;

    if (!beginMoveRows(sourceParent, sourceRow, sourceLast, destinationParent, destinationChild)) {
        return false;
    }

    if (sourceRow < destinationChild) {
        for (int i = count - 1; i >= 0; --i) {
            m_namespaces.move(sourceRow + i, destinationChild - count + i);
        }
    } else {
        for (int i = 0; i < count; ++i) {
            m_namespaces.move(sourceRow + i, destinationChild + i);
        }
    }

    endMoveRows();
    return true;
}

bool NamespaceModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count <= 0 || row + count > m_namespaces.count() || parent.isValid()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    for (auto it = m_namespaces.begin() + row; it < m_namespaces.begin() + row + count; ++it) {
        PageModel *model = m_pageModels.value(*it);
        if (model) {
            model->deleteLater();
            m_pageModels.remove(*it);
        }
    }
    m_namespaces.erase(m_namespaces.begin() + row, m_namespaces.begin() + row + count);

    endRemoveRows();
    return true;
}


int NamespaceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_namespaces.count();
}

QVariant NamespaceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    const int row = index.row();

    if (row < 0 || row >= m_namespaces.count()) {
        return QVariant();
    }

    if (role == NamespaceId) {
        return m_namespaces[row];
    } else if (role == NamespaceData) {
        return QVariant::fromValue(m_namespaces[row]);
    } else if (role == Pages) {
        return QVariant::fromValue(m_pageModels.value(m_namespaces[row]));
    }
    
    return QVariant();
}

QHash<int, QByteArray> NamespaceModel::roleNames() const
{
    return {
        {NamespaceId, "namespaceId"},
        {NamespaceData, "namespaceData"},
        {Pages, "pages"}
    };
}

QModelIndex NamespaceModel::namespaceIndex(const QString &namespace_id) const
{
    const int row = m_namespaces.indexOf(namespace_id);

    if (row >= 0) {
        return index(row, 0, QModelIndex());
    }

    return QModelIndex();
}

#include "moc_namespacemodel.cpp"
