import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import org.kde.kirigami as Kirigami

Item {
    id: splashRoot
    width: parent.width
    height: parent.height
    property bool lightBgMode: false
    property bool horizontalMode: parent.width >= parent.height ? 1 : 0

    function checkLightBgMode() {
        var r = Kirigami.Theme.backgroundColor.r
        var g = Kirigami.Theme.backgroundColor.g
        var b = Kirigami.Theme.backgroundColor.b

        if (r > 0.75 && g > 0.75 && b > 0.75) {
            lightBgMode = true
        } else {
            lightBgMode = false
        }
    }

    Component.onCompleted: {
        checkLightBgMode()
    }

    ParallelAnimation {
        running: true

        PropertyAnimation {
            target: splashCard
            property: "opacity"
            from: 1
            to: 1
            duration: 2000
        }
        PropertyAnimation {
            target: centeralCircle
            property: "opacity"
            from: 0
            to: 1
            duration: 2000
        }

        onFinished: {
            mainAnimRings.start()
        }
    }


    SequentialAnimation {
        id: mainAnimRings
        running: false
        loops: Animation.Infinite

        ParallelAnimation {
            PropertyAnimation {
                target: rectAn1
                property: "value"
                from: 0
                to: 1
                duration: 9000
            }

            PropertyAnimation {
                target: rectAn1
                property: "rotation"
                from: 0
                to: 760
                duration: 9000
            }

            PropertyAnimation {
                target: rectAn2
                property: "value"
                from: 0
                to: 1
                duration: 9000
            }

            PropertyAnimation {
                target: rectAn2
                property: "rotation"
                from: 0
                to: -760
                duration: 9000
            }
        }

        ParallelAnimation {
            PropertyAnimation {
                target: rectAn1
                property: "value"
                from: 1
                to: 0
                duration: 12000
            }

            PropertyAnimation {
                target: rectAn1
                property: "rotation"
                from: 760
                to: 0
                duration: 12000
            }

            PropertyAnimation {
                target: rectAn2
                property: "value"
                from: 1
                to: 0
                duration: 12000
            }

            PropertyAnimation {
                target: rectAn2
                property: "rotation"
                from: -760
                to: 0
                duration: 12000
            }
        }
    }

    Canvas {
        id: cnv
        anchors.fill: parent

        property var cw: cnv.width
        property var ch: cnv.height
        property var cx: cw / 2
        property var cy: ch / 2

        onPaint: {
            const ctx = cnv.getContext('2d')

            const cfg = {
                bgFillColor: Qt.darker(Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.05), 2),
                dirsCount: 6,
                stepsToTurn: 60,
                dotSize: 10,
                dotsCount: 50,
                dotColor: Kirigami.Theme.highlightColor
            }

            const drawRect = (color,x,y,w,h) => {
                ctx.fillStyle = color
                ctx.fillRect(x,y,w,h)
            }

            class Dot {
                constructor() {
                    this.pos = {x: cx, y: cy}
                    this.dir = (Math.random() * 3 | 0) * 2
                    this.step = 0
                }

                redrawDot() {
                    let color = cfg.dotColor
                    let size = cfg.dotSize
                    let x = this.pos.x - size / 2
                    let y = this.pos.y - size / 2

                    drawRect(color,x,y,size,size)
                }

                moveDot() {
                    this.step++
                    this.pos.x += dirsList[this.dir].x
                    this.pos.y += dirsList[this.dir].y
                }

                changeDir() {
                    if(this.step % cfg.stepsToTurn == 0) {
                        this.dir = Math.random() > .5 ? (this.dir+1) % cfg.dirsCount : (this.dir + cfg.dirsCount -1) % cfg.dirsCount
                    }
                }
            }
            let dirsList = []
            const createDirs = () => {
                for(let i = 0; i < 360; i += 360 / 6) {
                    let x = Math.cos(i * Math.PI / 180)
                    let y = Math.sin(i * Math.PI / 180)
                    dirsList.push({x:x,y:y})
                }
            }
            createDirs()

            let dotsList = []
            const addDot = () => {
                if(dotsList.length < cfg.dotsCount && Math.random() > .8) {
                    dotsList.push(new Dot())
                }
            }

            const refreshDots = () => {
                dotsList.forEach(i => {
                                     i.moveDot()
                                     i.redrawDot()
                                     i.changeDir()
                                 })
            }

            const loop = () => {
                drawRect(cfg.bgFillColor, 0,0,cw,ch)
                addDot()
                refreshDots()

                requestAnimationFrame(loop)
            }
            loop()

        }
    }

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.3)
        layer.enabled: true
        layer.effect: FastBlur {
            source: cnv
            anchors.fill: parent
            radius: 32
        }
    }

    Item {
        id: splashCard
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height

        Rounder {
            id: rectAn1
            width: centeralCircle.width + 90
            height: width
            anchors.centerIn: parent
            glowColor: Kirigami.Theme.textColor
            backgroundBorderColor: "#313131"
        }

        Rounder {
            id: rectAn2
            width: centeralCircle.width + 80
            height: width
            anchors.centerIn: parent
            glowColor: Kirigami.Theme.highlightColor
            backgroundBorderColor: "#313131"
        }

        Rectangle {
            id: centeralCircle
            width: horizontalMode ? splashCard.height * 0.75 : splashCard.width * 0.50
            height: width
            anchors.centerIn: parent
            color: Qt.rgba(0, 0, 0, 0.5)
            radius: splashRoot.width * splashRoot.height

            Kirigami.Icon {
                id: ovosWaveSvg
                anchors.fill: parent
                anchors.margins: 32
                source: Qt.resolvedUrl("icons/ovos-wave.svg")
                color: Kirigami.Theme.highlightColor
            }

            ColorOverlay {
                source: ovosWaveSvg
                anchors.fill: ovosWaveSvg
                color: Qt.darker(Kirigami.Theme.highlightColor, 1.25)
            }

            Kirigami.Icon {
                anchors.fill: parent
                anchors.margins: 32
                source: Qt.resolvedUrl("icons/ovos-egg.svg")
            }
        }
    }
}
