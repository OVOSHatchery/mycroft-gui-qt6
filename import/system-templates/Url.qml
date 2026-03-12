import QtQuick
import QtWebEngine 1.8

Item {
    id: root

    property string url: sessionData.url || "about:blank"

    WebEngineView {
        anchors.fill: parent
        url: root.url
    }
}
