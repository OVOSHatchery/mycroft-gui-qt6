import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

// Default idle / resting screen shown when no skill namespace is active.
// Shells and homescreen skills can replace this by showing their own namespace;
// this is only displayed as a fallback when nothing else is visible.
Item {
    id: root

    property string time_string: ""
    property string date_string: ""

    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            var now = new Date()
            time_string = Qt.formatTime(now, "hh:mm")
            date_string = Qt.formatDate(now, "dddd, MMMM d")
        }
    }

    Component.onCompleted: {
        var now = new Date()
        time_string = Qt.formatTime(now, "hh:mm")
        date_string = Qt.formatDate(now, "dddd, MMMM d")
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 8

        Label {
            text: time_string
            font.pixelSize: 96
            font.weight: Font.Light
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: date_string
            font.pixelSize: 28
            opacity: 0.7
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
