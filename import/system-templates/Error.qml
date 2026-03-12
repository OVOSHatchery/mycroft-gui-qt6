import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string label:  sessionData.label  || "An error occurred"
    property string detail: sessionData.detail || ""

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
            text: label
            font.pixelSize: 20
            font.weight: Font.DemiBold
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            visible: detail.length > 0
            text: detail
            font.pixelSize: 14
            opacity: 0.7
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
