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

class PageLoader;

class PageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        PageUi = Qt::UserRole + 1,
        NamespaceId,
        PageUrl
    };

    PageModel(QObject *parent = nullptr);
    ~PageModel();

    void insertPageLoaders(int position, const QList<PageLoader *> &pageLoaders);
    void insertDelegateLoaders(int position, const QList<PageLoader *> &pageLoaders) { insertPageLoaders(position, pageLoaders); }
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QList<PageLoader *> pageLoaders() const;
    QList<class GuiPage *> pages() const;
    QList<class GuiPage *> delegates() const { return pages(); }

    int currentIndex() const;
    void setCurrentIndex(int index);

Q_SIGNALS:
    void currentIndexChanged();

private:
    QList<PageLoader *> m_pageLoaders;
    int m_currentIndex = -1;
};
