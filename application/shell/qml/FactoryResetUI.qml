import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import Mycroft 1.0 as Mycroft

Rectangle {
    id: factoryResetUI
    color: Qt.darker(Kirigami.Theme.backgroundColor, 1.5)
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
    property bool horizontalMode: width >= height ? 1 : 0

    Component.onCompleted: {
        mainAnimRings.start()
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: Mycroft.Units.gridUnit * 4
        color: Kirigami.Theme.highlightColor
        radius: 4
        opacity: 0.8
        z: 6

        Label {
            id: factoryResetLabel
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            text: qsTr("Factory reset in progress... Please wait")
            color: Kirigami.Theme.textColor
        }
    }

    SequentialAnimation {
        id: mainAnimRings
        running: false
        loops: Animation.Infinite

        ParallelAnimation {
            PropertyAnimation {
                target: rectAn1
                property: "value"
                from: 0
                to: 1
                duration: 9000
            }

            PropertyAnimation {
                target: rectAn1
                property: "rotation"
                from: 0
                to: 760
                duration: 9000
            }

            PropertyAnimation {
                target: rectAn2
                property: "value"
                from: 0
                to: 1
                duration: 9000
            }

            PropertyAnimation {
                target: rectAn2
                property: "rotation"
                from: 0
                to: -760
                duration: 9000
            }
        }

        ParallelAnimation {
            PropertyAnimation {
                target: rectAn1
                property: "value"
                from: 1
                to: 0
                duration: 12000
            }

            PropertyAnimation {
                target: rectAn1
                property: "rotation"
                from: 760
                to: 0
                duration: 12000
            }

            PropertyAnimation {
                target: rectAn2
                property: "value"
                from: 1
                to: 0
                duration: 12000
            }

            PropertyAnimation {
                target: rectAn2
                property: "rotation"
                from: -760
                to: 0
                duration: 12000
            }
        }
    }

    Rounder {
        id: rectAn1
        width: centeralCircle.width + 90
        height: width
        anchors.centerIn: parent
        glowColor: Kirigami.Theme.textColor
        backgroundBorderColor: "#313131"
        z: 2
    }

    Rounder {
        id: rectAn2
        width: centeralCircle.width + 80
        height: width
        anchors.centerIn: parent
        glowColor: Kirigami.Theme.highlightColor
        backgroundBorderColor: "#313131"
        z: 2
    }

    Rectangle {
        id: centeralCircle
        width: horizontalMode ? parent.height * 0.75 : parent.width * 0.50
        height: width
        anchors.centerIn: parent
        color: Kirigami.Theme.backgroundColor
        radius: 500

        ProgressBar {
            id: progressBar
            width: parent.width * 0.8
            height: Mycroft.Units.gridUnit * 4
            anchors.centerIn: parent
            indeterminate: true
        }
    }
}
