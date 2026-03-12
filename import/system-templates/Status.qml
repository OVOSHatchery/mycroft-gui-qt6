import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property bool   success: sessionData.success !== undefined ? sessionData.success : true
    property string label:   sessionData.label   || ""

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        Text {
            text:  success ? "✓" : "✗"
            font.pixelSize: 96
            color: success ? "#4caf50" : "#f44336"
            Layout.alignment: Qt.AlignHCenter

            SequentialAnimation on opacity {
                running: true
                NumberAnimation { from: 0; to: 1; duration: 300 }
            }
        }

        Label {
            visible: label.length > 0
            text: label
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.preferredWidth: 300
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
