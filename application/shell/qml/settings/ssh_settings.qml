/*
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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS
import Qt5Compat.GraphicalEffects

Kirigami.ScrollablePage {
    id: sshSettingsView
    title: qsTr("SSH Settings")

    property bool connectionActive: false

    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            id: warnText
            level: 3
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            color: Kirigami.Theme.textColor
            text: qsTr("By enabling SSH Mode, anyone can access, change or delete anything on this device by connecting to it via another device.")
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.largeSpacing
        }

        Controls.Button {
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            text: qsTr("Enable SSH")
            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("system.ssh.enable", {"display": false})
            }
        }

        Controls.Button {
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            text: qsTr("Disable SSH")
            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("system.ssh.disable", {"display": false})
            }
        }
    }

    footer: Controls.ToolBar {
        contentItem: RowLayout {
            Controls.ToolButton {
                icon.name: "go-previous"
                text: qsTr("Back")
                onClicked: {
                    OVOS.GuiBusClient.sendRequest("mycroft.device.settings", {})
                }
            }
            Kirigami.Heading {
                level: 2
                text: qsTr("SSH Settings")
                Layout.fillWidth: true
            }
        }
    }
}
