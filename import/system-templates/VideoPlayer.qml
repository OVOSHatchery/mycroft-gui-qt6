import QtQuick
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string video_url: sessionData.video_url || sessionData.url || ""
    property bool   playing:   sessionData.playing   !== undefined ? sessionData.playing : true

    MediaPlayer {
        id: player
        source: video_url
        autoPlay: playing
    }

    VideoOutput {
        anchors.fill: parent
        source: player
        fillMode: VideoOutput.PreserveAspectFit
    }

    onPlayingChanged: {
        if (playing) player.play()
        else         player.pause()
    }
}
