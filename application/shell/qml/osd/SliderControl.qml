/*
 * Copyright 2020 Aditya Mehra <Aix.m@outlook.com>
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
import Qt5Compat.GraphicalEffects
import QtQuick.Window
import OVOS.GUI 1.0 as OVOS

Controls.Control {
    id: root

    property alias iconSource: iconSlider.source
    property real value
    property real changeValue
    property bool horizontalMode

    leftPadding: OVOS.Units.gridUnit * 0.4
    rightPadding: OVOS.Units.gridUnit * 0.4
    topPadding: OVOS.Units.gridUnit * 0.4
    bottomPadding: OVOS.Units.gridUnit * 0.4

    onValueChanged: {
        valMeter.value = value / 10
    }

    contentItem: MouseArea {
        preventStealing: true

        Rectangle {
            id: iconHolderArea
            color: "transparent"
            anchors.left: parent.left
            width: parent.width * 0.10
            height: parent.height

            Kirigami.Icon {
                id: iconSlider
                anchors.fill: parent
                anchors.margins: root.width > 800 ? OVOS.Units.gridUnit * 0.4 : OVOS.Units.gridUnit * 0.4
                color: "white"
                layer.enabled: true
                layer.effect: DropShadow {
                    anchors.fill: parent
                    anchors.margins: root.width > 800 ? OVOS.Units.gridUnit * 0.4 : OVOS.Units.gridUnit * 0.4
                    radius: 8
                    samples: 16
                    verticalOffset: 0
                    horizontalOffset: 0
                    spread: 0.5
                }
            }
        }

        Rectangle {
            id: rowButtonsArea
            anchors.left: iconHolderArea.right
            anchors.right: labelValHolderArea.left
            height: parent.height
            color: "transparent"

            RowLayout {
                id: rwl
                width: Math.round(parent.width)
                height: parent.height
                anchors.centerIn: parent
                spacing: 0

                Repeater {
                    id: valMeter
                    model: 10
                    property real value

                    Rectangle {
                        id: sct
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width / 10
                        color: "transparent"

                        Rectangle {
                            width: Math.floor(parent.width / 3)
                            height: width
                            anchors.centerIn: parent
                            color: index > (valMeter.value - 1) ? "#222" : "#eaf4ff"
                            radius: 100
                            layer.enabled: true
                            layer.effect: DropShadow {
                                radius: 8
                                samples: 16
                                verticalOffset: 0
                                horizontalOffset: 0
                                spread: 0.1
                                width: parent.width
                                height: parent.height
                            }

                            Rectangle {
                                anchors.centerIn: parent
                                color: index > (valMeter.value - 1) ? "transparent" : "white"
                                width: parent.width / 1.5
                                height: parent.height / 1.5
                                radius: 100
                                layer.enabled: true
                                layer.effect: Glow {
                                    color: index > (valMeter.value - 1) ? "transparent" : "white"
                                    width: parent.width
                                    height: parent.height
                                    radius: 12
                                    samples: 24
                                    spread: 0.3
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                valMeter.value = (index + 1)
                                root.changeValue = ((index + 1) / 10)
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: labelValHolderArea
            color: "transparent"
            anchors.right: parent.right
            width: parent.width * 0.10
            height: parent.height

            Controls.Label {
                id: labelval
                anchors.fill: parent
                fontSizeMode: Text.Fit
                minimumPixelSize: 3
                font.pixelSize: root.width > 800 ? 50 : 30
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: "white"
                text: valMeter.value * 10
                layer.enabled: true
                layer.effect: DropShadow {
                    radius: 8
                    samples: 16
                    verticalOffset: 0
                    horizontalOffset: 0
                    spread: 0.1
                    width: parent.width
                    height: parent.height
                }
            }
        }
    }

    background: Rectangle {
        radius: Kirigami.Units.largeSpacing
        color: Qt.rgba(0.2, 0.2, 0.2, 0.8)
        layer.enabled: true
        layer.effect: DropShadow {
            anchors.fill: parent
            radius: 8
            samples: 17
            horizontalOffset: 0
            verticalOffset: 0
            spread: 0.4
        }
    }
}
