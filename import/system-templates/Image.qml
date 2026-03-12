import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string image:            sessionData.image            || ""
    property string title:            sessionData.title            || ""
    property string caption:          sessionData.caption          || ""
    property string fill:             sessionData.fill             || "fit"
    property string background_color: sessionData.background_color || "transparent"

    Rectangle {
        anchors.fill: parent
        color: background_color

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            Label {
                visible: title.length > 0
                text: title
                font.pixelSize: 20
                font.weight: Font.DemiBold
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Image {
                Layout.fillWidth: true
                Layout.fillHeight: true
                source: image
                fillMode: {
                    if (fill === "crop")    return Image.PreserveAspectCrop
                    if (fill === "stretch") return Image.Stretch
                    return Image.PreserveAspectFit
                }
                smooth: true
            }

            Label {
                visible: caption.length > 0
                text: caption
                font.pixelSize: 14
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
