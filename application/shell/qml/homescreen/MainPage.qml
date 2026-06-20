import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS.GUI 1.0 as OVOS

Item {
    id: mainPageControl
    property bool dropDownMenuHintPersistence: namespaceData.persistent_menu_hint ? namespaceData.persistent_menu_hint : false

    Control {
        anchors.fill: parent

        contentItem: Item {
            id: mainContentItemArea
            anchors.fill: parent

            SwipeArea {
                id: swipeAreaType
                anchors.fill: parent
                propagateComposedEvents: true
                onSwipe: {
                    if(direction == "up") {
                        controlBarItem.open()
                    }
                    if(direction == "left") {
                        controlBarItem.close()
                        triggerGuiEvent("homescreen.swipe.change.wallpaper", {})
                    }
                    if(direction == "down") {
                        controlBarItem.close()
                    }
                }
            }

            Kirigami.Icon {
                id: downArrowMenuHint
                anchors.top: parent.top
                anchors.topMargin: -OVOS.Units.gridUnit
                anchors.horizontalCenter: parent.horizontalCenter
                width: OVOS.Units.gridUnit * 2.5
                height: OVOS.Units.gridUnit * 2.5
                opacity: mainPageControl.dropDownMenuHintPersistence ? 0.5 : 0
                source:  Qt.resolvedUrl("icons/down.svg")
                color: "white"

                SequentialAnimation {
                    id: downArrowMenuHintAnim
                    running: idleRoot.visible && !mainPageControl.dropDownMenuHintPersistence ? 1 : 0

                    PropertyAnimation {
                        target: downArrowMenuHint
                        property: "opacity"
                        to: 1
                        duration: 1000
                    }

                    PropertyAnimation {
                        target: downArrowMenuHint
                        property: "opacity"
                        to: 0.5
                        duration: 1000
                    }

                    PropertyAnimation {
                        target: downArrowMenuHint
                        property: "opacity"
                        to: 1
                        duration: 1000
                    }

                    PropertyAnimation {
                        target: downArrowMenuHint
                        property: "opacity"
                        to: 0
                        duration: 1000
                    }
                }
            }

            HorizontalDisplayLayout {
                anchors.fill: parent
                spacing: 0
                enabled: horizontalMode ? 1 : 0
                visible: horizontalMode ? 1 : 0
            }

            VerticalDisplayLayout {
                anchors.fill: parent
                spacing: 0
                enabled: horizontalMode ? 0 : 1
                visible: horizontalMode ? 0 : 1
            }
        }
    }
}
