import QtQuick
import QtWebEngine
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string url: namespaceData ? (namespaceData.url || "about:blank") : "about:blank"

    WebEngineView {
        anchors.fill: parent
        url: root.url
    }
}
