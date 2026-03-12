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
import Qt5Compat.GraphicalEffects
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Controls.Control {
    id: delegateRoot
    property bool toggled
    property alias iconSource: icon.source
    property alias text: label.text


    signal clicked(var mouse)

    leftPadding: applicationSettings.menuLabels ? Kirigami.Units.largeSpacing * 1.25 : Kirigami.Units.largeSpacing
    rightPadding: applicationSettings.menuLabels ? Kirigami.Units.largeSpacing * 1.25 : Kirigami.Units.largeSpacing
    topPadding: applicationSettings.menuLabels ? Kirigami.Units.largeSpacing * 1.25 : Kirigami.Units.largeSpacing
    bottomPadding: applicationSettings.menuLabels ? Kirigami.Units.largeSpacing * 1.25 : Kirigami.Units.largeSpacing

    implicitWidth: Kirigami.Units.iconSizes.medium * 2 + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding

    Layout.fillWidth: true
    Layout.maximumWidth: Kirigami.Units.iconSizes.huge * 2 + leftPadding + rightPadding

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        Kirigami.Icon {
            id: icon
            isMask: true
            color: iconMouseArea.pressed ? Kirigami.Theme.highlightedTextColor : "white"
            Layout.preferredWidth: parent.width / 2
            Layout.preferredHeight: Layout.preferredWidth
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }

        Controls.Label {
            id: label
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            visible: applicationSettings.menuLabels ? 1 : 0
            enabled: applicationSettings.menuLabels ? 1 : 0
            font.pixelSize: parent.height * 0.2
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }
    }

    background: Rectangle {
        radius: Kirigami.Units.largeSpacing
        color: Qt.rgba((Kirigami.Theme.highlightColor.r - 0.115), (Kirigami.Theme.highlightColor.g + 0.001), (Kirigami.Theme.highlightColor.b + 0.001), 1)
        layer.enabled: true
        layer.effect: DropShadow {
            samples: 16
            transparentBorder: true
            horizontalOffset: 2
            verticalOffset: 2
        }
    }

    MouseArea {
        id: iconMouseArea
        anchors.fill: parent
        propagateComposedEvents: true
        onClicked: {
            delegateRoot.clicked(mouse);
            root.delegateClicked();
        }
    }
}

