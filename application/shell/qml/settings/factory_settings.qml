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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS
import Qt5Compat.GraphicalEffects

Kirigami.ScrollablePage {
    id: factorySettingsView
    title: qsTr("Factory Reset Settings")

    property bool horizontalMode: width > height

    GridLayout {
        id: factorySettingsLayout
        width: parent.width
        columns: horizontalMode ? 2 : 1
        columnSpacing: Kirigami.Units.gridUnit / 2
        rowSpacing: Kirigami.Units.gridUnit / 2

        Controls.Button {
            text: qsTr("Wipe Cache")
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4
            
            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("system.factory.reset", {"wipe_cache": true, "wipe_data": false, "wipe_logs": false, "wipe_config": false, "reset_hardware": false})
            }
        }

        Controls.Button {
            text: qsTr("Wipe Config")
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4

            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("system.factory.reset", {"wipe_cache": false, "wipe_data": false, "wipe_logs": false, "wipe_config": true, "reset_hardware": false})
            }
        }

        Controls.Button {
            text: qsTr("Wipe Data")
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4

            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("system.factory.reset", {"wipe_cache": false, "wipe_data": true, "wipe_logs": false, "wipe_config": false, "reset_hardware": false})
            }
        }

        Controls.Button {
            text: qsTr("Wipe Logs")
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4

            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("system.factory.reset", {"wipe_cache": false, "wipe_data": false, "wipe_logs": true, "wipe_config": false, "reset_hardware": false})
            }
        }

        Controls.Button {
            id: factoryResetButton
            text: qsTr("Factory Reset")
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4

            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("system.factory.reset", {"wipe_cache": true, "wipe_data": true, "wipe_logs": true, "wipe_config": true, "reset_hardware": true})
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
                text: qsTr("Factory Reset")
                Layout.fillWidth: true
            }
        }
    }
}
