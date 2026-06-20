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
    id: homeScreenSettingsView
    title: qsTr("Homescreen Settings")

    property var modelItemList: namespaceData.idleScreenList
    property var activeIdle: namespaceData.selectedScreen

    function checkIfActive(screenId){
        return screenId == activeIdle
    }

    ListView {
        id: listIdleFaces
        width: parent.width
        implicitHeight: contentHeight
        clip: true
        model: modelItemList ? modelItemList.screenBlob : []
        delegate: Kirigami.AbstractListItem {
            contentItem: RowLayout {
                spacing: Kirigami.Units.gridUnit / 2

                Kirigami.Heading {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    font.weight: Font.DemiBold
                    verticalAlignment: Text.AlignVCenter
                    color: Kirigami.Theme.textColor
                    text: modelData.name
                    level: 2
                }

                Kirigami.Icon {
                    source: "images/tick.svg"
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    visible: checkIfActive(modelData.id)
                }
            }

            onClicked: {
                OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("../sounds/clicked.wav"))
                OVOS.GuiBusClient.sendRequest("mycroft.device.set.idle", {"selected": modelData.id})
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
                text: qsTr("Homescreen Settings")
                Layout.fillWidth: true
            }
        }
    }
}
