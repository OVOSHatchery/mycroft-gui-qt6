import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var    current_temp: sessionData.current_temp !== undefined ? sessionData.current_temp : "--"
    property var    min_temp:     sessionData.min_temp     !== undefined ? sessionData.min_temp     : "--"
    property var    max_temp:     sessionData.max_temp     !== undefined ? sessionData.max_temp     : "--"
    property string condition:    sessionData.condition    || ""
    property string icon:         sessionData.icon         || ""
    property string location:     sessionData.location     || ""

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12

        Label {
            visible: location.length > 0
            text: location
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
        }

        Image {
            visible: icon.length > 0
            source: icon
            width: 128
            height: 128
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
        }

        Label {
            text: current_temp + "°"
            font.pixelSize: 64
            font.weight: Font.Light
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: condition
            font.pixelSize: 22
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 24

            Label {
                text: "↓ " + min_temp + "°"
                font.pixelSize: 18
                color: "#5b9bd5"
            }
            Label {
                text: "↑ " + max_temp + "°"
                font.pixelSize: 18
                color: "#e07070"
            }
        }
    }
}
