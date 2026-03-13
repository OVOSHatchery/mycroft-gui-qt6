import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS 1.0 as OVOS

ColumnLayout {
    anchors.fill: parent
    spacing: 0

    HorizontalTopArea {
        Layout.fillWidth: true
        Layout.leftMargin: OVOS.Units.gridUnit
        Layout.rightMargin: OVOS.Units.gridUnit
        Layout.minimumHeight: parent.height * 0.17
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: Math.round(parent.height * 0.125)
    }

    TimeDisplay {
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 0.30
        Layout.leftMargin: OVOS.Units.gridUnit
        Layout.rightMargin: OVOS.Units.gridUnit
        Layout.topMargin: 1
        Layout.bottomMargin: 1
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: OVOS.Units.gridUnit
    }

    DayMonthDisplay {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: OVOS.Units.gridUnit * 2
        Layout.rightMargin: OVOS.Units.gridUnit * 2
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: OVOS.Units.gridUnit
    }

    BottomWidgetsArea {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: OVOS.Units.gridUnit * 2
        Layout.rightMargin: OVOS.Units.gridUnit * 2
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: OVOS.Units.gridUnit
    }
}
