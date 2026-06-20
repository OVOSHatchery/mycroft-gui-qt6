import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OVOS.GUI 1.0 as OVOS

OVOS.Page {
    id: root

    property string searchTerm: namespaceData ? (namespaceData.search_term || "") : ""
    property var    results:    namespaceData ? (namespaceData.results      || []) : []
    property var    skillCards: namespaceData ? (namespaceData.skill_cards  || []) : []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: root.searchTerm.length > 0 ? qsTr("Results for \"%1\"").arg(root.searchTerm)
                                                 : qsTr("Search Results")
                font.pixelSize: 20
                font.weight: Font.DemiBold
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        // Skill source chips (horizontal scroll)
        ListView {
            visible: root.skillCards && root.skillCards.length > 0
            Layout.fillWidth: true
            height: 40
            orientation: ListView.Horizontal
            spacing: 8
            clip: true
            model: root.skillCards

            delegate: Button {
                height: 36
                text: modelData.name || modelData.skill_id || ""
                font.pixelSize: 13
                leftPadding: 10; rightPadding: 10
            }
        }

        // Results list
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.results
            spacing: 2

            delegate: ItemDelegate {
                width: ListView.view.width

                contentItem: RowLayout {
                    spacing: 12

                    Image {
                        visible: (modelData.image || "").length > 0
                        source: modelData.image || ""
                        width: 48; height: 48
                        fillMode: Image.PreserveAspectCrop
                        smooth: true
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: modelData.title || ""
                            font.pixelSize: 16
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                        Label {
                            visible: (modelData.artist || "").length > 0
                            text: modelData.artist || ""
                            font.pixelSize: 13
                            opacity: 0.7
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    Label {
                        text: modelData.media_type || ""
                        font.pixelSize: 11
                        opacity: 0.5
                    }
                }

                onClicked: root.triggerGuiEvent("search.play", {
                    "playlistData": {
                        "uri":        modelData.uri        || "",
                        "title":      modelData.title      || "",
                        "artist":     modelData.artist     || "",
                        "image":      modelData.image      || "",
                        "media_type": modelData.media_type || "",
                        "skill_id":   modelData.skill_id   || ""
                    }
                })
            }
        }
    }
}
