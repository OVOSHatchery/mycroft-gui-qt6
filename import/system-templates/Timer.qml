/*
 * Copyright 2018-2024 OpenVoiceOS
 * SPDX-License-Identifier: Apache-2.0
 *
 * Session data keys (ovos-skill-alerts / any timer skill):
 *   seconds   - int    (current seconds remaining or elapsed)
 *   label     - string (timer name / description)
 *   countdown - bool   (true = count down, false = count up; default true)
 *   duration  - int    (original duration in seconds, drives arc progress)
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Shapes
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property int    seconds:       namespaceData ? (namespaceData.seconds   !== undefined ? namespaceData.seconds   : 0)    : 0
    property string label:         namespaceData ? (namespaceData.label     || "")                                        : ""
    property bool   countdown:     namespaceData ? (namespaceData.countdown !== undefined ? namespaceData.countdown : true) : true
    property int    totalDuration: namespaceData ? (namespaceData.duration  !== undefined ? namespaceData.duration  : 0)    : 0

    property int displaySeconds: seconds
    property int _inferredTotal: 0

    onSecondsChanged: {
        displaySeconds = seconds
        if (_inferredTotal === 0 && seconds > 0 && totalDuration === 0)
            _inferredTotal = seconds
    }
    onTotalDurationChanged: { if (totalDuration > 0) _inferredTotal = totalDuration }

    readonly property int   effectiveTotal: totalDuration > 0 ? totalDuration : _inferredTotal
    readonly property real  arcProgress:   effectiveTotal > 0
                                           ? Math.max(0, Math.min(1, displaySeconds / effectiveTotal))
                                           : 0

    Timer {
        id: internalTimer
        interval: 1000; repeat: true
        running: root.countdown ? root.displaySeconds > 0 : true
        onTriggered: {
            if (root.countdown) { if (root.displaySeconds > 0) root.displaySeconds-- }
            else                { root.displaySeconds++ }
        }
    }

    function formatTime(s) {
        var h   = Math.floor(s / 3600)
        var m   = Math.floor((s % 3600) / 60)
        var sec = s % 60
        var parts = []
        if (h > 0) parts.push(h < 10 ? "0" + h : String(h))
        parts.push(m < 10 ? "0" + m : String(m))
        parts.push(sec < 10 ? "0" + sec : String(sec))
        return parts.join(":")
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item { Layout.fillHeight: true }

        Label {
            visible: root.label.length > 0
            text: root.label
            font.pixelSize: 22; font.weight: Font.Medium
            Layout.alignment: Qt.AlignHCenter; opacity: 0.8
        }

        Item { Layout.preferredHeight: root.label.length > 0 ? 12 : 0 }

        // Arc + time
        Item {
            id: arcArea
            readonly property int sz: Math.min(root.width * 0.55, root.height * 0.55, 240)
            Layout.preferredWidth: sz; Layout.preferredHeight: sz
            Layout.alignment: Qt.AlignHCenter

            // Track ring
            Shape {
                anchors.fill: parent
                ShapePath {
                    strokeWidth: 8; strokeColor: Qt.rgba(1, 1, 1, 0.15); fillColor: "transparent"
                    capStyle: ShapePath.RoundCap
                    PathAngleArc {
                        centerX: arcArea.sz / 2; centerY: arcArea.sz / 2
                        radiusX: arcArea.sz / 2 - 10; radiusY: arcArea.sz / 2 - 10
                        startAngle: -90; sweepAngle: 360
                    }
                }
            }

            // Progress arc
            Shape {
                anchors.fill: parent
                visible: root.arcProgress > 0
                ShapePath {
                    id: progressPath
                    strokeWidth: 8
                    strokeColor: root.displaySeconds <= 10 && root.countdown ? "#ef5350" : "#42a5f5"
                    fillColor: "transparent"
                    capStyle: ShapePath.RoundCap
                    Behavior on strokeColor { ColorAnimation { duration: 400 } }
                    PathAngleArc {
                        centerX: arcArea.sz / 2; centerY: arcArea.sz / 2
                        radiusX: arcArea.sz / 2 - 10; radiusY: arcArea.sz / 2 - 10
                        startAngle: -90
                        sweepAngle: root.arcProgress * 360
                        Behavior on sweepAngle { NumberAnimation { duration: 900; easing.type: Easing.InOutQuad } }
                    }
                }
            }

            // Centred time label
            Label {
                anchors.centerIn: parent
                text: root.formatTime(root.displaySeconds)
                font.pixelSize: arcArea.sz * 0.22
                font.weight: Font.Light; font.family: "Monospace"
                color: root.displaySeconds <= 10 && root.countdown ? "#ef5350" : palette.text
                Behavior on color { ColorAnimation { duration: 300 } }

                SequentialAnimation on opacity {
                    running: root.displaySeconds <= 5 && root.countdown && root.displaySeconds > 0
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.2; duration: 500 }
                    NumberAnimation { to: 1.0; duration: 500 }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
