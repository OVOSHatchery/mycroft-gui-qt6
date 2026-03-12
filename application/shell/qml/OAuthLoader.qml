import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import Mycroft 1.0 as Mycroft
import QtWebEngine 1.9

Popup {
    id: oAuthPopup
    width: parent.width * 0.8
    height: parent.height * 0.8
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    property string url: ""
    property var appID
    property var skillID 
    property bool needsCredentials: false

    function forwardAuthentication(redirectURL) {
        Mycroft.MycroftController.sendRequest("ovos.shell.oauth.authentication.forward", {
            "redirectURL": redirectURL,
            "app_id": oAuthPopup.appID,
            "skill_id": oAuthPopup.skillID
        })
    }

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent
    }

     contentItem: Item {

        Item {
            width: rotation === 90 || rotation == -90 ? parent.height : parent.width
            height: rotation === 90 || rotation == -90 ? parent.width : parent.height
            anchors.centerIn: parent

            rotation: {
                switch (applicationSettings.rotation) {
                    case "CW":
                        return 90;
                    case "CCW":
                        return -90;
                    case "UD":
                        return 180;
                    case "NORMAL":
                    default:
                        return 0;
                }
            }

            Rectangle {
                anchors.fill: parent
                color: Kirigami.Theme.backgroundColor
                visible: oAuthPopup.opened && oAuthPopup.needsCredentials ? 1 : 0
                enabled: oAuthPopup.opened && oAuthPopup.needsCredentials ? 1 : 0
                
                ColumnLayout {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 16

                    Text {
                        id: oAuthClientIDText
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: qsTr("Client ID")
                        color: Kirigami.Theme.textColor
                    }

                    TextField {
                        id: oAuthClientID
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: qsTr("Enter Client ID")
                        color: Kirigami.Theme.textColor
                    }

                    Text {
                        id: oAuthClientSecretText
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: qsTr("Client Secret")
                        color: Kirigami.Theme.textColor
                    }

                    TextField {
                        id: oAuthClientSecret
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: qsTr("Enter Client Secret")
                        color: Kirigami.Theme.textColor
                    }

                    Button {
                        id: oAuthSubmit
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: qsTr("Submit")
                        
                        onClicked: {
                            Mycroft.MycroftController.sendRequest("ovos.shell.oauth.register.credentials", {
                                "app_id": oAuthPopup.appID,
                                "skill_id": oAuthPopup.skillID,
                                "client_id": oAuthClientID.text,
                                "client_secret": oAuthClientSecret.text
                            })
                            oAuthPopup.close()
                        }                     
                    }
                }
            }

            WebEngineView {
                id: engineView
                anchors.fill: parent
                enabled: oAuthPopup.opened && !oAuthPopup.needsCredentials ? 1 : 0
                visible: oAuthPopup.opened && !oAuthPopup.needsCredentials ? 1 : 0
                url: oAuthPopup.url

                onUrlChanged: {
                    url_to_check = engineView.url
                    if (url_to_check.indexOf("code") > -1 && url_to_check.indexOf("state") > -1) {
                        oAuthPopup.forwardAuthentication(url_to_check)
                        oAuthPopup.close()
                    }
                }
            }
        }
    }
}