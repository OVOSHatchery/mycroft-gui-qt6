/*
 * Copyright 2019 by Marco Martin <mart@kde.org>
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

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS

Delegate {
    iconSource: "qrc://icons/system-reboot"
    text: qsTr("Reboot Services")

    onClicked: {
        OVOS.GuiBusClient.sendRequest("system.mycroft.service.restart", {})
    }
}

