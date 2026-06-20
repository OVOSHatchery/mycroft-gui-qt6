import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string title: namespaceData ? (namespaceData.title || "") : ""
    property var    items: namespaceData ? (namespaceData.items || []) : []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            visible: root.title.length > 0
            text: root.title
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.fillWidth: true
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.items

            delegate: ItemDelegate {
                width: ListView.view.width
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
