import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia
import QtWebEngine
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string mediaType:   namespaceData ? (namespaceData.media_type   || "audio") : "audio"
    property string title:       namespaceData ? (namespaceData.title        || "") : ""
    property string artist:      namespaceData ? (namespaceData.artist       || "") : ""
    property string image:       namespaceData ? (namespaceData.image        || "") : ""
    property string bgImage:     namespaceData ? (namespaceData.bg_image     || "") : ""
    property string uri:         namespaceData ? (namespaceData.uri          || "") : ""
    property bool   playing:     namespaceData ? (namespaceData.playing      !== undefined ? namespaceData.playing   : true) : true
    property var    position:    namespaceData ? (namespaceData.position     !== undefined ? namespaceData.position  : 0)     : 0
    property var    duration:    namespaceData ? (namespaceData.duration     !== undefined ? namespaceData.duration  : 0)     : 0
    property bool   canPrev:     namespaceData ? (namespaceData.can_prev     !== undefined ? namespaceData.can_prev  : true)  : true
    property bool   canNext:     namespaceData ? (namespaceData.can_next     !== undefined ? namespaceData.can_next  : true)  : true
    property string loopStatus:  namespaceData ? (namespaceData.loop_status  || "None") : "None"
    property bool   shuffle:     namespaceData ? (namespaceData.shuffle      !== undefined ? namespaceData.shuffle   : false) : false
    property string javascript:  namespaceData ? (namespaceData.javascript   || "") : ""

    function formatSec(s) {
        var m = Math.floor(s / 60)
        var sec = Math.floor(s % 60)
        return m + ":" + (sec < 10 ? "0" : "") + sec
    }

    // --- Background image (audio mode) ---
    Image {
        visible: root.mediaType === "audio" && root.bgImage.length > 0
        anchors.fill: parent
        source: root.bgImage
        fillMode: Image.PreserveAspectCrop
        opacity: 0.25
        smooth: true
    }

    // === AUDIO mode ===
    ColumnLayout {
        visible: root.mediaType === "audio"
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.85, 460)
        spacing: 16

        Image {
            visible: root.image.length > 0
            source: root.image
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
            opacity: 0.75
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
            Label { text: formatSec(root.position); font.pixelSize: 12 }
            Item  { Layout.fillWidth: true }
            Label { text: formatSec(root.duration); font.pixelSize: 12 }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 24

            Button {
                text: "⏮"
                enabled: root.canPrev
                font.pixelSize: 20
                onClicked: root.triggerGuiEvent("previous", {})
            }
            Button {
                text: root.playing ? "⏸" : "▶"
                font.pixelSize: 20
                onClicked: root.triggerGuiEvent(root.playing ? "pause" : "resume", {})
            }
            Button {
                text: "⏭"
                enabled: root.canNext
                font.pixelSize: 20
                onClicked: root.triggerGuiEvent("next", {})
            }
        }
    }

    // === VIDEO mode ===
    Item {
        visible: root.mediaType === "video"
        anchors.fill: parent

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

        // Minimal overlay controls at the bottom
        RowLayout {
            anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; bottomMargin: 16 }
            spacing: 24
            opacity: 0.85

            Button {
                text: "⏮"; enabled: root.canPrev
                onClicked: root.triggerGuiEvent("previous", {})
            }
            Button {
                text: root.playing ? "⏸" : "▶"
                onClicked: {
                    root.triggerGuiEvent(root.playing ? "pause" : "resume", {})
                }
            }
            Button {
                text: "⏭"; enabled: root.canNext
                onClicked: root.triggerGuiEvent("next", {})
            }
        }

        Label {
            anchors { top: parent.top; left: parent.left; margins: 12 }
            text: root.title
            color: "white"
            font.pixelSize: 16
            visible: root.title.length > 0
        }
    }

    // === WEB mode ===
    Item {
        visible: root.mediaType === "web"
        anchors.fill: parent

        WebEngineView {
            anchors.fill: parent
            url: root.uri
            
            onLoadingChanged: (loadRequest) => {
                if (loadRequest.status === WebEngineView.LoadSucceededStatus
                        && root.javascript.length > 0) {
                    runJavaScript(root.javascript)
                }
            }
        }
    }
}
