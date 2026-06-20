import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string label: namespaceData ? (namespaceData.label || "") : ""

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        BusyIndicator {
            running: true
            width: 80; height: 80
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            visible: root.label.length > 0
            text: root.label
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
