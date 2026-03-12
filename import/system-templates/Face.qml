import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property bool sleeping: sessionData.sleeping !== undefined ? sessionData.sleeping : false

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
                width: 30; height: sleeping ? 4 : 30
                radius: sleeping ? 2 : 15
                color: "#333"
                Behavior on height { NumberAnimation { duration: 300 } }
            }

            Rectangle {
                width: 30; height: sleeping ? 4 : 30
                radius: sleeping ? 2 : 15
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
