import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string title: sessionData.title || ""
    property var    items: sessionData.items || []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            visible: title.length > 0
            text: title
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.fillWidth: true
        }

        GridView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            cellWidth: 160
            cellHeight: 180
            model: items

            delegate: Item {
                width: 160
                height: 180

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 6
                    spacing: 4

                    Image {
                        source: modelData.image || ""
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }

                    Label {
                        visible: (modelData.title || "").length > 0
                        text: modelData.title || ""
                        font.pixelSize: 13
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
