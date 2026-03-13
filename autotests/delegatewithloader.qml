import QtQuick.Layouts 1.4
import QtQuick 2.4
import QtQuick.Controls 2.0
import org.kde.kirigami 2.5 as Kirigami
import OVOS 1.0 as OVOS

OVOS.Delegate {
    id: mainLoaderView

    property var pageToLoad: sessionData.state

    contentItem: Loader {
        id: rootLoader
    }

    onPageToLoadChanged: {
        console.log(sessionData.state)
        rootLoader.setSource(sessionData.state + ".qml")
    }
}
