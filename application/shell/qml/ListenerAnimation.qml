import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import org.kde.kirigami as Kirigami
import OVOS 1.0 as OVOS

Rectangle {
    id: rootAnimator
    property Gradient borderGradient: borderGradient
    property int borderWidth: Kirigami.Units.largeSpacing - Kirigami.Units.smallSpacing
    property bool horizontalMode: parent.width > parent.height ? 1 : 0
    readonly property color primaryBorderColor: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.9)
    readonly property color secondaryBorderColor: Qt.rgba(1, 1, 1, 0.7)
    color: "transparent"
    visible: false

    Connections {
        target: OVOS.OVOSController
        onIntentRecevied: {
            switch(type){
            case "recognizer_loop:wakeword":
                rootAnimator.visible = true;
                break
            case "mycroft.mic.listen":
                rootAnimator.visible = true;
                break
            case "recognizer_loop:record_end":
                rootAnimator.visible = false;
                break
            case "mycroft.speech.recognition.unknown":
                rootAnimator.visible = false;
                break
            }
        }
    }

    Loader {
        id: loader
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        active: borderGradient
        sourceComponent: border
    }

    Gradient {
        id: borderGradient
        GradientStop {
            position: 0.000
            SequentialAnimation on color {
                loops: Animation.Infinite
                running: rootAnimator.visible
                ColorAnimation { from: rootAnimator.primaryBorderColor; to: rootAnimator.secondaryBorderColor;  duration: 1000 }
                ColorAnimation { from: rootAnimator.secondaryBorderColor; to: rootAnimator.primaryBorderColor;  duration: 1000 }
            }
        }
        GradientStop {
            position: 0.256
            color: Qt.rgba(0, 1, 1, 1)
            SequentialAnimation on color {
                loops: Animation.Infinite
                running: rootAnimator.visible
                ColorAnimation { from: rootAnimator.secondaryBorderColor; to: rootAnimator.primaryBorderColor;  duration: 1000 }
                ColorAnimation { from: rootAnimator.primaryBorderColor; to: rootAnimator.secondaryBorderColor;  duration: 1000 }
            }
        }
        GradientStop {
            position: 0.500
            SequentialAnimation on color {
                loops: Animation.Infinite
                running: rootAnimator.visible
                ColorAnimation { from: rootAnimator.primaryBorderColor; to: rootAnimator.secondaryBorderColor;  duration: 1000 }
                ColorAnimation { from: rootAnimator.secondaryBorderColor; to: rootAnimator.primaryBorderColor;  duration: 1000 }
            }
        }
        GradientStop {
            position: 0.756
            SequentialAnimation on color {
                loops: Animation.Infinite
                running: rootAnimator.visible
                ColorAnimation { from: rootAnimator.secondaryBorderColor; to: rootAnimator.primaryBorderColor;  duration: 1000 }
                ColorAnimation { from: rootAnimator.primaryBorderColor; to: rootAnimator.secondaryBorderColor;  duration: 1000 }
            }
        }
        GradientStop {
            position: 1.000
            SequentialAnimation on color {
                loops: Animation.Infinite
                running: rootAnimator.visible
                ColorAnimation { from: rootAnimator.primaryBorderColor; to: rootAnimator.secondaryBorderColor;  duration: 1000 }
                ColorAnimation { from: rootAnimator.secondaryBorderColor; to: rootAnimator.primaryBorderColor;  duration: 1000 }
            }
        }
    }

    Component {
        id: border
        Item {
            ConicalGradient {
                id: borderFill
                anchors.fill: parent
                gradient: borderGradient
                visible: false
            }

            FastBlur {
                anchors.fill: parent
                source: parent
                radius: 32
            }

            Rectangle {
                id: mask
                radius: rootAnimator.radius
                border.width: rootAnimator.borderWidth
                anchors.fill: parent
                color: 'transparent'
                visible: false
            }

            OpacityMask {
                id: opM
                anchors.fill: parent
                source: borderFill
                maskSource: mask
            }
        }
    }
}
