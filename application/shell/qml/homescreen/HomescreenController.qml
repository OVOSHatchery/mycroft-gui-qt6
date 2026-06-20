/*
 * HomescreenController — data broker between the OVOS bus and the shell homescreen.
 *
 * All homescreen data (datetime, weather, notifications, apps, examples, widgets,
 * media) is now managed by the GUI adapter plugin on the Python side.  This QML
 * object subscribes to the bus events that the adapter emits and exposes the
 * data as plain properties so HomescreenLoader (and Homescreen.qml) can bind to them
 * without touching namespaceData or the NamespaceView namespace machinery.
 *
 * Bus events consumed (emitted by the GUI adapter's HomescreenManager):
 *   homescreen.data.time            { time_string, date_string, weekday_string, ... }
 *   homescreen.data.weather         { weather_code, weather_temp, weather_api_enabled }
 *   homescreen.data.notifications   { notification_counter, notification_model }
 *   homescreen.data.apps            { applications_model }
 *   homescreen.data.examples        { skill_examples, skill_info_enabled, skill_info_prefix }
 *   homescreen.data.wallpaper       { wallpaper_path, selected_wallpaper }
 *   homescreen.data.connectivity    { system_connectivity }
 *   homescreen.widget.timer         { count, ... }
 *   homescreen.widget.alarm         { count, ... }
 *   homescreen.widget.media         { enabled, widget, state }
 */

import QtQuick
import OVOS.GUI 1.0 as OVOS

QtObject {
    id: homescreenController

    // ---- datetime ----
    property string timeString:    "00:00"
    property string dateString:    ""
    property string weekdayString: ""
    property string dayString:     ""
    property string monthString:   ""
    property string yearString:    ""

    // ---- weather ----
    property bool   weatherEnabled: false
    property var    weatherCode:    0
    property var    weatherTemp:    ""

    // ---- wallpaper ----
    property string wallpaperPath: ""
    property string selectedWallpaper: ""

    // ---- notifications ----
    property var notificationCounter: 0
    property var notificationModel:   null

    // ---- apps drawer ----
    property var applicationsModel: []
    property bool appsEnabled: false

    // ---- example utterances ----
    property var  skillExamples:    []
    property bool examplesEnabled:  false
    property bool examplesPrefix:   false

    // ---- connectivity ----
    property string systemConnectivity: "offline"

    // ---- widgets ----
    property var timerWidgetData:  null
    property int timerWidgetCount: 0
    property var alarmWidgetData:  null
    property int alarmWidgetCount: 0
    property bool mediaWidgetEnabled: false
    property var  mediaWidgetData:    null
    property var  mediaWidgetState:   null

    // ---- signals forwarded to homescreen ----
    signal homescreenRequested()

    // Subscribe to bus events
    Connections {
        target: OVOS.GuiBusClient
        onIntentRecevied: {
            switch (type) {
            case "homescreen.data.time":
                homescreenController.timeString    = data.time_string    || "00:00"
                homescreenController.dateString    = data.date_string    || ""
                homescreenController.weekdayString = data.weekday_string || ""
                homescreenController.dayString     = data.day_string     || ""
                homescreenController.monthString   = data.month_string   || ""
                homescreenController.yearString    = data.year_string    || ""
                break

            case "homescreen.data.weather":
                homescreenController.weatherEnabled = Boolean(data.weather_api_enabled)
                homescreenController.weatherCode    = data.weather_code || 0
                homescreenController.weatherTemp    = data.weather_temp || ""
                break

            case "homescreen.data.wallpaper":
                homescreenController.wallpaperPath      = data.wallpaper_path      || ""
                homescreenController.selectedWallpaper  = data.selected_wallpaper  || ""
                break

            case "homescreen.data.notifications":
                homescreenController.notificationCounter = data.notification_counter || 0
                homescreenController.notificationModel   = data.notification_model   || null
                break

            case "homescreen.data.apps":
                homescreenController.applicationsModel = data.applications_model || []
                homescreenController.appsEnabled = (data.applications_model || []).length > 0
                break

            case "homescreen.data.examples":
                var exs = data.skill_examples ? data.skill_examples.examples || [] : []
                homescreenController.skillExamples   = exs
                homescreenController.examplesEnabled = Boolean(data.skill_info_enabled)
                homescreenController.examplesPrefix  = Boolean(data.skill_info_prefix)
                break

            case "homescreen.data.connectivity":
                homescreenController.systemConnectivity = data.system_connectivity || "offline"
                break

            case "homescreen.widget.timer":
                homescreenController.timerWidgetData  = data
                homescreenController.timerWidgetCount = data.count || 0
                break

            case "homescreen.widget.alarm":
                homescreenController.alarmWidgetData  = data
                homescreenController.alarmWidgetCount = data.count || 0
                break

            case "homescreen.widget.media":
                homescreenController.mediaWidgetEnabled = Boolean(data.enabled)
                homescreenController.mediaWidgetData    = data.widget  || null
                homescreenController.mediaWidgetState   = data.state   || null
                break

            case "mycroft.device.show.idle":
            case "ovos.homescreen.displayed":
                homescreenController.homescreenRequested()
                break

            default:
                break
            }
        }
    }
}
