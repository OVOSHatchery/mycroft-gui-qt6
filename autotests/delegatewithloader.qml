import QtQuick.Layouts 1.4
import QtQuick 2.4
import QtQuick.Controls 2.0
import org.kde.kirigami 2.5 as Kirigami
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: mainLoaderView

    property var pageToLoad: namespaceData.state

    contentItem: Loader {
        id: rootLoader
    }

    onPageToLoadChanged: {
        console.log(namespaceData.state)
        rootLoader.setSource(namespaceData.state + ".qml")
    }
}
