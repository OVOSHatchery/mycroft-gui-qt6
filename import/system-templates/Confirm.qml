import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string question:    namespaceData ? (namespaceData.question    || "") : ""
    property string confirm_yes: namespaceData ? (namespaceData.confirm_yes || "Yes") : "Yes"
    property string confirm_no:  namespaceData ? (namespaceData.confirm_no  || "No") : "No"

    function respond(confirmed) {
        root.triggerGuiEvent("confirm.response", {"confirmed": confirmed})
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.8, 400)
        spacing: 24

        Label {
            text: root.question
            font.pixelSize: 22
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 24

            Button {
                text: root.confirm_no
                font.pixelSize: 16
                onClicked: root.respond(false)
            }

            Button {
                text: root.confirm_yes
                font.pixelSize: 16
                onClicked: root.respond(true)
            }
        }
    }
}
