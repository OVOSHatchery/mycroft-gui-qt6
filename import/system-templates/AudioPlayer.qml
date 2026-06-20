import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string title:     namespaceData ? (namespaceData.title     || "") : ""
    property string artist:    namespaceData ? (namespaceData.artist    || "") : ""
    property string album:     namespaceData ? (namespaceData.album     || "") : ""
    property string thumbnail: namespaceData ? (namespaceData.thumbnail || "") : ""
    property bool   playing:   namespaceData ? (namespaceData.playing   !== undefined ? namespaceData.playing   : false) : false
    property var    duration:  namespaceData ? (namespaceData.duration  !== undefined ? namespaceData.duration  : 0)     : 0
    property var    position:  namespaceData ? (namespaceData.position  !== undefined ? namespaceData.position  : 0)     : 0

    function formatMs(ms) {
        var s = Math.floor(ms / 1000)
        var m = Math.floor(s / 60)
        s = s % 60
        return m + ":" + (s < 10 ? "0" : "") + s
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.8, 420)
        spacing: 16

        Image {
            visible: root.thumbnail.length > 0
            source: root.thumbnail
            width: 200; height: 200
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        Label {
            text: root.title
            font.pixelSize: 22
            font.weight: Font.DemiBold
            Layout.fillWidth: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            visible: root.artist.length > 0
            text: root.artist
            font.pixelSize: 16
            opacity: 0.8
            Layout.fillWidth: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            visible: root.album.length > 0
            text: root.album
            font.pixelSize: 14
            opacity: 0.6
            Layout.fillWidth: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        ProgressBar {
            visible: root.duration > 0
            from: 0; to: root.duration
            value: root.position
            Layout.fillWidth: true
        }

        RowLayout {
            visible: root.duration > 0
            Layout.fillWidth: true
            Label { text: formatMs(root.position); font.pixelSize: 12 }
            Item  { Layout.fillWidth: true }
            Label { text: formatMs(root.duration); font.pixelSize: 12 }
        }

        Label {
            text: root.playing ? "▶ Playing" : "⏸ Paused"
            font.pixelSize: 14
            opacity: 0.7
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
