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

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: items

            delegate: ItemDelegate {
                width: parent.width
                contentItem: ColumnLayout {
                    spacing: 2
                    Label {
                        text: modelData.title || ""
                        font.pixelSize: 16
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                    Label {
                        visible: (modelData.subtitle || "").length > 0
                        text: modelData.subtitle || ""
                        font.pixelSize: 13
                        opacity: 0.7
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}
