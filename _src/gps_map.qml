import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6

Item {
    visible: true
    width: 512
    height: 512
    Plugin {
        id: mapPlugin
        name: "osm" // "mapboxgl", "esri", ...
        // specify plugin parameters if necessary
        // PluginParameter {
        //     name:
        //     value:
        // }
    }

    Map {
        id: gpsMap
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(63.418, 10.401951) // Oslo
        zoomLevel: 17

        MapQuickItem {
            id: marker
            objectName: "gpsMapItem"
            coordinate {latitude: 63.418
                        longitude: 10.401951}
            anchorPoint.x: markerImage.width * 0.5
            anchorPoint.y: markerImage.height

            sourceItem: Image {
                id: markerImage
                height: 100
                width: 100
                source: "qrc:/Sleipner_nobkg.png"
            }
           function updatePosition(latitude, longitude) {
               gpsMap.clearMapItems();
               marker.coordinate.latitude = latitude;
               marker.coordinate.longitude = longitude;
               gpsMap.addMapItem(marker);
               gpsMap.center.latitude = latitude;
               gpsMap.center.longitude = longitude;
               gpsMap.update();
           }
        }
    }
}
