/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: Apache-2.0
*/

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQml.Models 2.12
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kdeconnect 1.0 as KDEConnect

Controls.Popup {
    id: popRoot
    property QtObject currentDevice
    width: parent.width
    height: parent.height
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    background: Rectangle {
        color: Qt.rgba(0, 0, 0, 0.95)
    }

    Item {
        id: contentItem
        anchors.fill: parent

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

            ColumnLayout {
                id: pairingDialogLayout
                anchors.centerIn: parent

                Kirigami.Heading {
                    level: 3
                    text: "Pairing Request From " + currentDevice.name
                    color: "white"
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 3

                    Controls.Button {
                        id: acceptButton
                        Layout.fillWidth: true
                        Layout.minimumHeight: Kirigami.Units.gridUnit * 3
                        KeyNavigation.right: rejectButton
                        KeyNavigation.left: acceptButton

                        background: Rectangle {
                            color: acceptButton.activeFocus ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor
                        }

                        contentItem: Item {
                            RowLayout {
                                anchors.centerIn: parent
                                Kirigami.Icon {
                                    Layout.preferredWidth: Kirigami.Units.iconSizes.small
                                    Layout.preferredHeight: Kirigami.Units.iconSizes.small
                                    source: "dialog-ok"
                                }
                                Controls.Label {
                                    text: "Accept"
                                }
                            }
                        }

                        onClicked: {
                            currentDevice.acceptPairing()
                            popRoot.close()
                        }

                        Keys.onReturnPressed: {
                            clicked()
                        }

                    }

                    Controls.Button {
                        id: rejectButton
                        Layout.fillWidth: true
                        Layout.minimumHeight: Kirigami.Units.gridUnit * 3
                        KeyNavigation.right: rejectButton
                        KeyNavigation.left: acceptButton

                        background: Rectangle {
                            color: rejectButton.activeFocus ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor
                        }

                        contentItem: Item {
                            RowLayout {
                                anchors.centerIn: parent
                                Kirigami.Icon {
                                    Layout.preferredWidth: Kirigami.Units.iconSizes.small
                                    Layout.preferredHeight: Kirigami.Units.iconSizes.small
                                    source: "dialog-cancel"
                                }
                                Controls.Label {
                                    text: "Reject"
                                }
                            }
                        }

                        onClicked: {
                            currentDevice.rejectPairing()
                            popRoot.close()
                        }

                        Keys.onReturnPressed: {
                            clicked()
                        }
                    }
                }
            }
        }
    }
}
