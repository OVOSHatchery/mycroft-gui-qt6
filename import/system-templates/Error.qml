import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string label:  namespaceData ? (namespaceData.label  || "An error occurred") : "An error occurred"
    property string detail: namespaceData ? (namespaceData.detail || "") : ""

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.85, 500)
        spacing: 16

        Text {
            text: "⚠"
            font.pixelSize: 72
            color: "#f44336"
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: root.label
            font.pixelSize: 20
            font.weight: Font.DemiBold
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            visible: root.detail.length > 0
            text: root.detail
            font.pixelSize: 14
            opacity: 0.7
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
