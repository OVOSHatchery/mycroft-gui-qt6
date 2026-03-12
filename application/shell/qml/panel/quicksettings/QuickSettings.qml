/*
 * Copyright 2020 Aditya Mehra <Aix.m@outlook.com>
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

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root
    signal delegateClicked
    property bool verticalMode
    spacing: Kirigami.Units.largeSpacing * 2

    BrightnessSlider {
        Layout.fillWidth: true
    }
    VolumeSlider {
        Layout.fillWidth: true
    }

    GridLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        columns: verticalMode ? 4 : 7

        HomeDelegate {}
        WirelessDelegate {}
        RotationDelegate {}
        MuteDelegate {}
        AdditionalSettingsDelegate {}
        RebootDelegate {}
        ShutdownDelegate {}
    }
}
