import QtQuick
import QtWebEngine
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string html:         namespaceData ? (namespaceData.html         || "") : ""
    property string resource_url: namespaceData ? (namespaceData.resource_url || "about:blank") : "about:blank"

    WebEngineView {
        id: webview
        anchors.fill: parent

        function updateContent() {
            if (root.html.length > 0) {
                webview.loadHtml(root.html, root.resource_url)
            }
        }

        Component.onCompleted: updateContent()
    }

    onHtmlChanged: webview.updateContent()
}
