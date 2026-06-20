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
    id: displaySettingsView
    title: qsTr("Display Settings")

    property bool wallpaper_rotation_enabled: false
    property bool auto_dim_enabled: namespaceData.display_auto_dim ? namespaceData.display_auto_dim : 0
    property bool auto_nightmode_enabled: namespaceData.display_auto_nightmode ? namespaceData.display_auto_nightmode : 0
    property bool menuLabelsEnabled: false

    function getAutoRotation() {
        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.get.auto.rotation", {}, {"session": {"session_id": "default"}})
    }

    Component.onCompleted: {
        getAutoRotation()
        OVOS.GuiBusClient.sendRequest("ovos.shell.get.menuLabels.status", {}, {"session": {"session_id": "default"}})
    }

    Connections {
        target: OVOS.GuiBusClient
        onIntentRecevied: (type, data) => {
            if (type == "ovos.shell.get.menuLabels.status.response") {
                menuLabelsEnabled = data.enabled
            }
            if (type == "ovos.wallpaper.manager.get.auto.rotation.response") {
                wallpaper_rotation_enabled = data.auto_rotation
            }
        }
    }

    ColumnLayout {
        id: mainColLayoutDisplaySettings
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: displaySettingItemOneLabel.implicitHeight + Kirigami.Units.gridUnit
            color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
            border.width: 1
            border.color: Qt.darker(Kirigami.Theme.textColor, 1.5)
            radius: 6

            ColumnLayout {
                id: displaySettingItemOneLabel
                anchors.left: parent.left
                anchors.right: autoWallpaperRotationSwitch.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Kirigami.Units.gridUnit / 2

                Controls.Label {
                    id: settingOneLabel
                    text: qsTr("Wallpaper Rotation")
                    font.pixelSize: 18
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 14
                    color: Kirigami.Theme.textColor
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }

                Controls.Label {
                    text: qsTr("Changes the wallpaper automatically")
                    font.pixelSize: settingOneLabel.font.pixelSize / 1.5
                    color: Kirigami.Theme.textColor
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }
            }

            Controls.Button {
                id: autoWallpaperRotationSwitch
                width: Kirigami.Units.gridUnit * 10
                anchors.right: parent.right
                anchors.rightMargin: Kirigami.Units.gridUnit / 2
                height: parent.height - Kirigami.Units.gridUnit / 2
                anchors.verticalCenter: parent.verticalCenter
                checkable: true
                checked: displaySettingsView.wallpaper_rotation_enabled
                text: checked ? qsTr("ON") : qsTr("OFF")

                Kirigami.Icon {
                    source: autoWallpaperRotationSwitch.checked ? Qt.resolvedUrl("images/switch-green.svg") : Qt.resolvedUrl("images/switch-red.svg")
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    height: Kirigami.Units.iconSizes.medium
                    width: Kirigami.Units.iconSizes.medium
                }

                onClicked: {
                    OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                    if (autoWallpaperRotationSwitch.checked === true) {
                        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.enable.auto.rotation", {}, {"session": {"session_id": "default"}})
                    }
                    else {
                        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.disable.auto.rotation", {}, {"session": {"session_id": "default"}})
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: displaySettingItemTwoLabel.implicitHeight + Kirigami.Units.gridUnit
            color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
            border.width: 1
            border.color: Qt.darker(Kirigami.Theme.textColor, 1.5)
            radius: 6

            ColumnLayout {
                id: displaySettingItemTwoLabel
                anchors.left: parent.left
                anchors.right: autoDimSwitch.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Kirigami.Units.gridUnit / 2

                Controls.Label {
                    id: settingTwoLabel
                    text: qsTr("Auto Dim")
                    font.pixelSize: 18
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 14
                    color: Kirigami.Theme.textColor
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }

                Controls.Label {
                    text: qsTr("Dim's the display in 60 seconds")
                    font.pixelSize: settingTwoLabel.font.pixelSize / 1.5
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                    color: Kirigami.Theme.textColor
                    maximumLineCount: 1
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }
            }

            Controls.Button {
                id: autoDimSwitch
                width: Kirigami.Units.gridUnit * 10
                anchors.right: parent.right
                anchors.rightMargin: Kirigami.Units.gridUnit / 2
                height: parent.height - Kirigami.Units.gridUnit / 2
                anchors.verticalCenter: parent.verticalCenter
                checkable: true
                checked: displaySettingsView.auto_dim_enabled
                text: checked ? qsTr("ON") : qsTr("OFF")

                Kirigami.Icon {
                    source: autoDimSwitch.checked ? Qt.resolvedUrl("images/switch-green.svg") : Qt.resolvedUrl("images/switch-red.svg")
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    height: Kirigami.Units.iconSizes.medium
                    width: Kirigami.Units.iconSizes.medium
                }

                onClicked: {
                    OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                    OVOS.GuiBusClient.sendRequest("speaker.extension.display.set.auto.dim", {"auto_dim": autoDimSwitch.checked})
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: displaySettingItemThreeLabel.implicitHeight + Kirigami.Units.gridUnit
            color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
            border.width: 1
            border.color: Qt.darker(Kirigami.Theme.textColor, 1.5)
            radius: 6

            ColumnLayout {
                id: displaySettingItemThreeLabel
                anchors.left: parent.left
                anchors.right: autoNightmodeSwitch.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Kirigami.Units.gridUnit / 2

                Controls.Label {
                    id: settingThreeLabel
                    text: qsTr("Auto Nightmode")
                    font.pixelSize: 18
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 14
                    color: Kirigami.Theme.textColor
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }

                Controls.Label {
                    text: qsTr("Activates nightmode on homescreen, depending on the time of the day")
                    font.pixelSize: settingThreeLabel.font.pixelSize / 1.5
                    color: Kirigami.Theme.textColor
                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    maximumLineCount: 1
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }
            }

            Controls.Button {
                id: autoNightmodeSwitch
                width: Kirigami.Units.gridUnit * 10
                anchors.right: parent.right
                anchors.rightMargin: Kirigami.Units.gridUnit / 2
                height: parent.height - Kirigami.Units.gridUnit / 2
                anchors.verticalCenter: parent.verticalCenter
                checkable: true
                checked: displaySettingsView.auto_nightmode_enabled
                text: checked ? qsTr("ON") : qsTr("OFF")

                Kirigami.Icon {
                    source: autoNightmodeSwitch.checked ? Qt.resolvedUrl("images/switch-green.svg") : Qt.resolvedUrl("images/switch-red.svg")
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    height: Kirigami.Units.iconSizes.medium
                    width: Kirigami.Units.iconSizes.medium
                }

                onClicked: {
                    OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                    OVOS.GuiBusClient.sendRequest("speaker.extension.display.set.auto.nightmode", {"auto_nightmode": autoNightmodeSwitch.checked})
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: displaySettingItemFourLabel.implicitHeight + Kirigami.Units.gridUnit
            color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
            border.width: 1
            border.color: Qt.darker(Kirigami.Theme.textColor, 1.5)
            radius: 6

            ColumnLayout {
                id: displaySettingItemFourLabel
                anchors.left: parent.left
                anchors.right: displayMenuLabelsSwitch.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Kirigami.Units.gridUnit / 2

                Controls.Label {
                    id: settingFourLabel
                    text: qsTr("Display Menu Labels")
                    font.pixelSize: 18
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 14
                    color: Kirigami.Theme.textColor
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }

                Controls.Label {
                    text: qsTr("Enable|Disable display of menu labels")
                    font.pixelSize: settingFourLabel.font.pixelSize / 1.5
                    color: Kirigami.Theme.textColor
                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    maximumLineCount: 1
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }
            }

            Controls.Button {
                id: displayMenuLabelsSwitch
                width: Kirigami.Units.gridUnit * 10
                anchors.right: parent.right
                anchors.rightMargin: Kirigami.Units.gridUnit / 2
                height: parent.height - Kirigami.Units.gridUnit / 2
                anchors.verticalCenter: parent.verticalCenter
                checkable: true
                checked: displaySettingsView.menuLabelsEnabled
                text: checked ? qsTr("ON") : qsTr("OFF")

                Kirigami.Icon {
                    source: displayMenuLabelsSwitch.checked ? Qt.resolvedUrl("images/switch-green.svg") : Qt.resolvedUrl("images/switch-red.svg")
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    height: Kirigami.Units.iconSizes.medium
                    width: Kirigami.Units.iconSizes.medium
                }

                onClicked: {
                    OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                    OVOS.GuiBusClient.sendRequest("ovos.shell.set.menuLabels", {"enabled": displayMenuLabelsSwitch.checked}, {"session": {"session_id": "default"}})
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        Controls.Button {
            id: wallpaperSettingButton
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
                    text: "Wallpaper Settings"
                    color: Kirigami.Theme.textColor
                    level: 2
                }
            }

            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("mycroft.device.settings.wallpapers", {})
            }
        }
    }
}
