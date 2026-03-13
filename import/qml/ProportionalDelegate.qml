/*
 * Copyright 2019 by Marco Martin <mart@kde.org>
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
import OVOS 1.0 as OVOS

// DEPRECATED: Do not use for new skills. Use template-based system instead.
// See docs/QML_AUDIT_AND_MIGRATION.md for migration guidance.
Delegate {
    id: root

    property int unitDensity: 100
    property real proportionalGridUnit: Math.min(root.contentWidth, root.contentHeight) / unitDensity
    property int spacing: proportionalGridUnit * 10
    property alias enforceUniformSizes: layout.enforceUniformSizes

    leftPadding: Math.min(root.width, root.height)/15
    topPadding: Math.min(root.width, root.height)/15
    rightPadding: Math.min(root.width, root.height)/15
    bottomPadding: Math.min(root.width, root.height)/15 + root.parent.extraBottomPadding

    //contentItemAutoHeight: false
    contentItem: OVOS.BoxLayout {
        id: layout

        // Design Decision: Use proportional spacing (root.spacing) instead of zero
        // Proportional spacing scales with screen size and ensures consistent visual
        // relationships between UI elements across different device form factors.
        rowSpacing: root.spacing
        columnSpacing: root.spacing
    }
}
