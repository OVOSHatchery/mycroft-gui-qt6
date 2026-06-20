import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS.GUI 1.0 as OVOS

Rectangle {
    id: popbox
    color: "#313131"
    radius: OVOS.Units.gridUnit / 2
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: Kirigami.Units.largeSpacing
    anchors.rightMargin: Kirigami.Units.largeSpacing
    border.width: OVOS.Units.smallSpacing
    border.color: styleAreaNotifier.color
    height: OVOS.Units.gridUnit * 4
    property var currentNotification
    property string notifstyle: currentNotification.style

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
        height: OVOS.Units.gridUnit * 3
        width: OVOS.Units.gridUnit * 3
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
            anchors.margins: OVOS.Units.smallSpacing
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
        anchors.margins: OVOS.Units.largeSpacing

        Label {
            id: notificationContent
            text: currentNotification.text
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.bottomMargin: OVOS.Units.smallSpacing
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: parent.width * 0.045
            fontSizeMode: Text.Fit
            minimumPixelSize: 14
            maximumLineCount: 2
            elide: Text.ElideRight
            color: "#ffffff"
        }

        Kirigami.Separator {
            Layout.preferredWidth: Kirigami.Units.smallSpacing * 0.25
            Layout.fillHeight: true
            color: "#8F8F8F"
        }

        Item {
            Layout.preferredWidth: OVOS.Units.gridUnit * 4
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
                    width: OVOS.Units.iconSizes.medium
                    height: OVOS.Units.iconSizes.medium
                    source: Qt.resolvedUrl("icons/close.svg")
                    }
                }

                onClicked: {
                    popbox.destroy()
                }
            }
        }
    }
}
