import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import Mycroft 1.0 as Mycroft

ColumnLayout {
    anchors.fill: parent
    spacing: 0

    HorizontalTopArea {
        Layout.fillWidth: true
        Layout.leftMargin: Mycroft.Units.gridUnit
        Layout.rightMargin: Mycroft.Units.gridUnit
        Layout.minimumHeight: parent.height * 0.17
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: Math.round(parent.height * 0.125)
    }

    TimeDisplay {
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 0.30
        Layout.leftMargin: Mycroft.Units.gridUnit
        Layout.rightMargin: Mycroft.Units.gridUnit
        Layout.topMargin: 1
        Layout.bottomMargin: 1
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: Mycroft.Units.gridUnit
    }

    DayMonthDisplay {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: Mycroft.Units.gridUnit * 2
        Layout.rightMargin: Mycroft.Units.gridUnit * 2
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: Mycroft.Units.gridUnit
    }

    BottomWidgetsArea {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: Mycroft.Units.gridUnit * 2
        Layout.rightMargin: Mycroft.Units.gridUnit * 2
    }

    Item {
        Layout.fillWidth: true
        Layout.minimumHeight: Mycroft.Units.gridUnit
    }
}
