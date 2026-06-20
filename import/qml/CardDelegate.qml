/*
 * Copyright 2021 by Aditya Mehra <aix.m@outlook.com>
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
import OVOS.GUI 1.0 as OVOS

Delegate {
    id: root
    leftPadding: 0
    bottomPadding: 0
    topPadding: 0
    rightPadding: 0
    property int gridUnit: OVOS.Units.gridUnit
    
    skillBackgroundColorOverlay: "black"
    
    rightInset: gridUnit * 2
    leftInset: gridUnit * 2
    topInset: gridUnit * 2
    bottomInset: gridUnit * 2
    
    // Allows cards to set the background image of only the card
    // Allows setting a color overlay for card background
    property alias cardBackgroundOverlayColor: cardBackground.color
    property alias cardBackgroundImage: backgroundImage.source
    property int cardRadius: 20
    
    background: Rectangle {
        id: cardBackground
        radius: cardRadius
        color: Qt.rgba(0, 0, 0, 0.5)
        
        Image {
            id: backgroundImage
            anchors.fill: parent
            z: -1
            source: ""
            visible: source != "" ? 1 : 0 
            layer.enabled: source != "" ? 1 : 0
            layer.effect: OpacityMask {
                maskSource: Rectangle {
                    width: backgroundImage.width
                    height: backgroundImage.height
                    visible: false
                    radius: cardRadius
                }
            }
        }
    }

    contentItem: Item {
        z: 2
    }
}
