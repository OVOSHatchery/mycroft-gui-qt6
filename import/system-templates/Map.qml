import QtQuick
import QtQuick.Controls
import QtLocation 5.12
import QtPositioning 5.12

Item {
    id: root

    property real   latitude:  sessionData.latitude  !== undefined ? sessionData.latitude  : 0.0
    property real   longitude: sessionData.longitude !== undefined ? sessionData.longitude : 0.0
    property int    zoom:      sessionData.zoom       !== undefined ? sessionData.zoom      : 12
    property string label:     sessionData.label     || ""

    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter { name: "osm.useragent"; value: "OVOS-GUI" }
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(latitude, longitude)
        zoomLevel: zoom

        MapQuickItem {
            visible: label.length > 0
            coordinate: QtPositioning.coordinate(latitude, longitude)
            anchorPoint.x: pin.width / 2
            anchorPoint.y: pin.height
            sourceItem: Column {
                Image {
                    id: pin
                    source: "qrc:/qt-project.org/imports/QtLocation/images/location-marker.png"
                    width: 32; height: 32
                }
                Label {
                    text: label
                    background: Rectangle { color: "white"; radius: 3 }
                    padding: 2
                }
            }
        }
    }
}
