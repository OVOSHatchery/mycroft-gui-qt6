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

Item {
    id: connectionIcon
    property var window

    KDEConnect.DevicesModel {
        id: allDevicesModel
    }

    Repeater {
        model: allDevicesModel
        delegate: Item {
            property bool pairingRequest: device.hasPairingRequests
            
            onPairingRequestChanged: {
                if (pairingRequest) {
                    if(device.name.length > 0){
                        var component = Qt.createComponent("PairingArea.qml");
                        if (component.status != Component.Ready)
                        {
                            if (component.status == Component.Error) {
                                console.debug("Error: "+ component.errorString());
                            }
                            return;
                        } else {
                            window = component.createObject(popArea, {currentDevice: device})
                            window.open()
                        }

                    } else {
                        console.log("Unknown Request")
                    }

                } else {
                    console.log("pairing request timedout/closed")
                    window.close()
                }
            }
        }
    }

    Item {
        id: popArea
        anchors.fill: parent
    }
}
