import QtQuick
import QtWebEngine 1.8

Item {
    id: root

    property string html:         sessionData.html         || ""
    property string resource_url: sessionData.resource_url || "about:blank"

    WebEngineView {
        anchors.fill: parent
        onHtmlChanged: {
            if (html.length > 0) {
                loadHtml(html, resource_url)
            }
        }

        Component.onCompleted: {
            if (html.length > 0) {
                loadHtml(html, resource_url)
            }
        }
    }
}
