/*
 * ============================================================================
 * MAIN QML FILE - Entry point for the OVOS Namespace GUI
 * ============================================================================
 * 
 * WHAT IS THIS FILE?
 * This is the main UI definition for the OVOS GUI application (not shell mode).
 * It defines the application window, navigation, and skill display area.
 * 
 * QML BASICS FOR NEW DEVELOPERS:
 * - QML = Qt Markup Language (declarative UI, like HTML + JavaScript)
 * - Think of it like: HTML defines structure, CSS defines style, JS adds logic
 * - QML elements: Rectangle (div), Text (p), Button, Image, etc.
 * - "import" statements bring in Qt modules (like 'import React' in JS)
 * 
 * KEY CONCEPTS IN THIS FILE:
 * - Kirigami.ApplicationWindow: KDE's mobile-friendly app framework
 * - OVOS.NamespaceView: The area where namespace GUIs are displayed
 * - OVOSController: C++ object that handles WebSocket to OVOS core
 * - GlobalDrawer: Side menu with settings, hints, about pages
 * 
 * HOW DATA FLOWS:
 * 1. OVOS core sends message over WebSocket
 * 2. OVOSController (C++) receives it
 * 3. C++ updates SessionDataMap (skill data storage)
 * 4. QML bindings automatically update the UI
 * ============================================================================
 */

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import org.kde.kirigami 2.19 as Kirigami
import OVOS.GUI 1.0 as OVOS
import Qt5Compat.GraphicalEffects

