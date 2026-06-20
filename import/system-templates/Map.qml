import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property real   latitude:  namespaceData ? (namespaceData.latitude  !== undefined ? namespaceData.latitude  : 0.0) : 0.0
    property real   longitude: namespaceData ? (namespaceData.longitude !== undefined ? namespaceData.longitude : 0.0) : 0.0
    property int    zoom:      namespaceData ? (namespaceData.zoom      !== undefined ? namespaceData.zoom      : 12)  : 12
    property string label:     namespaceData ? (namespaceData.label     || "") : ""

    Plugin {
        id: mapPlugin
        name: "osm"
        // Use default OSM plugin parameters for Qt 6
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(root.latitude, root.longitude)
        zoomLevel: root.zoom

        MapQuickItem {
            visible: root.label.length > 0
            coordinate: QtPositioning.coordinate(root.latitude, root.longitude)
            anchorPoint.x: pin.width / 2
            anchorPoint.y: pin.height
            sourceItem: Column {
                Rectangle {
                    id: pin
                    width: 24; height: 24
                    radius: 12
                    color: "red"
                    border.color: "white"
                    border.width: 2
                }
                Label {
                    text: root.label
                    background: Rectangle { color: "white"; radius: 3 }
                    padding: 2
                }
            }
        }
    }
}
