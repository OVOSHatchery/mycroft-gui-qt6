import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS 1.0 as OVOS

Rectangle {
    id: delegate
    color: "#212121"
    radius: 15
    readonly property ListView listView: ListView.view
    width: listView.width
    height: notificationRowBoxLayout.implicitHeight + (Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing)
    
    RowLayout {
        id: notificationRowBoxLayout
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing

        Column {
            id: notificationColumnBoxLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Kirigami.Units.largeSpacing
            
            Label {
                id: notificationHeading
                text: modelData.sender
                width: parent.width
                elide: Text.ElideRight
                font.weight: Font.ExtraBold
                font.pixelSize: parent.width * 0.035
                color: "#ffffff"
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
                        if (modelData.action != "") {
                            OVOS.OVOSController.sendRequest(modelData.action, modelData.callback_data)
                        }
                        OVOS.OVOSController.sendRequest("ovos.notification.api.storage.clear.item", {"notification": modelData})
                    }
                }
            }

            Kirigami.Separator {
                width: parent.width
                height: Kirigami.Units.smallSpacing * 0.15
                color: "#8F8F8F"
            }

            Label {
                id: notificationContent
                text: modelData.text
                width: parent.width
                wrapMode: Text.WordWrap
                font.pixelSize: parent.width * 0.0375
                maximumLineCount: 2
                elide: Text.ElideRight
                color: "#ffffff"
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
                        if (modelData.action != "") {
                            OVOS.OVOSController.sendRequest(modelData.action, modelData.callback_data)
                        }
                        OVOS.OVOSController.sendRequest("ovos.notification.api.storage.clear.item", {"notification": modelData})
                    }
                }
            }
        }
        
        Kirigami.Separator {
            Layout.preferredWidth: Kirigami.Units.smallSpacing * 0.25
            Layout.fillHeight: true
            color: "#8F8F8F"
        }
        
        Item {
            Layout.minimumWidth: parent.width * 0.15
            Layout.fillHeight: true

            AbstractButton {
                width: parent.width - Kirigami.Units.largeSpacing * 2
                height: width
                anchors.centerIn: parent

                background: Rectangle {
                    color: "transparent"
                }

                contentItem: Kirigami.Icon {
                    anchors.centerIn: parent
                    width: Kirigami.Units.iconSizes.small
                    height: width
                    source: Qt.resolvedUrl("icons/delete.svg")
                }

                onClicked: {
                    OVOS.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
                    if (modelData.action != "") {
                            OVOS.OVOSController.sendRequest(modelData.action, modelData.callback_data)
                    }
                    OVOS.OVOSController.sendRequest("ovos.notification.api.storage.clear.item", {"notification": modelData})
                }
            }
        }
    }
} 
