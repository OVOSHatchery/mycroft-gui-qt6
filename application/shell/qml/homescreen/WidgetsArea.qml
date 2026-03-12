import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import Mycroft 1.0 as Mycroft

Row {
    id: widgetsRow
    property bool verticalMode: false

    Kirigami.Icon {
        id: notificationWigBtn
        width: widgetsRow.verticalMode ? parent.height * 0.5 : parent.height
        height: width
        visible: idleRoot.notificationModel ? (idleRoot.notificationModel.count > 0 ? 1 : 0) : 0
        enabled: idleRoot.notificationModel ? (idleRoot.notificationModel.count > 0 ? 1 : 0) : 0
        source: Qt.resolvedUrl("icons/notificationicon.svg")

        MouseArea {
            anchors.fill: parent
            onClicked: {
                notificationsStorageViewBox.open()
            }
        }

        Rectangle {
            color: "red"
            anchors.right: parent.right
            anchors.rightMargin: -Kirigami.Units.largeSpacing * 0.50
            anchors.top: parent.top
            anchors.topMargin: -Kirigami.Units.largeSpacing * 0.50
            width: parent.width * 0.50
            height: parent.height * 0.50
            radius: width
            z: 10

            Label {
                color: "white"
                anchors.centerIn: parent
                text: idleRoot.notificationModel ? idleRoot.notificationModel.count : 0
            }
        }
    }

    Kirigami.Icon {
        id: timerWigBtn
        width: widgetsRow.verticalMode ? parent.height * 0.5 : parent.height
        height: width
        visible: idleRoot.timerWidgetCount > 0 ? 1 : 0
        enabled: idleRoot.timerWidgetCount > 0 ? 1 : 0
        source: Qt.resolvedUrl("icons/timericon.svg")

        MouseArea {
            anchors.fill: parent
            onClicked: {
                Mycroft.MycroftController.sendRequest("ovos.gui.show.active.timers", {})
            }
        }
    }


    Kirigami.Icon {
        id: alarmWigBtn
        width: widgetsRow.verticalMode ? parent.height * 0.5 : parent.height
        height: width
        visible: idleRoot.alarmWidgetCount > 0 ? 1 : 0
        enabled: idleRoot.alarmWidgetCount > 0 ? 1 : 0
        source: Qt.resolvedUrl("icons/alarmicon.svg")

        MouseArea {
            anchors.fill: parent
            onClicked: {
                Mycroft.MycroftController.sendRequest("ovos.gui.show.active.alarms", {})
            }
        }
    }
}
