import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string title:   namespaceData ? (namespaceData.title   || "") : ""
    property var    headers: namespaceData ? (namespaceData.headers || []) : []
    property var    rows:    namespaceData ? (namespaceData.rows    || []) : []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            visible: root.title.length > 0
            text: root.title
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 0
            Repeater {
                model: root.headers
                delegate: Label {
                    text: modelData
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    padding: 8
                    Layout.fillWidth: true
                    background: Rectangle { color: "#e0e0e0" }
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.rows

            delegate: RowLayout {
                width: ListView.view.width
                spacing: 0
                Repeater {
                    model: modelData
                    delegate: Label {
                        text: modelData !== undefined ? String(modelData) : ""
                        font.pixelSize: 13
                        padding: 6
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
