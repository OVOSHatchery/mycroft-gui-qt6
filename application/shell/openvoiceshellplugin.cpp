/*
 * Copyright 2021 Aditya Mehra <aix.m@outlook.com>
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

#include "openvoiceshellplugin.h"
#include "placesmodel.h"
#include "configuration.h"
#include <QQmlContext>
#include <QQmlEngine>

static QObject *configSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new Configuration;
}

static QObject *placesModelSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new PlacesModel;
}


void OpenVoiceShellPlugin::registerTypes(const char *uri)
{
    qmlRegisterSingletonType<Configuration>(uri, 1, 0, "Configuration", configSingletonProvider);
    qmlRegisterSingletonType<PlacesModel>(uri, 1, 0, "PlacesModel", placesModelSingletonProvider);
}
