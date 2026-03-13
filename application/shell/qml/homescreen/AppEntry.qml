import QtQuick
import QtQuick.Controls
import QtQuick.Window
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import OVOS 1.0 as OVOS

Control {
    id: appEntryDelegate
    leftInset: 1
    rightInset: 1
    topInset: 1
    bottomInset: 1
    padding: 12

    background: Rectangle {
        id: delBackground
        color: Qt.darker(Kirigami.Theme.backgroundColor, 2)
        radius: 6
    }

    contentItem: Column {

        Item {
            width: parent.width
            height: parent.height / 2

            Image {
                width: parent.width < parent.height ? parent.width : parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                height: width
                source: model.thumbnail
            }
        }

        Label {
            width: parent.width
            height: parent.height / 2
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Kirigami.Theme.textColor
            text: model.name
            wrapMode: Text.Wrap
            font.capitalization: Font.Capitalize
            font.pixelSize: 22
            elide: Text.ElideRight
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            delBackground.color = Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.8)
            delBackground.color = Qt.darker(Kirigami.Theme.backgroundColor, 2)
            appBarRoot.close()
            OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
            OVOS.OVOSController.sendRequest(model.action, {})
        }
    }
}
