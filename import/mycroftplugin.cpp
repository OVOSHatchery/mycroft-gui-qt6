/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
 * Copyright 2018 David Edmundson <davidedmundson@kde.org>
 * Copyright 2018 Aditya Mehra <aix.m@outlook.com>
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

#include "mycroftplugin.h"

#include "mycroftcontroller.h"
#include "globalsettings.h"
#include "filereader.h"
#include "abstractdelegate.h"
#include "abstractskillview.h"
#include "activeskillsmodel.h"
#include "delegatesmodel.h"
#include "sessiondatamap.h"
#include "audiorec.h"
#include "mediaservice.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>

static QObject *fileReaderSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new FileReader;
}

static QObject *globalSettingsSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new GlobalSettings;
}

static QObject *mycroftControllerSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);

    //singleton managed internally, qml should never delete it
    engine->setObjectOwnership(MycroftController::instance(), QQmlEngine::CppOwnership);
    return MycroftController::instance();
}

static QObject *audioRecSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new AudioRec;
}

static QObject *mediaServiceSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new MediaService;
}

void MycroftPlugin::registerTypes(const char *uri)
{
    // DEPRECATED: QML module system is no longer used.
    // Singletons and types are now instantiated directly in main.cpp
    // and registered via context properties instead of QML module imports.
    // This method is kept for compatibility but does nothing.
    Q_UNUSED(uri);
}

#include "moc_mycroftplugin.cpp"

