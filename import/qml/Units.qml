/*
 * Copyright 2021 by Aditya Mehra <aix.m@outlook.com>
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
import QtQuick.Window
import org.kde.kirigami 2.14

pragma Singleton

QtObject {
    id: units

    // The fundamental unit of space that should be used for sizes
    property int gridUnit: 16
    property real devicePixelRatio: Math.max(1, ((fontMetrics.font.pixelSize*0.75) / fontMetrics.font.pointSize))
    property int largeSpacing: gridUnit / 2
    property int smallSpacing: largeSpacing / 2

    property variant fontMetrics: FontMetrics {
        function roundedIconSize(size) {
            if (size < 16) {
                return size;
            } else if (size < 22) {
                return 16;
            } else if (size < 32) {
                return 22;
            } else if (size < 48) {
                return 32;
            } else if (size < 64) {
                return 48;
            } else {
                return size;
            }
        }
    }

    property QtObject iconSizes: QtObject {
        property int small: fontMetrics.roundedIconSize(16 * devicePixelRatio)
        property int smallMedium: fontMetrics.roundedIconSize(22 * devicePixelRatio)
        property int medium: fontMetrics.roundedIconSize(32 * devicePixelRatio)
        property int large: fontMetrics.roundedIconSize(48 * devicePixelRatio)
        property int huge: fontMetrics.roundedIconSize(64 * devicePixelRatio)
        property int enormous: 128 * devicePixelRatio
    }
}
