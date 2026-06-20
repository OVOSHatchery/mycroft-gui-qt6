/*
 * Copyright 2018 Marco Martin <mart@kde.org>
 * Copyright 2018 by Aditya Mehra <aix.m@outlook.com>
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

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami
import OVOS.GUI 1.0 as OVOS

Kirigami.ScrollablePage {
    title: "Settings"
    objectName: "Settings"

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.Titles
    Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window

    ColumnLayout {
        id: settingsLayout
        width: parent.width
        implicitHeight: childrenRect.height
        spacing: Kirigami.Units.largeSpacing
        
        Kirigami.Heading {
            id: websocketLabel
            level: 2
            font.bold: true
            color: Kirigami.Theme.textColor;
            Layout.fillWidth: true
            text: "OVOS Core Address"
        }
        
        Controls.Label {
            id: exampleLabel
            text: "Example: <tt>ws://192.168.1.1</tt>"
            Layout.fillWidth: true
        }
                
        Controls.Control { 
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing
            
            background: Rectangle {
                Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                color: Kirigami.Theme.backgroundColor
                radius: 5
            }
            
            contentItem: Controls.TextField {
                id: webSocketAddressField
                Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                
                Component.onCompleted: {
                    webSocketAddressField.text = OVOS.GlobalSettings.webSocketAddress
                }
            }
        }

        RowLayout {
           Layout.fillWidth: true
           Layout.preferredHeight: Kirigami.Units.gridUnit * 4
                       
           Controls.Button {
                id: applySettings
                Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                Layout.fillWidth: true
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                text: "Apply"
                
                onClicked:(mouse)=> { 
                    OVOS.GlobalSettings.webSocketAddress = webSocketAddressField.text
                    OVOS.GuiBusClient.reconnect()
                }
            }
           
           Controls.Button{
                id: reverSettings
                Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                Layout.fillWidth: true
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                text: "Revert"
                
                onClicked:(mouse)=> {
                    webSocketAddressField.text = "ws://0.0.0.0"
                    OVOS.GlobalSettings.webSocketAddress = webSocketAddressField.text
                    OVOS.GuiBusClient.reconnect()
                }
            }
        }
        
        Item {
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
        }
        
        Controls.Switch {
            text: "Connect Automatically"
            checked: OVOS.GlobalSettings.autoConnect
            onCheckedChanged: OVOS.GlobalSettings.autoConnect = checked
        }
        
        Controls.Switch {
            id: remoteSTTSwitch
            text: "Remote STT"
            checked: applicationSettings.usesRemoteSTT
            onCheckedChanged: applicationSettings.usesRemoteSTT = checked
            visible: OVOS.GlobalSettings.displayRemoteConfig
        }
        
        Controls.Switch {
            text: "Remote TTS"
            checked: OVOS.GlobalSettings.usesRemoteTTS
            onCheckedChanged: OVOS.GlobalSettings.usesRemoteTTS = checked
            visible: OVOS.GlobalSettings.displayRemoteConfig
        }

        Controls.Switch {
            text: "Bus Isolation"
            checked: OVOS.GlobalSettings.useHivemindProtocol
            onCheckedChanged: OVOS.GlobalSettings.useHivemindProtocol = checked
        }
    }
}
