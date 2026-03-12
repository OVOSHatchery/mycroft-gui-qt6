import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import Mycroft 1.0 as Mycroft

Rectangle {
    id: popbox
    color: "#313131"
    radius: Mycroft.Units.gridUnit / 2
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: Kirigami.Units.largeSpacing
    anchors.rightMargin: Kirigami.Units.largeSpacing
    border.width: Mycroft.Units.smallSpacing
    border.color: styleAreaNotifier.color
    height: Mycroft.Units.gridUnit * 4
    property var currentNotification
    property string notifstyle: currentNotification.style
    property int notifduration: currentNotification.duration * 1000

    function getBoxWidth(){
        return popbox.width;
    }

    Item {
        id: timerBarBox
        anchors.left: parent.left
        anchors.leftMargin: Mycroft.Units.smallSpacing
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Mycroft.Units.smallSpacing
        z: 2

        Rectangle {
            id: timerBar
            color: "white"
            anchors.bottom: parent.bottom
            width: parent.width
            height: Mycroft.Units.smallSpacing
            radius: popbox.radius

            PropertyAnimation {
                id: timerBarAnimation
                target: timerBar
                property: "width"
                to: 0
                duration: notifduration
                running: timerBar.visible && timerBar.width > 0 ? 1 : 0
                onRunningChanged: {
                    timerBarAnimation.from = getBoxWidth()
                }
            }
        }
    }

    Timer {
        id: notificationEndTimer
        interval: notifduration
        repeat: false
        running: parent.visible
        onTriggered: {
            notificationsSys.clearNotificationSessionData()
            parent.destroy()
        }
    }

    Rectangle {
        id: styleAreaNotifier
        color: switch(popbox.notifstyle) {
            case "info":
                return "#3498db"
            case "warning":
                return "#cf850f"
            case "success":
                return "#00bc8c"
            case "error":
                return "#e74c3c"
            default:
                return "#3498db"
        }
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: Mycroft.Units.gridUnit * 3
        width: Mycroft.Units.gridUnit * 3
        radius: parent.radius / 2
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            horizontalOffset: 3
            samples: 16
            verticalOffset: 0
            spread: 0.3
            color: Qt.rgba(0, 0, 0, 0.4)
        }

        Kirigami.Icon {
            anchors.fill: parent
            anchors.margins: Mycroft.Units.smallSpacing
            source: switch(popbox.notifstyle) {
                case "info":
                    return "documentinfo"
                case "warning":
                    return "data-warning"
                case "success":
                    return "emblem-success"
                case "error":
                    return "emblem-error"
                default:
                    return "documentinfo"
            }
            color: "white"
        }
    }

    RowLayout {
        id: notificationRowBoxLayout
        anchors.left: styleAreaNotifier.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: Mycroft.Units.largeSpacing

        Label {
            id: notificationContent
            text: currentNotification.text
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.bottomMargin: Mycroft.Units.smallSpacing
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: parent.width * 0.045
            fontSizeMode: Text.Fit
            minimumPixelSize: 14
            maximumLineCount: 2
            elide: Text.ElideRight
            color: "#ffffff"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (currentNotification.action){
                        Mycroft.MycroftController.sendRequest(currentNotification.action, currentNotification.callback_data)
                        popbox.destroy()
                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.preferredWidth: Kirigami.Units.smallSpacing * 0.25
            Layout.fillHeight: true
            color: "#8F8F8F"
        }

        Item {
            Layout.preferredWidth: Mycroft.Units.gridUnit * 4
            Layout.fillHeight: true

            AbstractButton {
                width: parent.width - Kirigami.Units.largeSpacing * 2
                height: width
                anchors.centerIn: parent

                background: Rectangle {
                    color: "transparent"
                }

                contentItem: Item {
                    Kirigami.Icon {
                    anchors.centerIn: parent
                    width: Mycroft.Units.iconSizes.medium
                    height: Mycroft.Units.iconSizes.medium
                    source: Qt.resolvedUrl("icons/close.svg")
                    }
                }

                onClicked: {
                    Mycroft.MycroftController.sendRequest("ovos.notification.api.pop.clear.delete", {"notification": currentNotification})
                    popbox.destroy()
                }
            }
        }
    }
}
