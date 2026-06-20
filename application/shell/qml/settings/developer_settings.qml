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
    id: developerSettingsView
    title: qsTr("Developer Settings")

    property bool busyVisible: false

    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 10
            visible: developerSettingsView.busyVisible
            color: "transparent"

            Controls.BusyIndicator {
                anchors.centerIn: parent
                running: developerSettingsView.busyVisible
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        Controls.Button {
            id: advancedSettingButton
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(Kirigami.Units.gridUnit * 5, Kirigami.Units.iconSizes.large)

            contentItem: RowLayout {
                Kirigami.Icon {
                    source: "settings"
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                }

                Kirigami.Heading {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    font.weight: Font.DemiBold
                    text: qsTr("Advanced Settings")
                    color: Kirigami.Theme.textColor
                    level: 2
                }
            }

            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("ovos.phal.configuration.provider.list.groups", {})
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
                text: qsTr("Developer Settings")
                Layout.fillWidth: true
            }
        }
    }
}
