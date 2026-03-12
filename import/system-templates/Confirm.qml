import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string question:    sessionData.question    || ""
    property string confirm_yes: sessionData.confirm_yes || "Yes"
    property string confirm_no:  sessionData.confirm_no  || "No"

    function respond(confirmed) {
        triggerEvent("confirm.response", {"confirmed": confirmed})
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.8, 400)
        spacing: 24

        Label {
            text: question
            font.pixelSize: 22
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 24

            Button {
                text: confirm_no
                font.pixelSize: 16
                onClicked: respond(false)
            }

            Button {
                text: confirm_yes
                font.pixelSize: 16
                onClicked: respond(true)
            }
        }
    }
}
