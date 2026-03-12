import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    // seconds remaining / elapsed; updated by session data
    property int    seconds:   sessionData.seconds   !== undefined ? sessionData.seconds   : 0
    property string label:     sessionData.label     || ""
    property bool   countdown: sessionData.countdown !== undefined ? sessionData.countdown : true

    function formatTime(s) {
        var h = Math.floor(s / 3600)
        var m = Math.floor((s % 3600) / 60)
        var sec = s % 60
        var parts = []
        if (h > 0) parts.push(h < 10 ? "0" + h : h)
        parts.push(m < 10 ? "0" + m : m)
        parts.push(sec < 10 ? "0" + sec : sec)
        return parts.join(":")
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12

        Label {
            visible: label.length > 0
            text: label
            font.pixelSize: 22
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: formatTime(seconds)
            font.pixelSize: 72
            font.weight: Font.Light
            font.family: "Monospace"
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
