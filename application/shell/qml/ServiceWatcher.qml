import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS

Item {
    id: serviceWatcherRoot
    property bool skillServiceAlive: false
    property bool guiServiceAlive: false

    function queryGuiServiceIsAlive() {
        OVOS.GuiBusClient.sendRequest("mycroft.gui_service.is_alive", {})
    }

    function querySkillServiceIsAlive() {
        OVOS.GuiBusClient.sendRequest("mycroft.skills.is_alive", {})
    }

    Timer {
        id: serviceCheckTimer
        interval: 30000
        running: true
        repeat: true
        onTriggered: {
            queryGuiServiceIsAlive()
            querySkillServiceIsAlive()
        }
    }

    Connections {
        target: OVOS.GuiBusClient

        onIntentRecevied: {
            if(type == "mycroft.gui_service.is_alive.response"){
                serviceWatcherRoot.guiServiceAlive = Boolean(data.status)
            }

            if(type == "mycroft.skills.is_alive.response"){
                serviceWatcherRoot.skillServiceAlive = Boolean(data.status)
            }
        }
    }

    Loader {
        id: splashAnimation
        anchors.fill: parent
        enabled: !serviceWatcherRoot.guiServiceAlive
        visible: !serviceWatcherRoot.guiServiceAlive
        source: "SplashScreen.qml"
    }
}
