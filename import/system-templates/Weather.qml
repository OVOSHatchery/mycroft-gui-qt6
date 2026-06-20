/*
 * Copyright 2018-2024 OpenVoiceOS
 * SPDX-License-Identifier: Apache-2.0
 *
 * Session data keys (ovos-skill-weather):
 *   weatherCode           - int  (0-17: even=day, odd=night; 8-13=rain, 14-15=snow)
 *   currentTemperature    - string  (also: current_temp for legacy)
 *   highTemperature       - string  (also: max_temp)
 *   lowTemperature        - string  (also: min_temp)
 *   windSpeed             - string
 *   humidity              - string
 *   chanceOfPrecipitation - string
 *   weatherLocation       - string  (also: location)
 *   condition             - string  (human-readable condition)
 *   icon                  - string  (URL, optional)
 *   sunrise               - string
 *   sunset                - string
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    // Support both ovos-skill-weather key names and legacy fallbacks
    property var    weatherCode:    namespaceData ? (namespaceData.weatherCode            !== undefined ? namespaceData.weatherCode            : -1)   : -1
    property string currentTemp:    namespaceData ? (namespaceData.currentTemperature     || namespaceData.current_temp || "--") : "--"
    property string highTemp:       namespaceData ? (namespaceData.highTemperature        || namespaceData.max_temp      || "--") : "--"
    property string lowTemp:        namespaceData ? (namespaceData.lowTemperature         || namespaceData.min_temp      || "--") : "--"
    property string windSpeed:      namespaceData ? (namespaceData.windSpeed              || "")   : ""
    property string humidity:       namespaceData ? (namespaceData.humidity               || "")   : ""
    property string precipitation:  namespaceData ? (namespaceData.chanceOfPrecipitation  || "")   : ""
    property string location:       namespaceData ? (namespaceData.weatherLocation        || namespaceData.location || "") : ""
    property string condition:      namespaceData ? (namespaceData.condition              || "")   : ""
    property string iconUrl:        namespaceData ? (namespaceData.icon                   || "")   : ""
    property string sunrise:        namespaceData ? (namespaceData.sunrise                || "")   : ""
    property string sunset:         namespaceData ? (namespaceData.sunset                 || "")   : ""

    // Day/night: even weatherCode = day, odd = night; fallback to wall clock
    readonly property bool isNight: weatherCode >= 0
                                    ? (weatherCode % 2 !== 0)
                                    : (new Date().getHours() < 6 || new Date().getHours() >= 20)

    // Precipitation type: weatherCode 8-13 = rain, 14-15 = snow
    readonly property bool isRainy: weatherCode >= 8 && weatherCode <= 13
    readonly property bool isSnowy: weatherCode >= 14 && weatherCode <= 15

    readonly property color textCol: isNight ? "#e8eaf6" : "#1a1a2e"

    fillWidth: true
    leftPadding: 0; rightPadding: 0; topPadding: 0; bottomPadding: 0

    // ── Background gradient ───────────────────────────────────────────────────
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: root.isNight ? "#0d0d2b" : "#42a5f5" }
            GradientStop { position: 1.0; color: root.isNight ? "#1a237e" : "#1565c0" }
        }

        // Rain particles
        Canvas {
            id: rainCanvas
            anchors.fill: parent
            visible: root.isRainy
            opacity: 0.45
            property var particles: []
            renderStrategy: Canvas.Threaded

            onPaint: {
                var ctx = getContext("2d")
                ctx.strokeStyle = "rgba(180,220,255,0.7)"
                ctx.lineWidth = 1.5; ctx.lineCap = "round"
                ctx.clearRect(0, 0, width, height)
                for (var i = 0; i < particles.length; i++) {
                    var p = particles[i]
                    ctx.beginPath(); ctx.moveTo(p.x, p.y)
                    ctx.lineTo(p.x - p.l * 0.3, p.y + p.l * p.ys * 0.3)
                    ctx.stroke()
                    p.x -= 0.5; p.y += p.ys
                    if (p.y > height || p.x < 0) { p.x = Math.random() * width + 20; p.y = -10 }
                }
            }

            Timer { interval: 20; repeat: true; running: rainCanvas.visible; onTriggered: rainCanvas.requestPaint() }

            Component.onCompleted: {
                var arr = []
                for (var i = 0; i < 50; i++)
                    arr.push({ x: Math.random() * width, y: Math.random() * height,
                               l: 10 + Math.random() * 8, ys: 8 + Math.random() * 8 })
                particles = arr
            }
        }

        // Snow particles
        Canvas {
            id: snowCanvas
            anchors.fill: parent
            visible: root.isSnowy
            opacity: 0.6
            property var particles: []
            renderStrategy: Canvas.Threaded

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                for (var i = 0; i < particles.length; i++) {
                    var p = particles[i]
                    ctx.beginPath(); ctx.arc(p.x, p.y, p.r, 0, 2 * Math.PI)
                    ctx.fillStyle = "rgba(255,255,255," + p.a + ")"; ctx.fill()
                    p.x += p.wx; p.y += p.ys
                    if (p.y > height || p.x < 0 || p.x > width) { p.x = Math.random() * width; p.y = -5 }
                }
            }

            Timer { interval: 25; repeat: true; running: snowCanvas.visible; onTriggered: snowCanvas.requestPaint() }

            Component.onCompleted: {
                var arr = []
                for (var i = 0; i < 80; i++)
                    arr.push({ x: Math.random() * width, y: Math.random() * height,
                               r: 2 + Math.random() * 4, ys: 0.5 + Math.random() * 1.5,
                               wx: (Math.random() - 0.5) * 0.8, a: 0.4 + Math.random() * 0.5 })
                particles = arr
            }
        }
    }

    // ── Content ───────────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 0

        Label {
            visible: root.location.length > 0
            text: root.location
            color: root.textCol; font.pixelSize: 18; font.weight: Font.Medium
            Layout.alignment: Qt.AlignHCenter; opacity: 0.85
        }

        Item { Layout.fillHeight: true; Layout.preferredHeight: 8 }

        Image {
            visible: root.iconUrl.length > 0
            source: root.iconUrl
            width: 96; height: 96; fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            visible: root.iconUrl.length === 0 && root.condition.length > 0
            text: root.condition; color: root.textCol
            font.pixelSize: 20; Layout.alignment: Qt.AlignHCenter; opacity: 0.9
        }

        // Hero temperature
        Label {
            text: root.currentTemp + "°"; color: root.textCol
            font.pixelSize: Math.min(root.width * 0.25, root.height * 0.30, 120)
            font.weight: Font.Light; Layout.alignment: Qt.AlignHCenter
        }

        Label {
            visible: root.iconUrl.length > 0 && root.condition.length > 0
            text: root.condition; color: root.textCol
            font.pixelSize: 18; Layout.alignment: Qt.AlignHCenter; opacity: 0.85
        }

        Item { Layout.fillHeight: true; Layout.preferredHeight: 4 }

        // High / Low
        RowLayout {
            Layout.alignment: Qt.AlignHCenter; spacing: 28
            Label {
                text: "↑ " + root.highTemp + "°"
                color: root.isNight ? "#ef9a9a" : "#b71c1c"
                font.pixelSize: 20; font.weight: Font.Medium
            }
            Label {
                text: "↓ " + root.lowTemp + "°"
                color: root.isNight ? "#90caf9" : "#0d47a1"
                font.pixelSize: 20; font.weight: Font.Medium
            }
        }

        Item { Layout.fillHeight: true; Layout.preferredHeight: 12 }

        // Wind · Humidity · Precipitation
        RowLayout {
            visible: root.windSpeed.length > 0 || root.humidity.length > 0 || root.precipitation.length > 0
            Layout.alignment: Qt.AlignHCenter; spacing: 0

            Repeater {
                model: [
                    { icon: "💨", value: root.windSpeed,     show: root.windSpeed.length > 0     },
                    { icon: "💧", value: root.humidity,      show: root.humidity.length > 0      },
                    { icon: "☔", value: root.precipitation, show: root.precipitation.length > 0 }
                ]
                RowLayout {
                    visible: modelData.show; spacing: 4
                    Label { text: modelData.icon; font.pixelSize: 16 }
                    Label { text: modelData.value; color: root.textCol; font.pixelSize: 15; opacity: 0.85 }
                    Rectangle {
                        visible: index < 2 && modelData.show
                        width: 1; height: 14; color: root.textCol; opacity: 0.3
                        Layout.leftMargin: 12; Layout.rightMargin: 12
                    }
                }
            }
        }

        // Sunrise / sunset
        RowLayout {
            visible: root.sunrise.length > 0 || root.sunset.length > 0
            Layout.alignment: Qt.AlignHCenter; spacing: 24
            Label {
                visible: root.sunrise.length > 0
                text: "🌅 " + root.sunrise; color: root.textCol; font.pixelSize: 14; opacity: 0.75
            }
            Label {
                visible: root.sunset.length > 0
                text: "🌇 " + root.sunset; color: root.textCol; font.pixelSize: 14; opacity: 0.75
            }
        }

        Item { Layout.fillHeight: true; Layout.preferredHeight: 8 }
    }
}
