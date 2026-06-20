import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property bool   success: namespaceData ? (namespaceData.success !== undefined ? namespaceData.success : true) : true
    property string label:   namespaceData ? (namespaceData.label   || "") : ""

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        Text {
            text:  root.success ? "✓" : "✗"
            font.pixelSize: 96
            color: root.success ? "#4caf50" : "#f44336"
            Layout.alignment: Qt.AlignHCenter

            SequentialAnimation on opacity {
                running: true
                NumberAnimation { from: 0; to: 1; duration: 300 }
            }
        }

        Label {
            visible: root.label.length > 0
            text: root.label
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.preferredWidth: 300
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
