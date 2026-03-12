import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string prompt:  sessionData.prompt  || ""
    property var    options: sessionData.options || []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            visible: prompt.length > 0
            text: prompt
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: options
            spacing: 4

            delegate: ItemDelegate {
                width: parent.width
                text: modelData.label !== undefined ? modelData.label : String(modelData)
                font.pixelSize: 16
                onClicked: {
                    var value = modelData.value !== undefined ? modelData.value : modelData
                    triggerEvent("select.response", {"selected": value})
                }
            }
        }
    }
}
