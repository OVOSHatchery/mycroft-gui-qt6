import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string time_string: ""
    property string date_string: ""

    Timer {
        id: clockTimer
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            var now = new Date()
            time_string = Qt.formatTime(now, "hh:mm:ss")
            date_string = Qt.formatDate(now, "dddd, MMMM d yyyy")
        }
    }

    Component.onCompleted: clockTimer.triggered()

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 8

        Label {
            text: time_string
            font.pixelSize: 72
            font.weight: Font.Light
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: date_string
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
