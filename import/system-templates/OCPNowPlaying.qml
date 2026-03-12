import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia

Item {
    id: root

    property string mediaType:   sessionData.media_type   || "audio"
    property string title:       sessionData.title        || ""
    property string artist:      sessionData.artist       || ""
    property string image:       sessionData.image        || ""
    property string bgImage:     sessionData.bg_image     || ""
    property string uri:         sessionData.uri          || ""
    property bool   playing:     sessionData.playing      !== undefined ? sessionData.playing   : true
    property var    position:    sessionData.position     !== undefined ? sessionData.position  : 0
    property var    duration:    sessionData.duration     !== undefined ? sessionData.duration  : 0
    property bool   canPrev:     sessionData.can_prev     !== undefined ? sessionData.can_prev  : true
    property bool   canNext:     sessionData.can_next     !== undefined ? sessionData.can_next  : true
    property string loopStatus:  sessionData.loop_status  || "None"
    property bool   shuffle:     sessionData.shuffle      !== undefined ? sessionData.shuffle   : false
    property string javascript:  sessionData.javascript   || ""

    function formatSec(s) {
        var m = Math.floor(s / 60)
        var sec = Math.floor(s % 60)
        return m + ":" + (sec < 10 ? "0" : "") + sec
    }

    // --- Background image (audio mode) ---
    Image {
        visible: mediaType === "audio" && bgImage.length > 0
        anchors.fill: parent
        source: bgImage
        fillMode: Image.PreserveAspectCrop
        opacity: 0.25
        smooth: true
    }

    // === AUDIO mode ===
    ColumnLayout {
        visible: mediaType === "audio"
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.85, 460)
        spacing: 16

        Image {
            visible: image.length > 0
            source: image
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
            opacity: 0.75
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
            Label { text: formatSec(position); font.pixelSize: 12 }
            Item  { Layout.fillWidth: true }
            Label { text: formatSec(duration); font.pixelSize: 12 }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 24

            Button {
                text: "⏮"
                enabled: canPrev
                font.pixelSize: 20
                onClicked: triggerEvent("previous", {})
            }
            Button {
                text: playing ? "⏸" : "▶"
                font.pixelSize: 20
                onClicked: triggerEvent(playing ? "pause" : "resume", {})
            }
            Button {
                text: "⏭"
                enabled: canNext
                font.pixelSize: 20
                onClicked: triggerEvent("next", {})
            }
        }
    }

    // === VIDEO mode ===
    Item {
        visible: mediaType === "video"
        anchors.fill: parent

        MediaPlayer {
            id: videoPlayer
            source: mediaType === "video" ? uri : ""
            autoPlay: playing && mediaType === "video"
        }

        VideoOutput {
            anchors.fill: parent
            source: videoPlayer
            fillMode: VideoOutput.PreserveAspectFit
        }

        onVisibleChanged: {
            if (!visible) videoPlayer.stop()
        }

        // Minimal overlay controls at the bottom
        RowLayout {
            anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; bottomMargin: 16 }
            spacing: 24
            opacity: 0.85

            Button {
                text: "⏮"; enabled: canPrev
                onClicked: triggerEvent("previous", {})
            }
            Button {
                text: playing ? "⏸" : "▶"
                onClicked: {
                    triggerEvent(playing ? "pause" : "resume", {})
                    if (playing) videoPlayer.pause(); else videoPlayer.play()
                }
            }
            Button {
                text: "⏭"; enabled: canNext
                onClicked: triggerEvent("next", {})
            }
        }

        Label {
            anchors { top: parent.top; left: parent.left; margins: 12 }
            text: title
            color: "white"
            font.pixelSize: 16
            visible: title.length > 0
        }
    }

    // === WEB mode ===
    Item {
        visible: mediaType === "web"
        anchors.fill: parent

        // Loader lets us avoid the QtWebEngine import on systems that lack it
        Loader {
            id: webLoader
            anchors.fill: parent
            active: mediaType === "web" && uri.length > 0
            sourceComponent: webComponent
        }

        Component {
            id: webComponent
            WebEngineView {
                url: uri
                onLoadingChanged: {
                    if (loadRequest.status === WebEngineView.LoadSucceededStatus
                            && javascript.length > 0) {
                        runJavaScript(javascript)
                    }
                }
            }
        }
    }
}
