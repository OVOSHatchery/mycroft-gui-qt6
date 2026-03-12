import QtQuick
import QtQuick.Controls

Item {
    id: control
    property alias color: marqueeText.color
    property alias text: rtlDetector.text
    property alias font: marqueeText.font
    property alias minimumPointSize: marqueeText.minimumPointSize
    property alias minimumPixelSize: marqueeText.minimumPixelSize
    property alias fontSizeMode: marqueeText.fontSizeMode
    property alias verticalAlignment: marqueeText.verticalAlignment
    property alias paintedWidth: marqueeText.paintedWidth
    property int speed: 4000
    property int delay: 4000
    property var marqueeWidth: width
    property bool rightToLeft: false
    property int distance: marqueeWidth / 2

    onWidthChanged: {
        marqueeWidth = width
        reset()
    }

    function reset() {
        if(marqueeAnimator.running) {
            marqueeAnimator.stop()
            marqueeText.width = control.marqueeWidth
            marqueeText.x = control.x
            coverText.width = marqueeText.width
            coverText.x = marqueeText.width
            marqueeAnimator.start()
        } else {
            marqueeAnimator.start()
        }
        marqueeAnimator.start()
        marqueeText.enabled = false
        marqueeText.enabled = true
    }

    Text {
        id: rtlDetector
        visible: false
        width: parent.width

        onHorizontalAlignmentChanged: {
            if(rtlDetector.horizontalAlignment == Text.AlignRight) {
                leftToRight = true;
            }
        }
    }

    Text {
        id: marqueeText;
        horizontalAlignment: Text.AlignHLeft
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        width: parent.width
        elide: Text.ElideNone
        text: rtlDetector.text
        visible: x < parent.width ? 1 : 0
    }

    Text {
        id: coverText;
        horizontalAlignment: marqueeText.horizontalAlignment
        verticalAlignment: marqueeText.verticalAlignment
        height: marqueeText.height
        width: marqueeText.width
        elide: marqueeText.elide
        x: marqueeText.x - marqueeText.width
        font: marqueeText.font
        color: marqueeText.color
        text: marqueeText.text
        minimumPointSize: marqueeText.minimumPointSize
        minimumPixelSize: marqueeText.minimumPixelSize
        fontSizeMode: marqueeText.fontSizeMode
        opacity: 0
    }

    SequentialAnimation {
        id: marqueeAnimator
        loops: Animation.Infinite
        running: false

        PropertyAnimation {
            id: marqueeAnimatorDelayAnimator
            target: marqueeText
            property: "opacity"
            from: 1
            to: 1
            duration: control.delay
        }

        ScriptAction {
            script: coverText.opacity = 1
        }

        ParallelAnimation {
            PropertyAnimation {
                target: marqueeText
                property: "x"
                from: 0
                to: !rightToLeft ? (control.x + marqueeText.paintedWidth) + distance : (control.x - marqueeText.paintedWidth) - distance
                duration: speed
            }

            PropertyAnimation {
                target: coverText
                property: "x"
                from: !rightToLeft ? (control.x - marqueeText.paintedWidth) - distance  : (control.x + marqueeText.paintedWidth) + distance
                to: marqueeText.x
                duration: speed
            }
        }
    }
}
