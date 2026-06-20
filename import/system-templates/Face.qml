import QtQuick
import QtQuick.Layouts
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property bool sleeping: namespaceData ? (namespaceData.sleeping !== undefined ? namespaceData.sleeping : false) : false

    Rectangle {
        anchors.centerIn: parent
        width: 200; height: 200
        radius: 100
        color: "#f5c842"

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -20
            spacing: 40

            Rectangle {
                width: 30; height: root.sleeping ? 4 : 30
                radius: root.sleeping ? 2 : 15
                color: "#333"
                Behavior on height { NumberAnimation { duration: 300 } }
            }

            Rectangle {
                width: 30; height: root.sleeping ? 4 : 30
                radius: root.sleeping ? 2 : 15
                color: "#333"
                Behavior on height { NumberAnimation { duration: 300 } }
            }
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 40
            width: 60; height: 10
            radius: 5
            color: "#333"
        }
    }
}
