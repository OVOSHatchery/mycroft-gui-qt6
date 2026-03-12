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
        anchors.top: parent.top
        anchors.topMargin: Mycroft.Units.gridUnit
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height / 2
        spacing: Mycroft.Units.gridUnit
        verticalMode: true
    }

    WeatherArea {
        id: weatherItemBox
        anchors.top: widgetsRow.bottom
        anchors.topMargin: -(Mycroft.Units.gridUnit + 8)
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: parent.height / 2
        verticalMode: true
    }
}
