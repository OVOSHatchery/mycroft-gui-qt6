import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string title:   sessionData.title   || ""
    property var    headers: sessionData.headers || []
    property var    rows:    sessionData.rows    || []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            visible: title.length > 0
            text: title
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 0
            Repeater {
                model: headers
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
            model: rows

            delegate: RowLayout {
                width: parent.width
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
