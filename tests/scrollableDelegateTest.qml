import QtQuick 2.5
import QtQuick.Controls 2.4 as Controls
import QtQuick.Layouts 1.3
import OVOS 1.0 as OVOS
import org.kde.kirigami 2.5 as Kirigami

OVOS.ScrollableDelegate {
    id: root
    width: 500
    height: 500
    ListView {
        model: 100
        delegate: Text{text:"ggg"}
    }
}

