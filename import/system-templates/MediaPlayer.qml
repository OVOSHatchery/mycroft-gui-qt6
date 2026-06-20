import QtQuick
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string media_url: namespaceData ? (namespaceData.media_url || namespaceData.video_url || namespaceData.url || "") : ""
    property string title:     namespaceData ? (namespaceData.title     || "") : ""
    property string artist:    namespaceData ? (namespaceData.artist    || "") : ""
    property string album:     namespaceData ? (namespaceData.album     || "") : ""
    property string thumbnail: namespaceData ? (namespaceData.thumbnail || "") : ""
    property bool   playing:   namespaceData ? (namespaceData.playing   !== undefined ? namespaceData.playing : true) : true
    property var    duration:  namespaceData ? (namespaceData.duration  !== undefined ? namespaceData.duration  : 0)     : 0
    property var    position:  namespaceData ? (namespaceData.position  !== undefined ? namespaceData.position  : 0)     : 0
    property string media_type: namespaceData ? (namespaceData.media_type || "video") : "video"

    function formatMs(ms) {
        var s = Math.floor(ms / 1000)
        var m = Math.floor(s / 60)
        s = s % 60
        return m + ":" + (s < 10 ? "0" : "") + s
    }

    // Video output for video content
    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectFit
        visible: root.media_type === "video"
        
        Component.onCompleted: {
            OVOS.MediaService.setVideoSink(videoOutput.videoSink)
        }
        
        Component.onDestruction: {
            OVOS.MediaService.setVideoSink(null)
        }
    }

    // Web output for web content
    WebEngineView {
        anchors.fill: parent
        url: root.media_type === "web" ? root.media_url : ""
        visible: root.media_type === "web"
    }

    // Audio UI for audio content

    ColumnLayout {
        anchors.centerIn: parent
        anchors.fill: parent
        width: Math.min(parent.width * 0.8, 420)
        spacing: 16
        visible: root.media_type === "audio"

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

        Item { Layout.fillHeight: true }
    }
}
