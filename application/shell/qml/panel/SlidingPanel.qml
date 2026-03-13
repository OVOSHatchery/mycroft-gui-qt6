import QtQuick
import QtQuick.Controls
import QtQuick.Window
import org.kde.kirigami as Kirigami
import QtQuick.Layouts
import "quicksettings"
import OVOS 1.0 as OVOS

Item {
    id: pullControlRoot
    width: parent.width
    height: parent.height
    property alias position: pullDownRoot.menuPosition
    property alias menuOpen: pullDownRoot.menuOpen
    property bool verticalMode

    function open() {
        snapAnimationTpBt.restart();
        mainContents.y = 0
    }

    function close() {
        closeAnimationTpBt.restart();
        mainContents.y = -pullControlRoot.height
    }

    Control {
        id: pullDownRoot
        anchors.top: parent.top
        width: pullControlRoot.width
        height: pullControlRoot.height * menuPosition
        property bool menuOpen: false
        property bool menuClosed: true
        property bool menuDragging: false
        property int menuDragDirection
        property real menuPosition: 0.0
        property real dragPositionTopToBottom: 0.0
        property real dragPositionBottomToTop: 0.0

        onMenuOpenChanged: {
            if(menuOpen) {
                OVOS.OVOSController.sendRequest("mycroft.volume.get", {})
            }
        }

        Connections {
            target: mouseSwipeArea
            onCustomDragReleased: {
                if(pullDownRoot.menuDragDirection == 1){
                    if(pullDownRoot.menuPosition >= 0.1) {
                        snapAnimationTpBt.restart()
                    } else if(pullDownRoot.menuPosition < 0.1){
                        closeAnimationTpBt.restart()
                    }
                }
                if(pullDownRoot.menuDragDirection == 2){
                    if(pullDownRoot.menuPosition > 0.6) {
                        snapAnimationTpBt.restart()
                    } else if (pullDownRoot.menuPosition <= 0.6){
                        closeAnimationTpBt.restart()
                    }
                }
            }
        }

        SequentialAnimation {
            id: closeAnimationTpBt
            NumberAnimation {
                target: pullDownRoot
                property: "menuPosition"
                from: pullDownRoot.menuPosition
                to: 0.0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        SequentialAnimation {
            id: snapAnimationTpBt
            NumberAnimation {
                target: pullDownRoot
                property: "menuPosition"
                from: pullDownRoot.menuPosition
                to: 1.0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        function calculate_menu_position(dragPosition, direction) {
            if (direction == "TopToBottom") {
                menuPosition = Math.round(Math.max(0.0, Math.min(1.0, dragPosition * 2.0 - 1.0)) * 100.0) / 100.0
                if(dragPosition < 0.5 && dragPosition > 0.1) {
                    menuPosition = dragPosition
                }
                pullDownRoot.menuDragDirection = 1
            }
            if (direction == "BottomToTop") {
                menuPosition = Math.round(Math.max(0.0, Math.min(1.0, 1.0 - dragPosition * 2.0)) * 100.0) / 100.0
                pullDownRoot.menuDragDirection = 2
            }
        }

        function calculate_mainContents_y(){
            if (pullDownRoot.menuDragDirection == 1) {
                mainContents.y = -pullControlRoot.height + pullDownRoot.menuPosition * pullControlRoot.height
            }
            if (pullDownRoot.menuDragDirection == 2) {
                mainContents.y = -pullControlRoot.height + pullDownRoot.menuPosition * pullControlRoot.height
            }
        }

        onDragPositionTopToBottomChanged: {
            calculate_menu_position(dragPositionTopToBottom, "TopToBottom")
        }

        onDragPositionBottomToTopChanged: {
            calculate_menu_position(dragPositionBottomToTop, "BottomToTop")
        }

        onMenuPositionChanged: {
            if (menuPosition < 0) {
                menuPosition = 0
            }

            if (menuPosition == 0.0) {
                menuClosed = true
                menuOpen = false
                menuDragging = false
            } else if (menuPosition == 1.0) {
                menuClosed = false
                menuOpen = true
                menuDragging = false
            } else {
                menuClosed = false
                menuOpen = false
                menuDragging = true
            }

            calculate_mainContents_y()
        }

        background: Rectangle {
            color: "black"
            opacity: pullDownRoot.menuPosition < 0.6 ? Math.min(1, pullDownRoot.menuPosition) * 0.2 : Math.min(1, pullDownRoot.menuPosition) * 0.6
            visible: pullDownRoot.menuPosition > 0
            height: pullDownRoot.menuPosition < 0.6 ? quickSettings.implicitHeight : parent.height
        }

        MouseArea {
            id: mouseSwipeArea
            width: pullControlRoot.width
            height: pullDownRoot.height == 0 ?  0 : pullDownRoot.height
            propagateComposedEvents: true
            property real prevX: 0
            property real prevY: 0
            property real velocityX: 0.0
            property real velocityY: 0.0
            property int startX: 0
            property int startY: 0
            property bool tracing: false

            signal customDragReleased()

            onClicked: {
                mouse.accepted = false
            }

            onPressed: {
                startX = mouse.x
                startY = mouse.y
                prevX = mouse.x
                prevY = mouse.y
                velocityX = 0
                velocityY = 0
                tracing = true
            }

            onReleased: {
                customDragReleased()
                velocityX = 0
                velocityY = 0
                tracing = false
            }

            onPositionChanged: {
                if ( !tracing ) return
                var currVelX = (mouse.x-prevX)
                var currVelY = (mouse.y-prevY)

                velocityX = (velocityX + currVelX)/2.0;
                velocityY = (velocityY + currVelY)/2.0;

                prevX = mouse.x
                prevY = mouse.y

                // if ( mouse.y > startY && velocityY > 1) {
                //     pullDownRoot.dragPositionTopToBottom = (mouse.y - startY) / pullControlRoot.height
                // }

                if(pullDownRoot.menuPosition > 0.5){
                    if ( mouse.y < startY && velocityY < -2) {
                        pullDownRoot.dragPositionBottomToTop = (startY - mouse.y) / pullControlRoot.height
                    }
                }
            }
        }

        Item {
            id: mainContents
            width: parent.width
            height: pullControlRoot.height
            y: -pullControlRoot.height

            ColumnLayout {
                id: contentsLayout
                spacing: 0

                anchors {
                    fill: parent
                    margins: Kirigami.Units.largeSpacing * 2
                }
                QuickSettings {
                    id: quickSettings
                    Layout.fillWidth: true
                    onDelegateClicked: pullControlRoot.close();
                    verticalMode: pullControlRoot.verticalMode
                }
                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
}
