import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS 1.0 as OVOS

Button {
    id: controlButton
    Layout.preferredWidth: OVOS.Units.gridUnit * 5
    Layout.fillHeight: true
    Layout.margins: OVOS.Units.gridUnit * 0.1
    property alias buttonIcon: controlButtonContentIcon.source

    SequentialAnimation {
        id: controlButtonAnim

        PropertyAnimation {
            target: controlButtonBackground
            property: "color"
            Kirigami.Theme.colorSet: Kirigami.Theme.Button
            Kirigami.Theme.inherit: false
            to: Kirigami.Theme.highlightColor
            duration: 200
        }

        PropertyAnimation {
            target: controlButtonBackground
            property: "color"
            Kirigami.Theme.colorSet: Kirigami.Theme.Button
            Kirigami.Theme.inherit: false
            to: Kirigami.Theme.backgroundColor
            duration: 200
        }
    }

    onPressed: {
        controlButtonAnim.running = true;
    }

    contentItem: Item {
        Kirigami.Icon {
            id: controlButtonContentIcon
            width: Kirigami.Units.iconSizes.smallMedium
            height: width
            anchors.centerIn: parent

            ColorOverlay {
                source: parent
                anchors.fill: parent
                color: Kirigami.Theme.textColor
            }
        }
    }

    background: Rectangle {
        id: controlButtonBackground
        radius: 5
        Kirigami.Theme.colorSet: Kirigami.Theme.Button
        Kirigami.Theme.inherit: false
        color:  Kirigami.Theme.backgroundColor
        border.width: 1
        border.color: Kirigami.Theme.highlightColor
    }
}