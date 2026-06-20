import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string image:            namespaceData ? (namespaceData.image            || "") : ""
    property string title:            namespaceData ? (namespaceData.title            || "") : ""
    property string caption:          namespaceData ? (namespaceData.caption          || "") : ""
    property string fill:             namespaceData ? (namespaceData.fill             || "fit") : "fit"
    property string background_color: namespaceData ? (namespaceData.background_color || "transparent") : "transparent"

    Rectangle {
        anchors.fill: parent
        color: root.background_color

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            Label {
                visible: root.title.length > 0
                text: root.title
                font.pixelSize: 20
                font.weight: Font.DemiBold
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            AnimatedImage {
                Layout.fillWidth: true
                Layout.fillHeight: true
                source: root.image
                fillMode: {
                    if (root.fill === "crop")    return Image.PreserveAspectCrop
                    if (root.fill === "stretch") return Image.Stretch
                    return Image.PreserveAspectFit
                }
                playing: true
                smooth: true
            }

            Label {
                visible: root.caption.length > 0
                text: root.caption
                font.pixelSize: 14
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
