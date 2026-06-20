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

#include "namespacedatamodel.h"
#include "namespacedatamap.h"

#include <QDebug>

NamespaceDataModel::NamespaceDataModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

NamespaceDataModel::~NamespaceDataModel()
{
}

void NamespaceDataModel::insertData(int position, const QList<QVariantMap> &data)
{
    if (position < 0 || position > m_data.count()) {
        return;
    }

    beginInsertRows(QModelIndex(), position, position + data.count() - 1);

    int i = 0;
    for (const auto &item : data) {
        m_data.insert(position + i, item);
        ++i;
    }

    endInsertRows();
}

void NamespaceDataModel::updateData(int position, const QList<QVariantMap> &data)
{
    if (position < 0 || position + data.count() > m_data.count()) {
        return;
    }

    int i = 0;
    for (const auto &item : data) {
        m_data[position + i] = item;
        ++i;
    }

    emit dataChanged(index(position, 0), index(position + data.count() - 1, 0));
}

void NamespaceDataModel::clear()
{
    if (m_data.isEmpty()) {
        return;
    }

    beginResetModel();
    m_data.clear();
    endResetModel();
}

bool NamespaceDataModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent.isValid() || destinationParent.isValid()) {
        return false;
    }

    if (count <= 0 || sourceRow == destinationChild || sourceRow < 0 || sourceRow >= m_data.count() ||
        destinationChild < 0 || destinationChild >= m_data.count() || count - destinationChild > m_data.count() - sourceRow) {
        return false;
    }
    const int sourceLast = sourceRow + count - 1;

    if (!beginMoveRows(sourceParent, sourceRow, sourceLast, destinationParent, destinationChild)) {
        return false;
    }

    if (sourceRow < destinationChild) {
        for (int i = count - 1; i >= 0; --i) {
            m_data.move(sourceRow + i, destinationChild - count + i);
        }
    } else {
        for (int i = 0; i < count; ++i) {
            m_data.move(sourceRow + i, destinationChild + i);
        }
    }

    endMoveRows();
    return true;
}

bool NamespaceDataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count <= 0 || row + count > m_data.count() || parent.isValid()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    m_data.erase(m_data.begin() + row, m_data.begin() + row + count);
    endRemoveRows();
    return true;
}


int NamespaceDataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_data.count();
}

QVariant NamespaceDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    const int row = index.row();

    if (row < 0 || row >= m_data.count() || role != Data) {
        return QVariant();
    }

    return m_data[row];
}

QHash<int, QByteArray> NamespaceDataModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Data] = "data";
    
    if (!m_data.isEmpty()) {
        for (const auto &key : m_data.first().keys()) {
            roles[Data + 1 + roles.count()] = key.toUtf8();
        }
    }
    return roles;
}

#include "moc_namespacedatamodel.cpp"
