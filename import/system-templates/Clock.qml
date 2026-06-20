/*
 * Copyright 2018-2024 OpenVoiceOS
 * SPDX-License-Identifier: Apache-2.0
 *
 * Session data keys (ovos-skill-date-time):
 *   time_string     - "HH:MM" or "HH:MM:SS"
 *   weekday_string  - e.g. "MONDAY"
 *   daymonth_string - e.g. "14 MARCH"
 *   year_string     - e.g. "2024"
 *
 * Falls back to JS wall-clock when no session data is provided.
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string skillTimeString:  namespaceData ? (namespaceData.time_string     || "") : ""
    property string weekdayString:    namespaceData ? (namespaceData.weekday_string  || "") : ""
    property string dayMonthString:   namespaceData ? (namespaceData.daymonth_string || "") : ""
    property string yearString:       namespaceData ? (namespaceData.year_string     || "") : ""

    property string _hours:   ""
    property string _minutes: ""
    property string _dateStr: ""

    readonly property bool wideMode: width > height

    Timer {
        interval: 1000; repeat: true; running: true; triggeredOnStart: true
        onTriggered: {
            var now = new Date()
            var h = now.getHours(), m = now.getMinutes()
            root._hours   = h < 10 ? "0" + h : String(h)
            root._minutes = m < 10 ? "0" + m : String(m)
            root._dateStr = now.toLocaleDateString(Qt.locale(), Locale.LongFormat)
        }
    }

    readonly property string displayHours:    skillTimeString.length > 0 ? skillTimeString.split(":")[0] : _hours
    readonly property string displayMinutes:  skillTimeString.length > 0 ? skillTimeString.split(":")[1] : _minutes
    readonly property string displayWeekday:  weekdayString
    readonly property string displayDayMonth: dayMonthString
    readonly property string displayYear:     yearString
    readonly property string displayDate:     weekdayString.length > 0 ? weekdayString : _dateStr

    Item {
        anchors.fill: parent
        anchors.margins: wideMode ? parent.height * 0.08 : parent.width * 0.06

        // WIDE: hours | minutes side by side
        Row {
            visible: root.wideMode
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
            spacing: 0

            Label {
                width: parent.width * 0.47; height: parent.height
                text: root.displayHours
                font.pixelSize: height * 0.85; font.weight: Font.Bold
                horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter
                fontSizeMode: Text.VerticalFit; minimumPixelSize: 12
                color: palette.text
            }

            Label {
                width: parent.width * 0.06; height: parent.height
                text: ":"
                font.pixelSize: parent.height * 0.70; font.weight: Font.Bold
                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                fontSizeMode: Text.VerticalFit; minimumPixelSize: 12
                color: palette.highlight
                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.2; duration: 500 }
                    NumberAnimation { to: 1.0; duration: 500 }
                }
            }

            Label {
                width: parent.width * 0.47; height: parent.height
                text: root.displayMinutes
                font.pixelSize: height * 0.85; font.weight: Font.Bold
                horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter
                fontSizeMode: Text.VerticalFit; minimumPixelSize: 12
                color: palette.highlight
            }
        }

        // NARROW: stacked with date below
        ColumnLayout {
            visible: !root.wideMode
            anchors.centerIn: parent
            width: parent.width
            spacing: -parent.width * 0.04

            Label {
                text: root.displayHours
                font.pixelSize: parent.width * 0.70; font.weight: Font.Bold
                Layout.alignment: Qt.AlignHCenter; color: palette.text
                fontSizeMode: Text.HorizontalFit; minimumPixelSize: 12
            }
            Label {
                text: root.displayMinutes
                font.pixelSize: parent.width * 0.70; font.weight: Font.Bold
                Layout.alignment: Qt.AlignHCenter; color: palette.highlight
                fontSizeMode: Text.HorizontalFit; minimumPixelSize: 12
            }
            Label {
                visible: root.displayDate.length > 0
                text: root.displayDate
                font.pixelSize: parent.width * 0.08; font.capitalization: Font.AllUppercase
                Layout.alignment: Qt.AlignHCenter; opacity: 0.6
                fontSizeMode: Text.HorizontalFit; minimumPixelSize: 10
                Layout.topMargin: parent.width * 0.03
            }
        }
    }
}
