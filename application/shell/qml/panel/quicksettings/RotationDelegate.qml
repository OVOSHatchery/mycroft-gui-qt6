import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS

Delegate {
    iconSource: "qrc://icons/screen-rotate"
    text: qsTr("Rotation")

    onClicked: {
        if (applicationSettings.rotation === "CW") {
            applicationSettings.rotation = "NORMAL";
        } else if (applicationSettings.rotation === "NORMAL") {
            applicationSettings.rotation = "CCW";
        } else if (applicationSettings.rotation === "CCW") {
            applicationSettings.rotation = "UD";
        } else {
            //if (applicationSettings.rotation === "UD") {
            applicationSettings.rotation = "CW";
        }
        console.log(applicationSettings.rotation)
    }
}
