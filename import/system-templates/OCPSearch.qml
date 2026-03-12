import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string searchTerm: sessionData.search_term || ""
    property var    results:    sessionData.results      || []
    property var    skillCards: sessionData.skill_cards  || []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: searchTerm.length > 0 ? qsTr("Results for \"%1\"").arg(searchTerm)
                                            : qsTr("Search Results")
                font.pixelSize: 20
                font.weight: Font.DemiBold
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        // Skill source chips (horizontal scroll)
        ListView {
            visible: skillCards.length > 0
            Layout.fillWidth: true
            height: 40
            orientation: ListView.Horizontal
            spacing: 8
            clip: true
            model: skillCards

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
            model: results
            spacing: 2

            delegate: ItemDelegate {
                width: parent.width

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

                onClicked: triggerEvent("search.play", {
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
