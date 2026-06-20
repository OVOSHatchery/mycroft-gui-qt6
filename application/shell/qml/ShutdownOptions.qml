import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS

Popup {
    id: shutdownMenuPopup
    width: parent.width
    height: parent.height
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        color: Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.2)
        width: parent.width
        height: parent.height

        Image {
            id: shutdownMenuBackgroundImage
            anchors.fill: parent
            source: "icons/scrub.png"
            fillMode: Image.Stretch

            ColorOverlay {
                anchors.fill: parent
                source: shutdownMenuBackgroundImage
                color: Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.2)
            }
        }
    }

    contentItem: Item {

        Item {
            width: rotation === 90 || rotation == -90 ? parent.height : parent.width
            height: rotation === 90 || rotation == -90 ? parent.width : parent.height
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

            GridLayout {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right                
                anchors.margins: OVOS.Units.gridUnit * 4
                id: shutdownMenuGridLayout
                columns: parent.width > parent.height ? 3 : 1
                columnSpacing: OVOS.Units.gridUnit
                rowSpacing: OVOS.Units.gridUnit

                Item {
                    id: menuItemShutdown
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Kirigami.Icon {
                        id: midSolidShutdownIcon
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        source: "qrc://icons/mid-solid"

                        ColorOverlay {
                            anchors.fill: parent
                            source: midSolidShutdownIcon
                            color: Kirigami.Theme.backgroundColor
                        }
                    }

                    Kirigami.Icon {
                        id: shutdownMenuIcon
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        source: "system-shutdown"

                        ColorOverlay {
                            anchors.fill: parent
                            source: shutdownMenuIcon
                            color: Kirigami.Theme.highlightColor
                        }
                    }

                    Label {
                        id: shutdownMenuLabel
                        anchors.top: shutdownMenuIcon.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        horizontalAlignment: Text.AlignHCenter
                        text: "Shutdown"
                        font.pixelSize: parent.height * 0.075
                        color: Kirigami.Theme.textColor
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            OVOS.GuiBusClient.sendRequest("system.shutdown", {"display": true})
                            shutdownMenuPopup.close()
                        }
                        onPressed: {
                            menuItemShutdown.opacity = 0.5
                        }
                        onReleased: {
                            menuItemShutdown.opacity = 1
                        }
                    }
                }

                Item {
                    id: menuItemShutRestart
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Kirigami.Icon {
                        id: midSolidRestartIcon
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        source: "qrc://icons/mid-solid"

                        ColorOverlay {
                            anchors.fill: parent
                            source: midSolidRestartIcon
                            color: Kirigami.Theme.backgroundColor
                        }
                    }

                    Kirigami.Icon {
                        id: restartMenuIcon
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        source: "system-reboot"
                                                
                        ColorOverlay {
                            anchors.fill: parent
                            source: restartMenuIcon
                            color: Kirigami.Theme.highlightColor
                        }
                    }
                    
                    Label {
                        id: restartMenuLabel
                        anchors.top: restartMenuIcon.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        horizontalAlignment: Text.AlignHCenter
                        text: "Restart"
                        font.pixelSize: parent.height * 0.075
                        color: Kirigami.Theme.textColor
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            OVOS.GuiBusClient.sendRequest("system.reboot", {"display": true})
                            shutdownMenuPopup.close()
                        }
                        onPressed: {
                            menuItemShutRestart.opacity = 0.5
                        }
                        onReleased: {
                            menuItemShutRestart.opacity = 1
                        }
                    }
                }

                Item {
                    id: menuItemCancel
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Kirigami.Icon {
                        id: midSolidCancelIcon
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        source: "qrc://icons/mid-solid"

                        ColorOverlay {
                            anchors.fill: parent
                            source: midSolidCancelIcon
                            color: Kirigami.Theme.backgroundColor
                        }
                    }

                    Kirigami.Icon {
                        id: cancelMenuIcon
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        source: "dialog-cancel"
                                                
                        ColorOverlay {
                            anchors.fill: parent
                            source: cancelMenuIcon
                            color: Kirigami.Theme.highlightColor
                        }
                    }
                    
                    Label {
                        id: cancelMenuLabel
                        anchors.top: cancelMenuIcon.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        horizontalAlignment: Text.AlignHCenter
                        text: "Cancel"
                        font.pixelSize: parent.height * 0.075
                        color: Kirigami.Theme.textColor
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            shutdownMenuPopup.close()
                        }
                        onPressed: {
                            menuItemCancel.opacity = 0.5
                        }
                        onReleased: {
                            menuItemCancel.opacity = 1
                        }
                    }
                }
            }
        }
    }
}
