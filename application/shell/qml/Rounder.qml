import QtQuick
import Qt5Compat.GraphicalEffects

Rectangle {
    id: control
    width: parent.width
    height: parent.height
    property real value: 0
    color: "transparent"
    radius: parent.width / 2
    property alias glowColor: circle.borderColor
    property color backgroundBorderColor

    SequentialAnimation on color {
        id: expireAnimationBg
        running: false
        loops: Animation.Infinite
        PropertyAnimation {
            from: control.backgroundBorderColor;
            to: "transparent";
            duration: 1000
        }
        PropertyAnimation {
            from: "transparent";
            to: control.backgroundBorderColor;
            duration: 1000
        }
    }

    Row{
        id: circle
        property color circleColor: "transparent"
        property color borderColor
        property int borderWidth: 15
        anchors.fill: parent
        anchors.margins: -6
        width: parent.width
        height: width

        Item{
            width: parent.width/2
            height: parent.height
            clip: true

            Item{
                id: part1
                width: parent.width
                height: parent.height
                clip: true
                rotation: value > 0.5 ? 360 : 180 + 360*value
                transformOrigin: Item.Right

                Rectangle{
                    width: circle.width-(circle.borderWidth*2)
                    height: circle.height-(circle.borderWidth*2)
                    radius: width/2
                    x:circle.borderWidth
                    y:circle.borderWidth
                    color: circle.circleColor
                    border.color: circle.borderColor
                    border.width: circle.borderWidth
                    smooth: true
                }
            }
        }

        Item{
            width: parent.width/2
            height: parent.height
            clip: true

            Item{
                id: part2
                width: parent.width
                height: parent.height
                clip: true
                rotation: value <= 0.5 ? 180 : 360*(value)
                transformOrigin: Item.Left

                Rectangle{
                    width: circle.width-(circle.borderWidth*2)
                    height: circle.height-(circle.borderWidth*2)
                    radius: width/2
                    x: -width/2
                    y: circle.borderWidth
                    color: circle.circleColor
                    border.color: circle.borderColor
                    border.width: circle.borderWidth
                    smooth: true
                }
            }
        }
    }
}
