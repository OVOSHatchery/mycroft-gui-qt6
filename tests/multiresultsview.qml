import QtQuick 2.5
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import OVOS.GUI 1.0 as OVOS

ColumnLayout {
    width: 500
    height: 500

    TextField {
        id: input
        Layout.fillWidth: true
        onAccepted: {
            OVOS.GuiBusClient.sendText(input.text)
        }
    }
    Component.onCompleted: OVOS.GuiBusClient.start();

    ScrollView {
        id: scroll
        Layout.fillWidth: true
        Layout.fillHeight: true
        Flickable {
            contentWidth: width
            contentHeight: layout.height
            GridLayout {
                id: layout
                width: scroll.width - 20
                columns: 2
                //horizontalSpacing: 5
                OVOS.StackNamespaceView {
                    Layout.rowSpan: 2
                    clip:true
                    width: 400
                    height: 800
                }
                OVOS.StackNamespaceView {
                    clip: true
                    width: 800
                    height: 400
                }
                OVOS.StackNamespaceView {
                    clip: true
                    width: 400
                    height: 400
                }
            }
        }
    }
}
