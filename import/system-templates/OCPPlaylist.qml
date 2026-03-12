import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var tracks:       sessionData.tracks        || []
    property int currentIndex: sessionData.current_index !== undefined
                               ? sessionData.current_index : 0

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            text: qsTr("Queue")
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: tracks
            spacing: 2

            delegate: ItemDelegate {
                id: trackDelegate
                width: parent.width

                readonly property bool isCurrent: index === currentIndex

                background: Rectangle {
                    color: trackDelegate.isCurrent
                           ? Qt.rgba(1, 1, 1, 0.12)
                           : "transparent"
                    radius: 4
                }

                contentItem: RowLayout {
                    spacing: 12

                    // Playing indicator
                    Label {
                        text: trackDelegate.isCurrent ? "▶" : (index + 1).toString()
                        font.pixelSize: trackDelegate.isCurrent ? 16 : 13
                        opacity: trackDelegate.isCurrent ? 1.0 : 0.5
                        width: 24
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Image {
                        visible: (modelData.image || "").length > 0
                        source: modelData.image || ""
                        width: 44; height: 44
                        fillMode: Image.PreserveAspectCrop
                        smooth: true
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: modelData.title || ""
                            font.pixelSize: 16
                            font.weight: trackDelegate.isCurrent ? Font.DemiBold : Font.Normal
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                        Label {
                            visible: (modelData.artist || "").length > 0
                            text: modelData.artist || ""
                            font.pixelSize: 13
                            opacity: 0.7
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }
                }

                onClicked: triggerEvent("playlist.play", {
                    "playlistData": {
                        "uri":        modelData.uri        || "",
                        "title":      modelData.title      || "",
                        "artist":     modelData.artist     || "",
                        "image":      modelData.image      || "",
                        "media_type": modelData.media_type || ""
                    }
                })
            }
        }
    }
}
