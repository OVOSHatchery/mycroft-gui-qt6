/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
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
import QtQuick.Controls
import QtQuick.Window
import org.kde.kirigami as Kirigami

Label {
    id: root
    
    Layout.alignment: Qt.AlignCenter

    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    fontSizeMode: Text.Fit
    minimumPixelSize: 5
    font.pixelSize: 250

    renderType: height > 40
        ? Text.QtRendering
        : (Screen.devicePixelRatio % 1 !== 0 ? Text.QtRendering : Text.NativeRendering)

    Timer {
        id: sizeModeChangeTimer
        interval: 50
        running: false
        repeat: false

        onTriggered: {
            if (fontSizeMode == 1) {
                if(paintedHeight > root.height) {
                    // Text Height is bigger than implicit height switch font size mode to fit
                    root.fontSizeMode = 3
                    opacity = 1
                } else {
                    opacity = 1
                }
            }
        }
    }

    onTextChanged: {
        var wordlist = text.split(" ")
        var wordcount = text.split(" ").length
        if(wordcount <= 1) {
            fontSizeMode = Text.HorizontalFit
            opacity = 0
            // Do text height check if we are in horizontal fit mode to make sure text is not bigger than available height
            sizeModeChangeTimer.start()
        } else {
            // Check how many characters are in the longest word in the wordlist
            var longestWord = wordlist.reduce(function(a, b) {
                return a.length > b.length ? a : b;
            });
            var longestWordLength = longestWord.length

            // If char count in longest word less than 2 sylabels, use horizontal fit
            if(longestWordLength < 2) {
                fontSizeMode = Text.HorizontalFit   
            } else {
                fontSizeMode = Text.VerticalFit
            }
        }
    }
}