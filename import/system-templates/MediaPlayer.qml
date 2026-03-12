import QtQuick
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string media_url: sessionData.media_url || sessionData.video_url || sessionData.url || ""
    property string title:     sessionData.title     || ""
    property string artist:    sessionData.artist    || ""
    property string album:     sessionData.album     || ""
    property string thumbnail: sessionData.thumbnail || ""
    property bool   playing:   sessionData.playing   !== undefined ? sessionData.playing : true
    property var    duration:  sessionData.duration  !== undefined ? sessionData.duration  : 0
    property var    position:  sessionData.position  !== undefined ? sessionData.position  : 0
    property string media_type: sessionData.media_type || "video"  // "video", "audio", or auto-detect

    function formatMs(ms) {
        var s = Math.floor(ms / 1000)
        var m = Math.floor(s / 60)
        s = s % 60
        return m + ":" + (s < 10 ? "0" : "") + s
    }

    MediaPlayer {
        id: player
        source: media_url
        autoPlay: playing
    }

    // Video output for video content
    VideoOutput {
        anchors.fill: parent
        source: player
        fillMode: VideoOutput.PreserveAspectFit
        visible: media_type === "video"
    }

    // Audio UI for audio content
    ColumnLayout {
        anchors.centerIn: parent
        anchors.fill: parent
        width: Math.min(parent.width * 0.8, 420)
        spacing: 16
        visible: media_type === "audio"

        Image {
            visible: thumbnail.length > 0
            source: thumbnail
            width: 200; height: 200
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        Label {
            text: title
            font.pixelSize: 22
            font.weight: Font.DemiBold
            Layout.fillWidth: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            visible: artist.length > 0
            text: artist
            font.pixelSize: 16
            opacity: 0.8
            Layout.fillWidth: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            visible: album.length > 0
            text: album
            font.pixelSize: 14
            opacity: 0.6
            Layout.fillWidth: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }

        ProgressBar {
            visible: duration > 0
            from: 0; to: duration
            value: position
            Layout.fillWidth: true
        }

        RowLayout {
            visible: duration > 0
            Layout.fillWidth: true
            Label { text: formatMs(position); font.pixelSize: 12 }
            Item  { Layout.fillWidth: true }
            Label { text: formatMs(duration); font.pixelSize: 12 }
        }

        Label {
            text: playing ? "▶ Playing" : "⏸ Paused"
            font.pixelSize: 14
            opacity: 0.7
            Layout.alignment: Qt.AlignHCenter
        }

        Item { Layout.fillHeight: true }
    }

    onPlayingChanged: {
        if (playing) player.play()
        else         player.pause()
    }
}
