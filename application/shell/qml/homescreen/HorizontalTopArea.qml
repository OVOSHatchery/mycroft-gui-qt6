import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import Mycroft 1.0 as Mycroft

Rectangle {
    color: "transparent"

    WidgetsArea {
        id: widgetsRow
        anchors.left: parent.left
        anchors.right: weatherItemBox.left
        height: parent.height
        spacing: Mycroft.Units.gridUnit
    }

    WeatherArea {
        id: weatherItemBox
        anchors.right: parent.right
        anchors.rightMargin: Mycroft.Units.gridUnit * 0.50
        width: parent.width * 0.30
        height: parent.height
    }
}
