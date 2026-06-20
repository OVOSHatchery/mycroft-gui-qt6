import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS.GUI 1.0 as OVOS

Rectangle {
    color: "transparent"

    WidgetsArea {
        id: widgetsRow
        anchors.left: parent.left
        anchors.right: weatherItemBox.left
        height: parent.height
        spacing: OVOS.Units.gridUnit
    }

    WeatherArea {
        id: weatherItemBox
        anchors.right: parent.right
        anchors.rightMargin: OVOS.Units.gridUnit * 0.50
        width: parent.width * 0.30
        height: parent.height
    }
}
