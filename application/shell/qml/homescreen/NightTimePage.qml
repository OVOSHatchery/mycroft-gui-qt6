import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS 1.0 as OVOS

Control {
    id: nightTimeOverlayRoot
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    property bool horizontalMode: nightTimeOverlayRoot.width > nightTimeOverlayRoot.height ? 1 : 0
    property var time_string: sessionData.time_string ? sessionData.time_string.replace(":", "꞉") : ""

    background: Rectangle {
        width: idleRoot.width
        height: idleRoot.height
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -OVOS.Units.gridUnit * 2
        anchors.topMargin: -OVOS.Units.gridUnit * 2
        color: "#000000"
    }

    contentItem: Item {

        Label {
            anchors.fill: parent
            anchors.margins: OVOS.Units.gridUnit * 4
            font.capitalization: Font.AllUppercase
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment:Text.AlignVCenter
            font.weight: Font.ExtraBold
            fontSizeMode: Text.Fit
            minimumPixelSize: 20
            font.pixelSize: parent.height
            color: "#cdcdcd"
            text: nightTimeOverlayRoot.time_string
        }
    }
}

