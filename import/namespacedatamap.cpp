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

#include "namespacedatamap.h"

NamespaceDataMap::NamespaceDataMap(const QString &namespace_id, QObject *parent)
    : QQmlPropertyMap(this, parent),
      m_namespaceId(namespace_id)
{
}

QString NamespaceDataMap::namespaceId() const
{
    return m_namespaceId;
}

void NamespaceDataMap::insertAndNotify(const QString &key, const QVariant &value)
{
    insert(key, value);
    emit valueChanged(key, value);
}

void NamespaceDataMap::clearAndNotify(const QString &key)
{
    insert(key, QVariant());
    emit valueChanged(key, QVariant());
    emit dataCleared();
}

#include "moc_namespacedatamap.cpp"
