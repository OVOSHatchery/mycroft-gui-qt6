import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string text:  namespaceData ? (namespaceData.text  || "") : ""
    property string title: namespaceData ? (namespaceData.title || "") : ""

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Label {
            visible: root.title.length > 0
            text: root.title
            font.pixelSize: 22
            font.weight: Font.DemiBold
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Label {
                width: parent.width
                text: root.text
                font.pixelSize: 16
                wrapMode: Text.WordWrap
            }
        }
    }
}
