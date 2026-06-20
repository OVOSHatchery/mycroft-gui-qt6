/*
 * Copyright 2018 Aditya Mehra <aix.m@outlook.com>
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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import OVOS.GUI 1.0 as OVOS
import Qt5Compat.GraphicalEffects

Kirigami.ScrollablePage {
    id: wallpaperSettings
    title: qsTr("Wallpaper Settings")

    property var currentProvider
    property var currentWallpaper
    property var providersModel
    property var wallpapersProviderCollection
    property bool providerHasCollection
    property bool providerIsConfigurable
    property bool wallpaperRotation: false

    Connections {
        target: OVOS.GuiBusClient
        onIntentRecevied: (type, data) => {
            if (type == "ovos.wallpaper.manager.get.active.provider.response") {
                currentProvider = data.active_provider
            }
            if (type == "ovos.wallpaper.manager.get.registered.providers.response") {
                var model = {"providers": data.registered_providers}
                providersModel = model.providers
                providersComboBox.model = providersModel
            }
            if (type == "ovos.wallpaper.manager.get.wallpaper.response") {
                currentWallpaper = data.url
            }
            if (type == "ovos.wallpaper.manager.get.auto.rotation.response") {
                wallpaperRotation = data.auto_rotation
            }
            if (type == "ovos.wallpaper.manager.get.provider.config.response") {
                configureProviderPopupDialog.providerName = data.provider_name
                configureProviderPopupDialog.providerConfiguration = convertConfigToArray(data.config)
            }
            if (type == "homescreen.wallpaper.set") {
                currentWallpaper = data.url
            }
            if (type == "ovos.phal.wallpaper.manager.provider.registered") {
                getRegisteredProviders()
            }
        }
    }

    function getActiveProvider() {
        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.get.active.provider", {})
    }

    function getRegisteredProviders() {
        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.get.registered.providers", {})
    }

    function getCurrentWallpaper() {
        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.get.wallpaper", {})
    }

    function getAutoRotation() {
        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.get.auto.rotation", {})
    }

    function refreshProvider() {
        var idx = providersComboBox.currentIndex
        if (providersComboBox.model && providersComboBox.model[idx]) {
            wallpapersProviderCollection = providersComboBox.model[idx].wallpaper_collection
            providerIsConfigurable = providersComboBox.model[idx].provider_configurable
            providerHasCollection = wallpapersProviderCollection && wallpapersProviderCollection.length > 0
        } else {
            providerHasCollection = false
        }
    }

    function getProviderConfig() {
        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.get.provider.config", {"provider_name": providersComboBox.currentValue})
    }

    function convertConfigToArray(obj) {
        var result = [];
        for (var key in obj) {
            result.push({ "key": key, "value": obj[key].toLowerCase() });
        }
        return result;
    }

    function convertArrayToObject(keyValueArray) {
        var result = {};
        for (var i = 0; i < keyValueArray.length; i++) {
            var obj = keyValueArray[i];
            result[obj.key] = obj.value;
        }
        return result;
    }

    Component.onCompleted: {
        getActiveProvider()
        getRegisteredProviders()
        getCurrentWallpaper()
        getAutoRotation()
    }

    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        RowLayout {
            id: wallpaperProviderSelector
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4

            Controls.ComboBox {
                id: providersComboBox
                Layout.fillWidth: true
                Layout.fillHeight: true
                textRole: "provider_display_name"
                valueRole: "provider_name"

                onCurrentValueChanged: {
                    refreshProvider()
                }
            }

            Controls.Button {
                id: setProviderButton
                Layout.preferredWidth: Kirigami.Units.gridUnit * 12
                Layout.fillHeight: true
                enabled: providersComboBox.currentValue != currentProvider

                contentItem: RowLayout {
                    Kirigami.Icon {
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                        source: "dialog-ok"
                    }
                    Controls.Label {
                        color: setProviderButton.enabled ? Kirigami.Theme.textColor : Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.5)
                        text: qsTr("Set Provider")
                    }
                }

                onClicked: {
                    OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.set.active.provider", {"provider_name": providersComboBox.currentValue})
                    getActiveProvider()
                    getCurrentWallpaper()
                    refreshProvider()
                }
            }

            Controls.Button {
                id: configureProviderButton
                Layout.preferredWidth: Kirigami.Units.gridUnit * 12
                Layout.fillHeight: true
                enabled: wallpaperSettings.providerIsConfigurable

                contentItem: RowLayout {
                    Kirigami.Icon {
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                        source: "configure"
                    }
                    Controls.Label {
                        color: configureProviderButton.enabled ? Kirigami.Theme.textColor : Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.5)
                        text: qsTr("Configure")
                    }
                }

                onClicked: {
                    getProviderConfig()
                    configureProviderPopupDialog.open()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: Kirigami.Units.gridUnit * 10
            Layout.fillHeight: true
            color: Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.7)
            border.color: Kirigami.Theme.backgroundColor
            border.width: 1
            radius: 4

            Controls.Label {
                id: collectionNotAvailableViewLabel
                anchors.fill: parent
                anchors.margins: Kirigami.Units.gridUnit / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                color: Kirigami.Theme.textColor
                visible: !wallpaperSettings.providerHasCollection
                text: qsTr("This provider generates new wallpapers dynamically instead of wallpaper collections.")
            }

            GridView {
                id: wallpapersView
                anchors.fill: parent
                anchors.margins: Kirigami.Units.gridUnit / 2
                visible: wallpaperSettings.providerHasCollection
                cellWidth: width / 4
                cellHeight: cellWidth * 0.6
                clip: true
                model: wallpaperSettings.wallpapersProviderCollection
                delegate: Controls.ItemDelegate {
                    width: wallpapersView.cellWidth
                    height: wallpapersView.cellHeight
                    padding: 4

                    background: Rectangle {
                        color: "transparent"
                    }

                    contentItem: Rectangle {
                        color: Kirigami.Theme.backgroundColor
                        border.color: currentWallpaper == modelData ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor
                        border.width: 2

                        Image {
                            id: delegateImage
                            anchors.fill: parent
                            anchors.margins: 4
                            source: Qt.resolvedUrl(modelData)
                            fillMode: Image.PreserveAspectCrop
                        }
                    }

                    onClicked: {
                        if(modelData != currentWallpaper) {
                            setWallpaperPopupDialog.open(modelData)
                        }
                    }
                }
            }
        }
    }

    footer: Controls.ToolBar {
        contentItem: RowLayout {
            Controls.ToolButton {
                icon.name: "go-previous"
                text: qsTr("Back")
                onClicked: {
                    OVOS.GuiBusClient.sendRequest("mycroft.device.settings", {})
                }
            }
            Kirigami.Heading {
                level: 2
                text: qsTr("Wallpaper Settings")
                Layout.fillWidth: true
            }
        }
    }

    Controls.Popup {
        id: setWallpaperPopupDialog
        anchors.centerIn: parent
        width: parent.width * 0.8
        height: parent.height * 0.8
        modal: true
        focus: true
        
        property var imageUrl

        function open(url) {
            imageUrl = url
            setWallpaperPopupDialog.opened = true
        }

        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
            radius: 6
            border.color: Kirigami.Theme.highlightColor
            border.width: 1
        }

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.largeSpacing

            Image {
                Layout.fillWidth: true
                Layout.fillHeight: true
                source: Qt.resolvedUrl(setWallpaperPopupDialog.imageUrl)
                fillMode: Image.PreserveAspectFit
            }

            RowLayout {
                visible: wallpaperSettings.wallpaperRotation
                Layout.fillWidth: true
                Kirigami.Icon {
                    source: "state-warning"
                    Layout.preferredWidth: Kirigami.Units.iconSizes.small
                    Layout.preferredHeight: Kirigami.Units.iconSizes.small
                }
                Controls.Label {
                    text: qsTr("Wallpaper rotation is enabled. Disable it to set a static wallpaper.")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Controls.Button {
                    text: qsTr("Set Wallpaper")
                    enabled: !wallpaperSettings.wallpaperRotation
                    onClicked: {
                        if(currentProvider == providersComboBox.currentValue) {
                            OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.set.wallpaper", {"url": setWallpaperPopupDialog.imageUrl})
                        } else {
                            OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.set.active.provider", {"provider_name": providersComboBox.currentValue, "provider_image": setWallpaperPopupDialog.imageUrl})
                        }
                        setWallpaperPopupDialog.close()
                    }
                }
                Controls.Button {
                    text: qsTr("Cancel")
                    onClicked: setWallpaperPopupDialog.close()
                }
            }
        }
    }

    Controls.Popup {
        id: configureProviderPopupDialog
        anchors.centerIn: parent
        width: parent.width * 0.8
        height: parent.height * 0.8
        modal: true
        focus: true

        property var providerName
        property var providerConfiguration
        property var newConfiguration: []

        function close() {
            providerConfiguration = {}
            configureProviderPopupDialog.opened = false
        }

        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
            radius: 6
            border.color: Kirigami.Theme.highlightColor
            border.width: 1
        }

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.largeSpacing

            Kirigami.Heading {
                text: qsTr("Configure Provider: ") + configureProviderPopupDialog.providerName
                level: 2
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: configureProviderPopupDialog.providerConfiguration
                delegate: RowLayout {
                    width: parent.width
                    Controls.Label {
                        text: modelData.key
                        Layout.preferredWidth: parent.width * 0.4
                    }
                    Controls.TextField {
                        text: modelData.value
                        Layout.fillWidth: true
                        onTextChanged: {
                            var keyExists = false
                            for (var i = 0; i < configureProviderPopupDialog.newConfiguration.length; i++) {
                                if (configureProviderPopupDialog.newConfiguration[i].key === modelData.key) {
                                    configureProviderPopupDialog.newConfiguration[i].value = text
                                    keyExists = true
                                }
                            }
                            if (!keyExists) {
                                configureProviderPopupDialog.newConfiguration.push({
                                    "key": modelData.key,
                                    "value": text
                                })
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Controls.Button {
                    text: qsTr("Apply")
                    onClicked: {
                        var config = convertArrayToObject(configureProviderPopupDialog.newConfiguration)
                        OVOS.GuiBusClient.sendRequest("ovos.wallpaper.manager.set.provider.config", 
                            {"provider_name": configureProviderPopupDialog.providerName, "config": config}) 
                        configureProviderPopupDialog.close()
                    }
                }
                Controls.Button {
                    text: qsTr("Cancel")
                    onClicked: configureProviderPopupDialog.close()
                }
            }
        }
    }
}
