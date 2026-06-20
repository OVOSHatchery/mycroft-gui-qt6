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

#pragma once

#include <QAbstractListModel>

class PageModel;

class NamespaceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList whiteList READ whiteList WRITE setWhiteList NOTIFY whiteListChanged)

public:
    enum Roles {
        NamespaceId = Qt::UserRole + 1,
        NamespaceData,
        Pages
    };

    NamespaceModel(QObject *parent = nullptr);
    ~NamespaceModel();

    void insertNamespaces(int position, const QStringList &namespaces);
    Q_INVOKABLE void insertSkills(int position, const QStringList &namespaces) { insertNamespaces(position, namespaces); }
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QModelIndex namespaceIndex(const QString &namespace_id) const;
    PageModel *pageModelForNamespace(const QString &namespace_id) const;
    Q_INVOKABLE PageModel *delegatesModelForSkill(const QString &namespace_id) { return pageModelForNamespace(namespace_id); }
    QList<PageModel *> pageModels() const;
    QStringList activeNamespaces() const;

    QStringList whiteList() const;
    void setWhiteList(const QStringList &whiteList);

    void checkGuiActivation(const QString &namespace_id);

Q_SIGNALS:
    void whiteListChanged();

private:
    QStringList m_namespaces;
    QStringList m_whiteList;
    QHash<QString, PageModel *> m_pageModels;
};
