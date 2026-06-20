import QtQuick
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string video_url: namespaceData ? (namespaceData.video_url || namespaceData.url || "") : ""
    property bool   playing:   namespaceData ? (namespaceData.playing   !== undefined ? namespaceData.playing : true) : true

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectFit

        Component.onCompleted: {
            OVOS.MediaService.setVideoSink(videoOutput.videoSink)
        }
        
        Component.onDestruction: {
            OVOS.MediaService.setVideoSink(null)
        }
    }
}
