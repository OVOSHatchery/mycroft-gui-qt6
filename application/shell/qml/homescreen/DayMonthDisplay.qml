import QtQuick.Layouts
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects
import OVOS.GUI 1.0 as OVOS

Rectangle {
    id: dayMonthDisplay
    color: "transparent"
    property bool verticalMode: false

    function update_dateFormat(longShortMonth) {
        var longShortMonth = longShortMonth
        switch(idleRoot.dateFormat) {
            case "DMY":
                return namespaceData.weekday_string.substring(0,3) + " " + namespaceData.day_string + " " +  longShortMonth + ", " + namespaceData.year_string
                break
            case "MDY":
                return namespaceData.weekday_string.substring(0,3) + " " + longShortMonth + " " + namespaceData.day_string + ", " + namespaceData.year_string
                break
            case "YMD":
                return namespaceData.year_string + ", " + longShortMonth + " " + namespaceData.weekday_string.substring(0,3) + " " + namespaceData.day_string
                break
            default:
                return namespaceData.weekday_string.substring(0,3) + " " + namespaceData.day_string + " " +  longShortMonth + ", " + namespaceData.year_string
                break
        }
    }

    Label {
        id: weekday
        width: parent.width
        height: parent.height
        fontSizeMode: Text.Fit
        minimumPixelSize: dayMonthDisplay.verticalMode ? 30 : 50
        font.pixelSize: Math.round(parent.height * 0.725)
        horizontalAlignment: dayMonthDisplay.verticalMode ? Text.AlignHCenter : (idleRoot.rtlMode ? Text.AlignRight : Text.AlignLeft)
        verticalAlignment: Text.AlignVCenter
        maximumLineCount: 1
        elide: idleRoot.rtlMode ? Text.ElideLeft : Text.ElideRight
        font.weight: Font.DemiBold
        font.letterSpacing: 1.1
        property var longShortMonth: horizontalMode ? (namespaceData.month_string ? namespaceData.month_string : "" ) : (namespaceData.month_string ? namespaceData.month_string.substring(0,3) : "")
        text: namespaceData.year_string && namespaceData.weekday_string ? dayMonthDisplay.update_dateFormat(longShortMonth) : ""
        color: "white"
        layer.enabled: true
        layer.effect: DropShadow {
            verticalOffset: 4
            color: idleRoot.shadowColor
            radius: 11
            spread: 0.4
            samples: 16
        }
    }
}
