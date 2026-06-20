import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string prompt:  namespaceData ? (namespaceData.prompt  || "") : ""
    property var    options: namespaceData ? (namespaceData.options || []) : []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            visible: root.prompt.length > 0
            text: root.prompt
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.options
            spacing: 4

            delegate: ItemDelegate {
                width: ListView.view.width
                text: modelData.label !== undefined ? modelData.label : String(modelData)
                font.pixelSize: 16
                onClicked: {
                    var value = modelData.value !== undefined ? modelData.value : modelData
                    root.triggerGuiEvent("select.response", {"selected": value})
                }
            }
        }
    }
}
