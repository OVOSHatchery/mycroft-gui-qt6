import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

// Modernized HomeScreen template ported from legacy ovos-skill-homescreen
// Displays wallpaper, clock, date, and widgets.
OVOS.Page {
    id: root
    
    // Properties matching legacy homescreen session data
    property string wallpaper_path: namespaceData.wallpaper_path || "https://raw.githubusercontent.com/OpenVoiceOS/ovos-PHAL-plugin-wallpaper-manager/refs/heads/dev/ovos_PHAL_plugin_wallpaper_manager/wallpapers/default.jpg"
    property string time_string: namespaceData.time_string || "--:--"
    property string date_string: namespaceData.date_string || ""
    property var notification_model: namespaceData.notification_model || []
    
    // UI state
    property bool horizontalMode: width > height
    
    // Background Wallpaper
    background: Image {
        source: root.wallpaper_path
        fillMode: Image.PreserveAspectCrop
        
        // Gradient overlay for readability
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0.4) }
                GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.6) }
            }
        }
    }
    
    // Main Layout
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 16
        
        // Top Area: Date and Connectivity (Mocked)
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: root.date_string
                font.pixelSize: 24
                color: "white"
                opacity: 0.9
            }
            
            Item { Layout.fillWidth: true }
            
            // Notification indicator
            Row {
                spacing: 8
                visible: root.notification_model.length > 0
                
                Label {
                    text: root.notification_model.length
                    color: "white"
                    font.bold: true
                }
                
                // Icon would go here
            }
        }
        
        Item { Layout.fillHeight: true }
        
        // Center Area: Big Clock
        ColumnLayout {
            Layout.alignment: Qt.AlignCenter
            spacing: 0
            
            Label {
                text: root.time_string
                font.pixelSize: root.horizontalMode ? 160 : 120
                font.weight: Font.ExtraLight
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }
        }
        
        Item { Layout.fillHeight: true }
        
        // Bottom Area: Examples or Weather summary (Mocked)
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: "Ask me: \"What's the weather?\""
                font.italic: true
                color: "white"
                opacity: 0.6
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
