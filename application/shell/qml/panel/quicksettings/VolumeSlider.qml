/*
 * Copyright 2021 by Aditya Mehra <aix.m@outlook.com>
 * Copyright 2018 by Marco Martin <mart@kde.org>
 * Copyright 2018 David Edmundson <davidedmundson@kde.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import QtQuick
import OVOS.GUI 1.0 as OVOS

SliderBase {
    id: root
    property real changeValue: slider.value
    property bool muted: false
    property bool volSync: true
    iconSource: muted ? "qrc://icons/volume-mute" : "qrc://icons/volume-high"

    slider.from: 0
    slider.to: 100
    slider.stepSize: 10
    sliderButtonLabel: Math.round(slider.position * 100)

    onChangeValueChanged: {
        if (volSync){
            OVOS.GuiBusClient.sendRequest("mycroft.volume.set", {"percent": (changeValue / 100)});
        } else {
            volSync = true
        }
    }

    onIconClicked: {
        if(muted) {
            OVOS.GuiBusClient.sendRequest("mycroft.volume.unmute", {});
            muted = false
        } else {
            OVOS.GuiBusClient.sendRequest("mycroft.volume.mute", {});
            muted = true
        }
    }

    Component.onCompleted: {
        OVOS.GuiBusClient.sendRequest("mycroft.volume.get", {});
    }

    Connections {
        target: OVOS.GuiBusClient
        onSocketStatusChanged: {
            if (OVOS.GuiBusClient.status == OVOS.GuiBusClient.Open) {
                OVOS.GuiBusClient.sendRequest("mycroft.volume.get", {});
            }
        }
        onIntentRecevied: {
            if (type == "mycroft.volume.get.response") {
                volSync = false
                slider.value = Math.round(data.percent * 100);
            }
        }
    }
}
