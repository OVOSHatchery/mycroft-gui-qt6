import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

// Default idle / resting screen shown when no skill namespace is active.
// In monolithic app, this is identical to HomeScreen for consistent UX.
OVOS.Page {
    id: root
    
    property string wallpaper_path: "https://raw.githubusercontent.com/OpenVoiceOS/ovos-PHAL-plugin-wallpaper-manager/refs/heads/dev/ovos_PHAL_plugin_wallpaper_manager/wallpapers/default.jpg"
    property string time_string: "--:--"
    property string date_string: ""
    
    Timer {
        id: clockTimer
        interval: 1000
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: {
            var now = new Date()
            root.time_string = now.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
            root.date_string = now.toLocaleDateString(Qt.locale(), Locale.LongFormat).toUpperCase()
        }
    }
    
    // Background Wallpaper
    background: Image {
        source: root.wallpaper_path
        fillMode: Image.PreserveAspectCrop
        
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0.4) }
                GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.6) }
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 16
        
        Label {
            text: root.date_string
            font.pixelSize: 24
            color: "white"
            opacity: 0.9
        }
        
        Item { Layout.fillHeight: true }
        
        Label {
            text: root.time_string
            font.pixelSize: width > height ? 160 : 120
            font.weight: Font.ExtraLight
            color: "white"
            Layout.alignment: Qt.AlignCenter
        }
        
        Item { Layout.fillHeight: true }
        
        Label {
            text: "Ask me anything"
            font.italic: true
            color: "white"
            opacity: 0.6
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
