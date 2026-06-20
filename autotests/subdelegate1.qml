 
import QtQuick 2.9
import QtQuick.Controls 2.0
import org.kde.lottie 1.0
import OVOS.GUI 1.0 as OVOS

Rectangle {
    color: "red"
    LottieAnimation {
        id: thinkingAnimation
        visible: true
        anchors.fill:parent
        source: Qt.resolvedUrl("./sunny.json")
        loops: Animation.Infinite
        fillMode: Image.PreserveAspectFit
        running: true
    }
}
