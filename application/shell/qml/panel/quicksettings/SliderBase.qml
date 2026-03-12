/*
 * Copyright 2020 Aditya Mehra <Aix.m@outlook.com>
 * Copyright 2018 by Marco Martin <mart@kde.org>
 * Copyright 2018 David Edmundson <davidedmundson@kde.org>
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
import QtQuick.Templates 2.2 as T
import org.kde.kirigami as Kirigami
import Mycroft 1.0 as Mycroft

Controls.Control {
    id: root

    property alias iconSource: icon.source
    property alias slider: slider
    property alias sliderButtonLabel: sliderButtonLabel.text
    signal iconClicked

    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    Layout.fillWidth: true
    Layout.preferredHeight: Kirigami.Units.gridUnit * 3
    contentItem: MouseArea {
        //to not cause unwanted scrolls
        preventStealing: true
        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

            Kirigami.Icon {
                id: icon
                isMask: true
                Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                Layout.preferredHeight: Layout.preferredWidth
                color: Qt.rgba((Kirigami.Theme.highlightColor.r - 0.115), (Kirigami.Theme.highlightColor.g + 0.001), (Kirigami.Theme.highlightColor.b + 0.001), 1)

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        iconClicked()
                    }
                }
            }

            T.Slider {
                id: slider
                Layout.fillWidth: true
                Layout.fillHeight: true
                handle: Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    x: slider.visualPosition * (slider.width - width)
                    color: Qt.rgba((Kirigami.Theme.highlightColor.r - 0.115), (Kirigami.Theme.highlightColor.g + 0.001), (Kirigami.Theme.highlightColor.b + 0.001), 1)
                    radius: width
                    implicitWidth: Kirigami.Units.gridUnit * 3
                    implicitHeight: Kirigami.Units.gridUnit * 2

                    Controls.Label {
                        id: sliderButtonLabel
                        text: Math.round(slider.position * 10)
                        anchors.centerIn: parent
                        Layout.preferredWidth: textMetrics.width
                        color: Kirigami.Theme.textColor
                        TextMetrics {
                            id
                            : textMetrics
                            text: "10"
                        }
                    }
                }

                background: Rectangle {
                    x: slider.leftPadding
                    y: (slider.height - height) / 2
                    width: slider.availableWidth
                    height: Kirigami.Units.gridUnit * 1.25
                    radius: width
                    color: Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.8)

                    Rectangle {
                        width: slider.visualPosition * parent.width
                        height: parent.height
                        color: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.8)
                        radius: 100
                    }
                }
            }
        }
    }

    background: Rectangle {
        radius: Kirigami.Units.largeSpacing
        color: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.4)
    }
}
