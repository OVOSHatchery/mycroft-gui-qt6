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
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import org.kde.kirigami as Kirigami
import QtQuick.Window
import QtQuick.Controls.Material 2.0
import "./panel" as Panel
import "./osd" as Osd

// Direct imports (replacing module system)
import "qrc:/qml/Units.qml" as Units
import "qrc:/qml/SoundEffects.qml" as SoundEffects

Kirigami.AbstractApplicationWindow {
    id: root
    visible: true
    visibility: "Maximized"
    flags: Qt.FramelessWindowHint
    property var controllerStatus: OVOSController.status
    property bool platformEGLFS: true
    property bool slidingPanelShouldOpen: false

    function showShutDownDialog() {
        slidingPanel.close()
        shutdownOptions.open()
    }

    onControllerStatusChanged: {
        serviceWatcher.queryGuiServiceIsAlive();
        serviceWatcher.querySkillServiceIsAlive();
    }

    Component.onCompleted: {
        var platform = environmentSummary.readVariable("QT_QPA_PLATFORM")
        if (platform == "eglfs") {
            platformEGLFS = true
        }
        Kirigami.Units.longDuration = 100;
        Kirigami.Units.shortDuration = 100;
        // Color scheme defaults (configuration would come from OVOSPlugin if available)
        palette.mid = "#6e6e6e"
        palette.dark = Qt.darker("#6e6e6e", 1.2)
    }

    Connections {
        target: resetOperations
        onResetOperationsStarted: {
            contentsRect.visible = false
            contentsRect.enabled = false
            factoryResetUI.visible = true
            factoryResetUI.enabled = true
        }
        onResetOperationsFinished: {
            var platform = environmentSummary.readVariable("QT_QPA_PLATFORM")
            if (platform == "eglfs") {
                resetOperations.runRestartDevice()
            }
        }
    }

    Connections {
        target: OVOSController
        onIntentRecevied: {
            if (type == "ovos.shell.exec.factory.reset") {
                var script_to_run_path = data.script
                resetOperations.runResetOperations(script_to_run_path)
            }
            if (type == "ovos.shell.oauth.start.authentication") {
                oauthLoader.url = data.url
                oauthLoader.needsCredentials = data.needs_credentials
                oauthLoader.skillID = data.skill_id
                oauthLoader.appID = data.app_id
                oauthLoader.open()
            }
            if (type == "ovos.shell.oauth.display.qr.code") {
                oauthQrCodeLoader.skillID = data.skill_id
                oauthQrCodeLoader.appID = data.app_id
                oauthQrCodeLoader.qrCodePath = "file://" + data.qr
                oauthQrCodeLoader.open()
            }
            if (type == "ovos.display.screenshot.get") {
                var folderpath = data.folderpath
                var filepath = folderpath + "/" + "screen-" +  Qt.formatDateTime(new Date(), "hhmmss-ddMMyy") + ".png"
                mainView.grabToImage(function(result) {
                    result.saveToFile(filepath);
                });
                OVOSController.sendRequest("ovos.display.screenshot.get.response", {"result": filepath});
            }
            if (type == "ovos.shell.get.menuLabels.status") {
                OVOSController.sendRequest("ovos.shell.get.menuLabels.status.response", {"enabled": applicationSettings.menuLabels});
            }
            if (type == "ovos.shell.set.menuLabels") {
                applicationSettings.menuLabels = data.enabled
                OVOSController.sendRequest("ovos.shell.get.menuLabels.status.response", {"enabled": applicationSettings.menuLabels});
            }
        }
    }

    ShutdownOptions {
        id: shutdownOptions
    }

    Loader {
        anchors.fill: parent
        source: "KdeConnect.qml"
        z: 1200
    }

    FactoryResetUI {
        id: factoryResetUI
        visible: false
        enabled: false
    }

    Timer {
        interval: 20000
        running: GlobalSettings.autoConnect && OVOSController.status != OVOSController.Open
        triggeredOnStart: true
        onTriggered: {
            console.log("Trying to connect to Mycroft");
            OVOSController.start();
            slidingPanel.close();
            // updateSchemeList would be called on OVOSPlugin.Configuration if available
        }
    }

    Rectangle {
        id: contentsRect
        color: "black"
        anchors.centerIn: parent
        rotation: {
            switch (applicationSettings.rotation) {
                case "CW":
                    return 90;
                case "CCW":
                    return -90;
                case "UD":
                    return 180;
                case "NORMAL":
                default:
                    return 0;
            }
        }
        width: rotation === 90 || rotation == -90 ? parent.height : parent.width
        height: rotation === 90 || rotation == -90 ? parent.width : parent.height

        Loader {
            width: parent.width
            height: parent.height
            source: "Keyboard.qml"
            z: 1000
        }

        Image {
            source: "background.png"
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            opacity: !mainView.currentItem && serviceWatcher.guiServiceAlive
            Behavior on opacity {
                OpacityAnimator {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InQuad
                }
            }
        }

        ServiceWatcher {
            id: serviceWatcher
            anchors.fill: parent
        }

        // Homescreen — shown when the skill stack is empty or a show.idle event fires.
        // idle.qml owns the HomescreenController; main.qml only controls visibility.
        Loader {
            id: homescreenLoader
            anchors.fill: parent
            z: 1
            source: "homescreen/idle.qml"
            visible: !mainView.currentItem && serviceWatcher.guiServiceAlive
        }

        StatusIndicator {
            id: si
            enabled: serviceWatcher.guiServiceAlive
            visible: serviceWatcher.guiServiceAlive
            anchors {
                top: parent.top
                right: parent.right
                margins: Kirigami.Units.largeSpacing
            }
            z: 998
        }

        OAuthLoader {
            id: oauthLoader
            z: 6
        }

        OAuthQrCodeLoader {
            id: oauthQrCodeLoader
            z: 6
        }

        Item {
            anchors.fill: parent
            visible: slidingPanel.position < 0.05
            enabled: slidingPanel.position < 0.05
            z: 10

            Osd.VolumeOSD {
                id: volumeOSD
                width: parent.width - Kirigami.Units.gridUnit
                height: Kirigami.Units.gridUnit * 3
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Units.gridUnit * 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.25
                refSlidingPanel: slidingPanel.position
                horizontalMode: root.width > root.height ? 1 : 0
            }
        }

        Flickable {
            id: pullDownMenuFlickableArea
            anchors.fill: parent
            boundsBehavior: Flickable.DragAndOvershootBounds
            flickableDirection: Flickable.VerticalFlick

            rebound: Transition {
                SequentialAnimation {
                    NumberAnimation {
                        properties: "x,y"
                        duration: Kirigami.Units.longDuration * 3
                        easing.type: Easing.InOutCubic
                    }

                    ScriptAction {
                        script: {
                            if(slidingPanelShouldOpen) {
                                slidingPanel.open()
                                slidingPanelShouldOpen = false
                            }
                        }
                    }
                }
            }

            onContentYChanged: {
                if (contentY < 0 && contentY < -height * 0.15) {
                    if(!slidingPanel.menuOpen) {
                        slidingPanelShouldOpen = true
                    }
                }
            }

            onMovementEnded: {
                pullDownMenuFlickableArea.returnToBounds()
            }

            // Template renderer (replaces Mycroft.SkillView)
            Rectangle {
                id: mainView
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                width: contentsRect.width
                height: contentsRect.height
                z: 2
                color: "black"

                Rectangle {
                    anchors.bottom: parent.top
                    width: parent.width
                    height: pullDownMenuFlickableArea.moving && slidingPanelShouldOpen ? contentsRect.height : 0
                    visible: pullDownMenuFlickableArea.moving && (pullDownMenuFlickableArea.contentY < -height * 0.15) ? 1 : 0
                    enabled: pullDownMenuFlickableArea.moving && (pullDownMenuFlickableArea.contentY < -height * 0.15) ? 1 : 0

                    gradient: Gradient {
                        GradientStop { position: 0.8; color: "black"}
                        GradientStop { position: 0.995; color: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.4) }
                        GradientStop { position: 1.0; color: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.1)}
                    }

                    onVisibleChanged: {
                        if(visible && (pullDownMenuFlickableArea.contentY < -height * 0.15)){
                            SoundEffects.playClickedSound("qrc:/sounds/flicked.wav")
                        }
                    }
                }

                ListenerAnimation {
                    id: listenerAnimator
                    anchors.fill: parent
                }

                NotificationsSystem {
                    id: notificationManager
                }
            }
        }

        FastBlur {
            anchors.fill: mainView
            source: mainView
            radius: 50
            visible: slidingPanel.position > 0.5 || shutdownOptions.opened ? 1 : 0
            opacity: slidingPanel.position > 0.5 || shutdownOptions.opened ? 1 : 0
            z: 4
        }

        Panel.SlidingPanel {
            id: slidingPanel
            width: parent.width
            height: parent.height
            verticalMode: contentsRect.height > contentsRect.width ? 1 : 0
            z: 5
        }

        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: (1 - applicationSettings.fakeBrightness) * 0.85
            z: 6
        }
    }
}
