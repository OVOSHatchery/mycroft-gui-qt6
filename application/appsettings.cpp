/*
 * Copyright 2019 by Marco Martin <mart@kde.org>
 * Copyright 2018 by Aditya Mehra <aix.m@outlook.com>
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

#include <QDebug>
#include <QFile>
#include "appsettings.h"

AppSettings::AppSettings(QObject *parent) :
    QObject(parent)
{
}

bool AppSettings::darkMode() const
{
    return m_settings.value(QStringLiteral("darkMode"), true).toBool();
}

void AppSettings::setDarkMode(bool dark)
{
    if (AppSettings::darkMode() == dark) {
        return;
    }

    m_settings.setValue(QStringLiteral("darkMode"), dark);
    emit darkModeChanged();
}

bool AppSettings::usesRemoteSTT() const
{
    return m_settings.value(QStringLiteral("usesRemoteSTT"), false).toBool();
}

void AppSettings::setUsesRemoteSTT(bool remote)
{
    if (AppSettings::usesRemoteSTT() == remote) {
        return;
    }

    m_settings.setValue(QStringLiteral("usesRemoteSTT"), remote);
    emit usesRemoteSTTChanged();
}

// Shell mode settings

QString AppSettings::rotation() const
{
    return m_settings.value(QStringLiteral("rotation"), QStringLiteral("NORMAL")).toString();
}

void AppSettings::setRotation(const QString &rotation)
{
    if (AppSettings::rotation() == rotation) {
        return;
    }

    m_settings.setValue(QStringLiteral("rotation"), rotation);
    emit rotationChanged();
}

qreal AppSettings::fakeBrightness() const
{
    return m_settings.value(QStringLiteral("fakeBrightness"), 1.0).toDouble();
}

void AppSettings::setFakeBrightness(qreal brightness)
{
    if (AppSettings::fakeBrightness() == brightness) {
        return;
    }

    m_settings.setValue(QStringLiteral("fakeBrightness"), brightness);
    emit fakeBrightnessChanged();
}

bool AppSettings::menuLabels() const
{
    return m_settings.value(QStringLiteral("menuLabels"), false).toBool();
}

void AppSettings::setMenuLabels(bool menuLabels)
{
    if (AppSettings::menuLabels() == menuLabels) {
        return;
    }

    m_settings.setValue(QStringLiteral("menuLabels"), menuLabels);
    emit menuLabelsChanged();
}
