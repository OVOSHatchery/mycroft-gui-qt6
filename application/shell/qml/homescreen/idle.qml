import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import Mycroft 1.0 as Mycroft
import "." as Local

Item {
    id: idleRoot

    // -----------------------------------------------------------------------
    // HomescreenController — data source for all homescreen properties.
    // Subscribes to homescreen.data.* and homescreen.widget.* bus events
    // emitted by the HomescreenManager in the GUI adapter plugin.
    // -----------------------------------------------------------------------
    HomescreenController {
        id: homescreenController
        onHomescreenRequested: {
            // Return to the main page whenever the homescreen is re-activated
            mainView.currentIndex = 1
            appsDrawer.close()
        }
    }

    // -----------------------------------------------------------------------
    // sessionData shim — sub-components (DayMonthDisplay, WeatherArea, etc.)
    // reference sessionData.xxx via QML scope lookup.  This QtObject makes
    // all the same keys available without changing those files.
    // -----------------------------------------------------------------------
    property QtObject sessionData: QtObject {
        property string time_string:         homescreenController.timeString
        property string date_string:         homescreenController.dateString
        property string weekday_string:      homescreenController.weekdayString
        property string day_string:          homescreenController.dayString
        property string month_string:        homescreenController.monthString
        property string year_string:         homescreenController.yearString
        property var    weather_code:        homescreenController.weatherCode
        property var    weather_temp:        homescreenController.weatherTemp
        property bool   weather_api_enabled: homescreenController.weatherEnabled
        property string wallpaper_path:      homescreenController.wallpaperPath
        property string selected_wallpaper:  homescreenController.selectedWallpaper
        property var    notification_model:  homescreenController.notificationModel
        property var    applications_model:  homescreenController.applicationsModel
        property var    skill_examples:      homescreenController.skillExamples
        property bool   skill_info_enabled:  homescreenController.examplesEnabled
        property bool   skill_info_prefix:   homescreenController.examplesPrefix
        property string system_connectivity: homescreenController.systemConnectivity
        // persistent_menu_hint is shell-local; not driven by the adapter
        property bool   persistent_menu_hint: false
    }

    // -----------------------------------------------------------------------
    // Public properties used by sub-components via idleRoot.xxx
    // -----------------------------------------------------------------------
    property bool  horizontalMode:     width > height
    readonly property color primaryBorderColor:   Qt.rgba(1, 0, 0, 0.9)
    readonly property color secondaryBorderColor: Qt.rgba(1, 1, 1, 0.7)
    property color shadowColor:        Qt.rgba(0, 0, 0, 0.7)
    property bool  rtlMode:            false
    property bool  appsEnabled:        homescreenController.appsEnabled
    property bool  examplesEnabled:    homescreenController.examplesEnabled
    property bool  examplesPrefix:     homescreenController.examplesPrefix
    property bool  weatherEnabled:     homescreenController.weatherEnabled
    property string systemConnectivity: homescreenController.systemConnectivity
    property var   timeString:         homescreenController.timeString

    // Widget properties — directly bound from HomescreenController
    property var  timerWidgetData:    homescreenController.timerWidgetData
    property int  timerWidgetCount:   homescreenController.timerWidgetCount
    property var  alarmWidgetData:    homescreenController.alarmWidgetData
    property int  alarmWidgetCount:   homescreenController.alarmWidgetCount
    property bool mediaWidgetEnabled: homescreenController.mediaWidgetEnabled
    property var  mediaWidgetData:    homescreenController.mediaWidgetData
    property var  mediaWidgetState:   homescreenController.mediaWidgetState

    // Notification model — watched for storage view updates
    property var notificationModel: homescreenController.notificationModel

    // Examples cycle
    property var    textModel:    homescreenController.skillExamples
                                  ? (homescreenController.skillExamples.examples || [])
                                  : []
    property string exampleEntry: ""

    // controlBarItem — alias that MainPage.qml references to open/close the apps drawer
    property alias controlBarItem: appsDrawer

    signal exampleEntryUpdate(string exampleEntry)

    // -----------------------------------------------------------------------
    // triggerGuiEvent() — compat shim called by MainPage.qml via scope lookup.
    // Translates the one homescreen GUI event into the matching bus message.
    // -----------------------------------------------------------------------
    function triggerGuiEvent(eventName, data) {
        if (eventName === "homescreen.swipe.change.wallpaper") {
            Mycroft.MycroftController.sendRequest("ovos.wallpaper.manager.change.wallpaper", {})
        } else {
            Mycroft.MycroftController.sendRequest(eventName, data || {})
        }
    }

    // -----------------------------------------------------------------------
    // getWeatherImagery() — called by WeatherArea.qml via scope lookup
    // -----------------------------------------------------------------------
    function getWeatherImagery(weathercode) {
        switch (weathercode) {
        case 0: return "icons/sun.svg"
        case 1: return "icons/partial_clouds.svg"
        case 2: return "icons/clouds.svg"
        case 3: return "icons/rain.svg"
        case 4: return "icons/rain.svg"
        case 5: return "icons/storm.svg"
        case 6: return "icons/snow.svg"
        case 7: return "icons/fog.svg"
        }
        return ""
    }

    // -----------------------------------------------------------------------
    // Notification model update → sync storage view
    // -----------------------------------------------------------------------
    onNotificationModelChanged: {
        if (notificationModel) {
            notificationsStorageView.model = notificationModel.storedmodel
            notificationsStorageView.forceLayout()
            if (notificationModel.count <= 0 && notificationsStorageViewBox.opened) {
                notificationsStorageViewBox.close()
            }
        }
    }

    onTextModelChanged: {
        if (!idleRoot.examplesEnabled) {
            return
        }
        exampleEntry = idleRoot.textModel[0] ? idleRoot.textModel[0] : ""
        exampleEntryUpdate(exampleEntry)
        textTimer.running = true
    }

    onVisibleChanged: {
        if (visible && idleRoot.textModel && idleRoot.examplesEnabled) {
            textTimer.running = true
        }
    }

    // -----------------------------------------------------------------------
    // Bus events not related to homescreen data
    // -----------------------------------------------------------------------
    Connections {
        target: Mycroft.MycroftController
        onIntentRecevied: {
            if (type == "phal.brightness.control.auto.night.mode.enabled") {
                mainView.currentIndex = 0
            } else if (type == "ovos.homescreen.main_view.current_index.set") {
                mainView.currentIndex = data.current_index
                Mycroft.MycroftController.sendRequest(
                    "ovos.homescreen.main_view.current_index.get.response",
                    {"current_index": mainView.currentIndex})
            } else if (type == "ovos.homescreen.main_view.current_index.get") {
                Mycroft.MycroftController.sendRequest(
                    "ovos.homescreen.main_view.current_index.get.response",
                    {"current_index": mainView.currentIndex})
            }
        }
    }

    // -----------------------------------------------------------------------
    // Background wallpaper
    // -----------------------------------------------------------------------
    Image {
        anchors.fill: parent
        source: homescreenController.wallpaperPath && homescreenController.selectedWallpaper
            ? Qt.resolvedUrl(homescreenController.wallpaperPath + homescreenController.selectedWallpaper)
            : Qt.resolvedUrl("https://raw.githubusercontent.com/OpenVoiceOS/ovos-PHAL-plugin-wallpaper-manager/refs/heads/dev/ovos_PHAL_plugin_wallpaper_manager/wallpapers/default.jpg")
        fillMode: Image.PreserveAspectCrop
    }

    // -----------------------------------------------------------------------
    // Background gradient overlay (same as original)
    // -----------------------------------------------------------------------
    Item {
        anchors.fill: parent

        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(0, parent.height + 20)
            gradient: Gradient {
                GradientStop { position: 0;    color: Qt.rgba(0, 0, 0, 0) }
                GradientStop { position: 0.75; color: Qt.rgba(0, 0, 0, 0.1) }
                GradientStop { position: 1.0;  color: Qt.rgba(0, 0, 0, 0.3) }
            }
        }

        RadialGradient {
            anchors.fill: parent
            angle: 0
            verticalRadius: parent.height * 0.625
            horizontalRadius: parent.width * 0.5313
            gradient: Gradient {
                GradientStop { position: 0.0;  color: Qt.rgba(0, 0, 0, 0) }
                GradientStop { position: 0.6;  color: Qt.rgba(0, 0, 0, 0.2) }
                GradientStop { position: 0.75; color: Qt.rgba(0, 0, 0, 0.3) }
                GradientStop { position: 1.0;  color: Qt.rgba(0, 0, 0, 0.5) }
            }
        }
    }

    // -----------------------------------------------------------------------
    // Right-edge swipe handle (navigate forward in mainView)
    // -----------------------------------------------------------------------
    Item {
        width: Mycroft.Units.gridUnit * 4
        height: Mycroft.Units.gridUnit * 12
        anchors.right: parent.right
        anchors.rightMargin: -Mycroft.Units.gridUnit * 2
        anchors.verticalCenter: parent.verticalCenter
        visible: mainView.currentIndex == 0
        enabled: mainView.currentIndex == 0
        z: 2

        Rectangle {
            width: Mycroft.Units.gridUnit * 0.5
            height: horizontalMode ? Mycroft.Units.gridUnit * 3.5 : Mycroft.Units.gridUnit * 2.5
            anchors.right: parent.right
            anchors.rightMargin: Mycroft.Units.gridUnit * 0.5
            anchors.verticalCenter: parent.verticalCenter
            color: Qt.rgba(0.5, 0.5, 0.5, 0.5)
            radius: Mycroft.Units.gridUnit
            z: 2
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                appsDrawer.close()
                Mycroft.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
                if (mainView.currentIndex == 0) {
                    mainView.currentIndex = 1
                } else if (mainView.currentIndex == 1) {
                    mainView.currentIndex = 2
                }
                Mycroft.MycroftController.sendRequest(
                    "ovos.homescreen.main_view.current_index.get.response",
                    {"current_index": mainView.currentIndex})
            }
        }
    }

    // -----------------------------------------------------------------------
    // Bottom-edge apps drawer handle
    // -----------------------------------------------------------------------
    Item {
        width: Mycroft.Units.gridUnit * 12
        height: Mycroft.Units.gridUnit * 4
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -Mycroft.Units.gridUnit * 2
        anchors.horizontalCenter: parent.horizontalCenter
        visible: mainView.currentIndex == 1 && idleRoot.appsEnabled
        enabled: mainView.currentIndex == 1
        z: 2

        Rectangle {
            width: horizontalMode ? Mycroft.Units.gridUnit * 3.5 : Mycroft.Units.gridUnit * 2.5
            height: Mycroft.Units.gridUnit * 0.5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Mycroft.Units.gridUnit * 0.5
            anchors.horizontalCenter: parent.horizontalCenter
            color: Qt.rgba(0.5, 0.5, 0.5, 0.5)
            radius: Mycroft.Units.gridUnit
            visible: !appsDrawer.visible
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                Mycroft.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
                appsDrawer.open()
            }
        }
    }

    // -----------------------------------------------------------------------
    // Left-edge swipe handle (navigate backward in mainView)
    // -----------------------------------------------------------------------
    Item {
        width: Mycroft.Units.gridUnit * 4
        height: Mycroft.Units.gridUnit * 12
        anchors.left: parent.left
        anchors.leftMargin: -Mycroft.Units.gridUnit * 2
        anchors.verticalCenter: parent.verticalCenter
        visible: mainView.currentIndex == 1 || mainView.currentIndex == 2
        enabled: mainView.currentIndex == 1 || mainView.currentIndex == 2
        z: 2

        Rectangle {
            width: Mycroft.Units.gridUnit * 0.5
            height: horizontalMode ? Mycroft.Units.gridUnit * 3.5 : Mycroft.Units.gridUnit * 2.5
            anchors.left: parent.left
            anchors.leftMargin: Mycroft.Units.gridUnit * 0.5
            anchors.verticalCenter: parent.verticalCenter
            color: Qt.rgba(0.5, 0.5, 0.5, 0.5)
            radius: Mycroft.Units.gridUnit
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                appsDrawer.close()
                Mycroft.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
                if (mainView.currentIndex == 1) {
                    mainView.currentIndex = 0
                } else if (mainView.currentIndex == 2) {
                    mainView.currentIndex = 1
                }
                Mycroft.MycroftController.sendRequest(
                    "ovos.homescreen.main_view.current_index.get.response",
                    {"current_index": mainView.currentIndex})
            }
        }
    }

    // -----------------------------------------------------------------------
    // Main content — NightTimePage / MainPage switcher
    // -----------------------------------------------------------------------
    StackLayout {
        id: mainView
        currentIndex: 1
        anchors.fill: parent

        NightTimePage {
            id: nightTimeView
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        MainPage {
            id: mainPageView
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    // -----------------------------------------------------------------------
    // Apps drawer (replaces CardDelegate controlBar)
    // -----------------------------------------------------------------------
    Drawer {
        id: appsDrawer
        edge: Qt.BottomEdge
        width: parent.width
        height: appsBarContent.implicitHeight + Kirigami.Units.largeSpacing * 2

        background: Rectangle {
            color: Qt.rgba(0, 0, 0, 0.85)
            radius: Kirigami.Units.largeSpacing
        }

        Local.AppsBar {
            id: appsBarContent
            width: parent.width
            visible: idleRoot.appsEnabled
            parentItem: idleRoot
            appsModel: homescreenController.applicationsModel
        }
    }

    // -----------------------------------------------------------------------
    // Notifications storage popup
    // -----------------------------------------------------------------------
    Popup {
        id: notificationsStorageViewBox
        width: parent.width * 0.80
        height: parent.height * 0.80
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        parent: idleRoot
        dim: true

        Overlay.modeless: Rectangle {
            id: modelessBg
            color: Qt.rgba(0, 0, 0, 0.75)

            FastBlur {
                anchors.fill: modelessBg
                source: modelessBg
                radius: 32
            }
        }

        background: Rectangle { color: "transparent" }

        Row {
            id: topBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height * 0.20
            spacing: parent.width * 0.10

            Rectangle {
                width: parent.width * 0.15
                height: parent.height
                color: "#212121"
                radius: 10

                Kirigami.Icon {
                    width: parent.width * 0.75
                    height: width
                    anchors.centerIn: parent
                    source: Qt.resolvedUrl("icons/dialog-close.svg")
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: notificationsStorageViewBox.close()
                }
            }

            Rectangle {
                width: parent.width * 0.35
                height: parent.height
                color: "#212121"
                radius: 10

                Label {
                    width: parent.width
                    height: 80
                    anchors.centerIn: parent
                    font.capitalization: Font.AllUppercase
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.weight: Font.ExtraBold
                    fontSizeMode: Text.Fit
                    font.pixelSize: height
                    text: qsTr("Notifications")
                    color: "#ffffff"
                }
            }

            Rectangle {
                width: parent.width * 0.30
                height: parent.height
                color: "#212121"
                radius: 10

                Kirigami.Icon {
                    anchors.centerIn: parent
                    opacity: 0.2
                    width: parent.height / 1.5
                    height: parent.height / 1.5
                    source: Qt.resolvedUrl("icons/clear.svg")
                    color: "white"
                }

                Label {
                    width: parent.width
                    height: 80
                    anchors.centerIn: parent
                    font.capitalization: Font.AllUppercase
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.weight: Font.ExtraBold
                    fontSizeMode: Text.Fit
                    font.pixelSize: height
                    text: qsTr("Clear")
                    color: "#ffffff"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Mycroft.SoundEffects.playClickedSound(Qt.resolvedUrl("sounds/clicked.wav"))
                        sendAllNotificationActions()
                        Mycroft.MycroftController.sendRequest("ovos.notification.api.storage.clear", {})
                    }
                }
            }
        }

        ListView {
            id: notificationsStorageView
            anchors.top: topBar.bottom
            anchors.topMargin: Kirigami.Units.largeSpacing
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            clip: true
            highlightFollowsCurrentItem: false
            spacing: Kirigami.Units.smallSpacing
            property int cellHeight: notificationsStorageView.height
            delegate: NotificationDelegate {}
        }
    }

    // -----------------------------------------------------------------------
    // Example utterance cycling timers
    // -----------------------------------------------------------------------
    Timer {
        id: textTimer
        interval: 30000
        running: false
        repeat: true
        signal runEntryChangeA
        signal runEntryChangeB

        onTriggered: {
            runEntryChangeA()
            setExampleText()
        }
    }

    Timer {
        id: timer
        function setTimeout(cb, delayTime) {
            timer.interval = delayTime
            timer.repeat = false
            timer.triggered.connect(cb)
            timer.triggered.connect(function release() {
                timer.triggered.disconnect(cb)
                timer.triggered.disconnect(release)
            })
            timer.start()
        }
    }

    // -----------------------------------------------------------------------
    // Helper functions
    // -----------------------------------------------------------------------
    function setExampleText() {
        timer.setTimeout(function() {
            textTimer.runEntryChangeB()
            var index = idleRoot.textModel.indexOf(exampleEntry)
            var nextItem
            if (index >= 0 && index < idleRoot.textModel.length - 1) {
                nextItem = idleRoot.textModel[index + 1]
            } else {
                nextItem = idleRoot.textModel[0]
            }
            idleRoot.exampleEntry = nextItem
            exampleEntryUpdate(exampleEntry)
        }, 500)
    }

    function sendAllNotificationActions() {
        if (!notificationModel || !notificationModel.storedmodel) return
        notificationModel.storedmodel.forEach(function(modelData) {
            if (modelData.action !== "") {
                Mycroft.MycroftController.sendRequest(modelData.action, modelData.callback_data)
            }
        })
    }
}
