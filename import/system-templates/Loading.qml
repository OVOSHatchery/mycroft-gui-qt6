import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string label: sessionData.label || ""

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        BusyIndicator {
            running: true
            width: 80; height: 80
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            visible: label.length > 0
            text: label
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
