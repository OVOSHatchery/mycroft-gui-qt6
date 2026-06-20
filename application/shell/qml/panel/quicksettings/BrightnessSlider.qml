/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
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
    property bool bSync: true

    iconSource: Qt.resolvedUrl("./brightness-increase.svg")

    slider.value: applicationSettings.fakeBrightness
    slider.from: 0
    slider.to: 1

    onChangeValueChanged: {
        applicationSettings.fakeBrightness = changeValue;
        console.log("Fake Brightness Value: " + changeValue)
        if (bSync){
            OVOS.GuiBusClient.sendRequest("phal.brightness.control.sync", {"brightness": changeValue * 100})
        } else {
            bSync = true
        }
    }

    Connections {
        target: OVOS.GuiBusClient
        onIntentRecevied: {
            if (type == "phal.brightness.control.get.response") {
                var roundingValue = data.brightness / 100;
                bSync = false
                slider.value = parseFloat(roundingValue).toFixed(1);
            }
            if (type == "phal.brightness.control.auto.dim.update") {
                var roundingDimValue = data.brightness / 100;
                bSync = false
                slider.value = parseFloat(roundingDimValue).toFixed(1);
            }
        }
    }
}
