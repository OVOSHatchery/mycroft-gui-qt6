/*
 * Copyright 2018-2024 OpenVoiceOS
 * SPDX-License-Identifier: Apache-2.0
 *
 * Session data keys (ovos-skill-alerts):
 *   alarmName      - string  (alarm label)
 *   alarmTime      - string  (time text, e.g. "07:30")
 *   alarmAmPm      - string  (e.g. "AM" or "PM"; empty for 24-hour)
 *   alarmExpired   - bool    (true = alarm is currently ringing)
 *   alarmRepeat    - bool    (true = repeating alarm)
 *   alarmRepeatStr - string  (e.g. "Every weekday")
 *   alarmIndex     - int     (index for cancel/snooze events)
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string alarmName:      namespaceData ? (namespaceData.alarmName      || "")    : ""
    property string alarmTime:      namespaceData ? (namespaceData.alarmTime      || "--:--"): "--:--"
    property string alarmAmPm:      namespaceData ? (namespaceData.alarmAmPm      || "")    : ""
    property bool   alarmExpired:   namespaceData ? (namespaceData.alarmExpired   === true)  : false
    property bool   alarmRepeat:    namespaceData ? (namespaceData.alarmRepeat    === true)  : false
    property string alarmRepeatStr: namespaceData ? (namespaceData.alarmRepeatStr || "")    : ""
    property int    alarmIndex:     namespaceData ? (namespaceData.alarmIndex     !== undefined ? namespaceData.alarmIndex : 0) : 0

    // Expired = ringing: flash a warning background tint
    background: Rectangle {
        color: root.alarmExpired ? "#b71c1c" : "#1a1a2e"
        Behavior on color { ColorAnimation { duration: 600 } }

        RadialGradient {
            anchors.fill: parent
            visible: !root.alarmExpired
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#2c2c44" }
                GradientStop { position: 0.5; color: "#1a1a2e" }
            }
        }

        SequentialAnimation on opacity {
            running: root.alarmExpired
            loops: Animation.Infinite
            NumberAnimation { to: 0.6; duration: 800 }
            NumberAnimation { to: 1.0; duration: 800 }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 12

        // Repeat info row
        RowLayout {
            visible: root.alarmRepeat || root.alarmRepeatStr.length > 0
            Layout.alignment: Qt.AlignHCenter
            spacing: 6

            Kirigami.Icon {
                source: "media-repeat-all"
                implicitWidth: 20; implicitHeight: 20
                color: palette.text
            }
            Label {
                text: root.alarmRepeatStr || "Repeating"
                font.pixelSize: 16; opacity: 0.75
            }
        }

        Item { Layout.fillHeight: true }

        // Alarm name
        Label {
            visible: root.alarmName.length > 0
            text: root.alarmName
            font.pixelSize: Math.min(root.width * 0.07, 32)
            font.weight: Font.Medium
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // Time display  (large, blinks when expired)
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 4

            Label {
                id: timeLabel
                text: root.alarmTime
                font.pixelSize: Math.min(root.width * 0.22, root.height * 0.28, 96)
                font.weight: Font.Bold
                font.family: "Monospace"
                color: root.alarmExpired ? "#ffcdd2" : palette.text

                SequentialAnimation on opacity {
                    running: root.alarmExpired
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.3; duration: 600 }
                    NumberAnimation { to: 1.0; duration: 600 }
                }
            }

            Label {
                visible: root.alarmAmPm.length > 0
                text: root.alarmAmPm
                font.pixelSize: Math.min(root.width * 0.07, 32)
                font.weight: Font.Light
                color: root.alarmExpired ? "#ffcdd2" : palette.text
                Layout.alignment: Qt.AlignBottom
                bottomPadding: 12
            }
        }

        Item { Layout.fillHeight: true }

        // Snooze / Dismiss buttons (only shown when expired)
        RowLayout {
            visible: root.alarmExpired
            Layout.fillWidth: true
            spacing: 12

            Button {
                text: "Snooze"
                Layout.fillWidth: true
                icon.name: "media-playback-pause"
                onClicked: OVOS.GuiBusClient.sendRequest(
                    "ovos.alerts.snooze_alarm",
                    {"alarmIndex": root.alarmIndex, "alarmName": root.alarmName})
            }

            Button {
                text: "Dismiss"
                Layout.fillWidth: true
                icon.name: "dialog-cancel"
                highlighted: true
                onClicked: OVOS.GuiBusClient.sendRequest(
                    "ovos.alerts.cancel_alarm",
                    {"alarmIndex": root.alarmIndex, "alarmName": root.alarmName})
            }
        }
    }
}
