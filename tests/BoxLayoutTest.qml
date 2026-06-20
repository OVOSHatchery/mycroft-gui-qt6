import QtQuick 2.5
import QtQuick.Controls 2.4 as Controls
import QtQuick.Layouts 1.3
import OVOS.GUI 1.0 as OVOS
import org.kde.kirigami 2.5 as Kirigami

OVOS.BoxLayout {
    id: root
    width: 400
    height: 600
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: "red"
    }
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: "Blue"
    }
}

