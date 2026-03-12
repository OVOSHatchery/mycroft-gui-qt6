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
import QtQuick.VirtualKeyboard 2.4


Item {

    states: [
        State {
            name: "rot0"
            when: inputAreaPanel.rotation == 0
            AnchorChanges {
                target: inputAreaPanel
                anchors.right: parent.right
                anchors.left: parent.left
            }
            PropertyChanges {
                target: inputAreaPanel
                width: parent.width
            }

        },
        State {
            name: "rot180"
            when: inputAreaPanel.rotation == 180
            AnchorChanges {
                target: inputAreaPanel
                anchors.right: parent.right
                anchors.left: parent.left
            }
            PropertyChanges {
                target: inputAreaPanel
                width: parent.width
            }

        },
        State {
            name: "rot90"
            when: inputAreaPanel.rotation === 90
            AnchorChanges {
                target: inputAreaPanel
                anchors.right: undefined
                anchors.left: parent.left
            }
            PropertyChanges {
                target: inputAreaPanel
                width: parent.width * 0.55
            }

        },
        State {
            name: "rot90n"
            when: inputAreaPanel.rotation === -90
            AnchorChanges {
                target: inputAreaPanel
                anchors.right: parent.right
                anchors.left: undefined
            }
            PropertyChanges {
                target: inputAreaPanel
                width: parent.width * 0.55
            }
        }
    ]

    InputPanel {
        height: parent.height
        rotation: contentsRect.rotation

        id: inputAreaPanel
        visible: active
        y: active ? parent.height - height : parent.height
    }
}
