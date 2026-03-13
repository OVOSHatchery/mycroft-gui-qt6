/*
 * Copyright 2018 Marco Martin <mart@kde.org>
 * Copyright 2018 by Aditya Mehra <aix.m@outlook.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami
import OVOS 1.0 as OVOS

Kirigami.Page {
    title: "Hints"
    objectName: "hints"
    property var hintsModel: []
    property var filteredModel: []

    /*
     * PROTOCOL: Hints are requested from ovos-core via the message bus.
     *
     * CLIENT → SERVER:
     *   type: "gui.hints.get"
     *   data: {}
     *
     * SERVER → CLIENT:
     *   type: "gui.hints.get.response"
     *   data: {
     *     "hints": [
     *       {
     *         "title": "Weather Skill",
     *         "image": "https://...",
     *         "category": "Daily",
     *         "examples": ["What's the weather?", "Will it rain tomorrow?"]
     *       },
     *       ...
     *     ]
     *   }
     */

    Component.onCompleted: {
        OVOS.OVOSController.sendRequest("gui.hints.get", {})
    }

    Connections {
        target: OVOS.OVOSController
        function onSocketMessageReceived(type, data) {
            if (type === "gui.hints.get.response") {
                var hints = data["hints"]
                if (hints && hints.length > 0) {
                    hintsModel = hints
                    filteredModel = hints
                }
            }
        }
    }

    function filterModel(text) {
        var result = []
        for (var i = 0; i < hintsModel.length; i++) {
            var obj = hintsModel[i];
            var searchText = text.toLowerCase()
            if (obj.title.toLowerCase().includes(searchText)
                || obj.category.toLowerCase().includes(searchText)
                || obj.examples.join(" ").toLowerCase().includes(searchText)) {
                result.push(obj)
            }
        }
        return result;
    }

    ColumnLayout {
        anchors.fill: parent

        QQC2.TextField {
            id: filterHints
            placeholderText: qsTr("Search:")
            Layout.fillWidth: true
            onTextChanged: {
                filteredModel = filterModel(text)
            }
        }

        Kirigami.ScrollablePage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Kirigami.CardsListView {
                id: skillslistmodelview
                clip: true;
                model: filteredModel
                anchors.fill: parent

                delegate: HintsDelegate {
                    imageSource: modelData.image || ""
                    title: modelData.title || ""
                    examples: modelData.examples || []
                    category: modelData.category || ""
                }
            }
        }
    }
}
