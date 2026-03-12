/*
 * Copyright 2022 Aditya Mehra <aix.m@outlook.com>
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

#ifndef PLACESMODEL_H
#define PLACESMODEL_H

#include <QAbstractListModel>
#include <QDir>
#include <QStandardPaths>
#include <QUrl>
#include <QObject>
#include <QFileSystemWatcher>

class PlacesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        UrlRole,
        IconRole,
        IsRemovableRole,
        IsMountedRole,
        IsSystemRole,
        LiteralPathRole
    };

    PlacesModel(QObject *parent = nullptr);
    ~PlacesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int count() const;

    Q_INVOKABLE void mount(int index);
    Q_INVOKABLE void unmount(int index);
    Q_INVOKABLE bool hasHintSystem(const QString literalPath);

    int getHomePathIndex();
    void update();
    void clear();
    QString getLiteralPath(const QString path);

Q_SIGNALS:
    void placeMounted(int index, QString path, QString name, QString icon, bool removable, bool mounted, bool system, QString literalPath);
    void placeUnmounted(int index, QString path, QString name, QString icon, bool removable, bool mounted, bool system, QString literalPath);
    void countChanged();

private:
    QFileSystemWatcher *m_watcher;
    QList<QUrl> m_places;
    QList<QString> m_names;
    QList<QString> m_icons;
    QList<bool> m_removable;
    QList<bool> m_mounted;
    QList<QUrl> m_mountPoints;
    QList<QString> m_literalPaths;
    QList<bool> m_system;
};

#endif // PLACESMODEL_H