// ============================================================================
// KIRIGAMI APPLICATION WINDOW
// ============================================================================
// Kirigami is KDE's UI framework - provides mobile-friendly components
// ApplicationWindow is the root of our Qt application (similar to <html>)
// All other QML elements are children of this window
Kirigami.ApplicationWindow {
    // Unique identifier for this component
    id: root
    
    // Make the window visible immediately when QML loads
    visible: true

    // =========================================================================
    // WINDOW SIZE
    // =========================================================================
    // If deviceWidth/Height are set via command line, use those
    // Otherwise fall back to default 640x480
    width: deviceWidth > 0 ? deviceWidth : 640
    height: deviceHeight > 0 ? deviceHeight : 480
    
    // Position window at bottom-right of available desktop area
    // This is important for device screens that aren't full desktop
    x: deviceWidth > 0 ? Screen.desktopAvailableWidth - width : 0
    y: deviceHeight > 0 ? Screen.desktopAvailableHeight - height : 0

    // Background color - black for better contrast with colorful namespace UIs
    color: "black"

    // =========================================================================
    // APPLICATION INITIALIZATION
    // =========================================================================
    // Component.onCompleted runs once when this QML element is fully loaded
    // This is like $(document).ready() in jQuery
    Component.onCompleted: {
        // HACK: Configure drawer handle anchor for navigation drawer
        // This is a workaround for Kirigami API quirk
        globalDrawer.handle.handleAnchor = handleAnchor;

        // If --maximize was passed, maximize the window
        if (deviceMaximized) {
            showMaximized()
        }

        // If a specific skill home page was requested (--skill weather.home)
        // and OVOS is already connected, navigate directly to that skill
        if (singleNamespaceHome.length > 0 && OVOS.GuiBusClient.status === OVOS.GuiBusClient.Open) {
            OVOS.GuiBusClient.sendRequest(singleNamespaceHome, {});
        }
        
        // Note: STT (speech-to-text) and TTS (text-to-speech) are handled
        // by OVOS core, not this display client. The GUI just shows visual output.
    }

    // =========================================================================
    // CONNECT TO OVOS CORE SIGNALS
    // =========================================================================
    // Connections let us react to events from OVOSController (C++)
    // Think of it like event listeners: element.addEventListener('event', handler)
    Connections {
        // Listen to OVOSController's status changes (connecting, open, closed)
        target: OVOS.GuiBusClient
        
        // When connection status changes, check if we should load skill home
        function onStatusChanged(status) {
            if (singleNamespaceHome.length > 0 && OVOS.GuiBusClient.status === OVOS.GuiBusClient.Open) {
                OVOS.GuiBusClient.sendRequest(singleNamespaceHome, {});
            }
        }
        
        // When a skill times out (user was idle), close the skill GUI
        // This allows returning to the homescreen
        function onNamespaceTimeoutReceived(namespace_id) {
            if(mainView.currentItem.contentItem.namespaceId() == namespace_id) {
                root.close()
            }
        }
    }
    
    // =========================================================================
    // ANOTHER HACK FOR KIRIGAMI DRAWER
    // =========================================================================
    // Track when page stack depth changes to adjust drawer handle
    // This is a known Kirigami quirk that needs workaround
    Connections {
        target: root.pageStack.layers
        function onDepthChanged() {
            if (root.pageStack.layers.depth == 1) {
                globalDrawer.handle.handleAnchor = handleAnchor;
            } else {
                globalDrawer.handle.handleAnchor = null;
            }
        }
    }

    // =========================================================================
    // GLOBAL DRAWER (SIDE MENU)
    // =========================================================================
    // GlobalDrawer is a slide-out navigation menu (like Facebook app)
    // Contains: Hints, Settings, About, Dark Mode toggle
    globalDrawer: Kirigami.GlobalDrawer {
        // Header image at top of drawer
        header: Image {
            source: "banner.png"
            fillMode: Image.PreserveAspectFit
        }
        
        // Show/hide drawer handle based on mode
        handleVisible: !hideTextInput
        
        // Configure theming - support dark mode
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: applicationSettings.darkMode ? Kirigami.Theme.Complementary : Kirigami.Theme.View

        // Menu actions in the drawer
        // Each Kirigami.Action is like a menu item
        actions: [
            // HINTS - show skill suggestions
            Kirigami.Action {
                text: "Hints"
                icon.name: "help-hint"
                // Only show on desktop, not mobile
                visible: !Kirigami.Settings.isMobile
                // Check if hints page is currently shown
                checked: pageStack.layers.currentItem.objectName == "hints"
                onTriggered: {
                    // Toggle hints page visibility
                    if (checked) {
                        pageStack.layers.pop(pageStack.layers.initialItem);
                    } else if (pageStack.layers.depth > 1) {
                        pageStack.layers.replace(Qt.resolvedUrl("HintsPage.qml"));
                    } else {
                        pageStack.layers.push(Qt.resolvedUrl("HintsPage.qml"));
                    }
                }
            },
            
            // SETTINGS - configuration page
            Kirigami.Action {
                text: "Settings"
                icon.name: "configure"
                checked: pageStack.layers.currentItem.objectName == "Settings"
                onTriggered: {
                    if (checked) {
                        pageStack.layers.pop(pageStack.layers.initialItem);
                    } else if (pageStack.layers.depth > 1) {
                        pageStack.layers.replace(Qt.resolvedUrl("SettingsPage.qml"));
                    } else {
                        pageStack.layers.push(Qt.resolvedUrl("SettingsPage.qml"));
                    }
                }
            },
            
            // ABOUT - app info
            Kirigami.Action {
                text: "About"
                icon.name: "help-about"
                checked: pageStack.layers.currentItem.objectName == "About"
                onTriggered: {
                    if (checked) {
                        pageStack.layers.pop(pageStack.layers.initialItem);
                    } else if (pageStack.layers.depth > 1) {
                        pageStack.layers.replace(Qt.resolvedUrl("AboutPage.qml"));
                    } else {
                        pageStack.layers.push(Qt.resolvedUrl("AboutPage.qml"));
                    }
                }
            }
        ]
        
        // Dark mode toggle switch at bottom of drawer
        Switch {
            id: nightSwitch
            visible: !Kirigami.Settings.isMobile
            text: "Dark Mode"
            // Bind to application settings - changes sync automatically!
            checked: applicationSettings.darkMode
            // When toggle changes, update the setting
            onCheckedChanged: applicationSettings.darkMode = checked
        }
    }

    // =========================================================================
    // AUTO-CONNECT TIMER
    // =========================================================================
    // Timer that tries to connect to OVOS if not connected
    // Runs every 20 seconds if not connected and autoConnect is enabled
    Timer {
        interval: 20000  // 20 seconds
        // Keep running if autoConnect is on AND not connected
        running: OVOS.GlobalSettings.autoConnect && OVOS.GuiBusClient.status != OVOS.GuiBusClient.Open
        // Trigger immediately on start (don't wait 20s first time)
        triggeredOnStart: true
        onTriggered: {
            print("Trying to connect to OVOS");
            OVOS.GuiBusClient.start();
        }
    }

    // =========================================================================
    // PAGE STACK (NAVIGATION)
    // =========================================================================
    // Configure toolbar style based on whether we're in main view or layers
    pageStack.globalToolBar.style: pageStack.layers.depth == 1 ? Kirigami.ApplicationHeaderStyle.None : Kirigami.ApplicationHeaderStyle.Auto

    // =========================================================================
    // INITIAL PAGE (MAIN CONTENT AREA)
    // =========================================================================
    // This is the main content - where namespace UIs are displayed
    pageStack.initialPage: Kirigami.Page {
        // Remove default padding - we want full-bleed content
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        actions: [
            Kirigami.Action {
                text: "Settings"
                icon.name: "configure"
                onTriggered: pageStack.layers.push(Qt.resolvedUrl("SettingsPage.qml"))
            },
            Kirigami.Action {
                text: "About"
                icon.name: "help-about"
                onTriggered: pageStack.layers.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        ]
        
        // Handle back button press (e.g., Android back button)
        onBackRequested: {
            if (mainView.active) {
                event.accepted = true  // Prevent default
                mainView.goBack()      // Let skill handle it
            }
        }
        
        // Main container - covers the whole page
        Rectangle {
            // Background color: black if dark mode, otherwise theme background
            color: nightSwitch.checked ? "black" : Kirigami.Theme.backgroundColor
            
            // Apply screen rotation if set via command line
            rotation: globalScreenRotation || 0
            
            // Fill parent (the Page)
            anchors.fill: parent
            
            // =========================================================================
            // BACKGROUND IMAGE
            // =========================================================================
            // Show OVOS logo background when no skill is active
            // Fades out when a skill is displayed (mainView.currentItem exists)
            Image {
                visible: singleNamespace.length === 0
                source: "background.png"
                fillMode: Image.PreserveAspectFit
                anchors.fill: parent
                opacity: !mainView.currentItem ? 1 : 0  // 1 = fully visible, 0 = hidden
                
                // Animate opacity changes smoothly
                Behavior on opacity {
                    OpacityAnimator {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.InQuad
                    }
                }
            }

            // =========================================================================
            // NAMESPACE VIEW (WHERE SKILL GUIS RENDER)
            // =========================================================================
            // This is the core component that displays namespace UIs
            // It receives messages from OVOSController and renders accordingly
            OVOS.NamespaceView {
                id: mainView
                
                // If --skill was specified, only show that skill
                // Otherwise show all active skills
                activeNamespaces.whiteList: singleNamespace.length > 0 ? [singleNamespace] : []
                
                // Theme colors based on dark mode setting
                Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.View
                
                // Fill the available space
                anchors.fill: parent
            }

            // =========================================================================
            // START BUTTON (WHEN NOT CONNECTED)
            // =========================================================================
            // Show a "Start" button if OVOS is not connected
            // This is the manual connect button
            Button {
                anchors.centerIn: parent
                text: "start"
                // Only visible when status is "Closed" (not connecting or open)
                visible: OVOS.GuiBusClient.status == OVOS.GuiBusClient.Closed
                onClicked: (mouse)=> {
                    OVOS.GuiBusClient.start();
                }
            }

            // =========================================================================
            // STATUS INDICATOR
            // =========================================================================
            // Shows connection status (speaking, listening, etc.)
            // Positioned in top-right corner
            OVOS.StatusIndicator {
                id: si
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: Kirigami.Units.largeSpacing
                }
                z: 999  // Always on top of other elements
            }

            // =========================================================================
            // UTTERANCE DISPLAY (WHAT USER SAID)
            // =========================================================================
            // Shows the text of what the user said (for visual feedback)
            // Appears next to status indicator
            Kirigami.Heading {
                id: inputQuery
                Kirigami.Theme.colorSet: mainView.Kirigami.Theme.colorSet
                anchors.right: si.left
                anchors.rightMargin: Kirigami.Units.largeSpacing
                anchors.verticalCenter: si.verticalCenter
                level: 3  // Heading size (1=largest, 6=smallest)
                
                // Start invisible, fade in when user types
                opacity: 0
                
                // When text changes, show it and start auto-hide timer
                onTextChanged: {
                    opacity = 1;
                    utteranceTimer.restart();
                }
                
                // Auto-hide timer - hide utterance after 8 seconds
                Timer {
                    id: utteranceTimer
                    interval: 8000
                    onTriggered: {
                        inputQuery.text = "";
                        inputQuery.opacity = 0
                    }
                }
                
                // Animate opacity changes
                Behavior on opacity {
                    OpacityAnimator {
                        duration: Kirigami.units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }

                // Listen for skill handling the utterance
                Connections {
                    target: OVOS.GuiBusClient
                    function onUtteranceManagedByNamespace(skill) {
                        // Update display when a skill handles an utterance
                    }
                }
            }
        }

        // =========================================================================
        // FOOTER (TEXT INPUT AREA)
        // =========================================================================
        // Bottom bar with text input for typing commands
        footer: Control {
            // Theme based on dark mode
            Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            
            // Hide entirely in kiosk/tablet mode (--hideTextInput)
            visible: !hideTextInput
            height: hideTextInput ? 0 : implicitHeight
            implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
            
            // Container for the input elements
            contentItem: RowLayout {
                // Handle anchor for navigation drawer
                Item {
                    id: handleAnchor
                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                }
                
                // Back button (Visible on all platforms)
                ToolButton {
                    id: backButton
                    Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                    Layout.preferredWidth: handleAnchor.width
                    Layout.fillHeight: true
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                    enabled: true
                    icon.name: "go-previous"
                    
                    onClicked:(mouse)=> {
                        if (mainView.currentItem) {
                            mainView.currentItem.backRequested()
                        }
                    }
                    visible: true
                }
                
                
                // =========================================================================
                // TEXT INPUT FIELD
                // =========================================================================
                // Where user types commands to OVOS
                TextField {
                    id: qinput
                    Layout.fillWidth: true

                    placeholderText: "Ask OVOS..."
                    
                    // When user presses Enter/Return
                    onAccepted: {
                        var txt = qinput.text.trim()
                        if (txt !== "") {
                            // Send text to OVOS core for processing
                            OVOS.GuiBusClient.sendText(txt)
                            qinput.text = ""
                        }
                    }
                    focus: false
                    
                    // Note: speechRequested was removed - STT is handled by OVOS core
                    
                    // Select all text when field gets focus
                    onFocusChanged: {
                        if (focus) {
                            selectAll();
                        }
                    }
                }
                
                // =========================================================================
                // MICROPHONE BUTTON (HIDDEN)
                // =========================================================================
                // This button was used for voice input, but it's now hidden
                // because STT (speech-to-text) is handled by OVOS core directly
                // The display client just shows the visual result
                ToolButton {
                    id: micButton
                    Kirigami.Theme.colorSet: nightSwitch.checked ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                    Layout.preferredWidth: handleAnchor.width
                    Layout.fillHeight: true
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                    icon.name: "audio-input-microphone"
                    
                    // Mic button hidden — STT handled by OVOS core, not the display client
                    visible: false
                }
            }
            
            // Background of the footer - gradient for visual polish
            background: Rectangle {
                color: Kirigami.Theme.backgroundColor
                LinearGradient {
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.top
                    }
                    implicitHeight: Kirigami.Units.gridUnit/2

                    start: Qt.point(0, height)
                    end: Qt.point(0, 0)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: Qt.rgba(0, 0, 0, 0.2)
                        }
                        GradientStop {
                            position: 0.3
                            color: Qt.rgba(0, 0, 0, 0.1)
                        }
                        GradientStop {
                            position: 1.0
                            color:  "transparent"
                        }
                    }
                }
            }
        }
    }
}
