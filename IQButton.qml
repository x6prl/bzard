/*
 *     This file is part of IQ Notifier.
 *
 * IQ Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * IQ Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IQ Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.5

Rectangle {
    id: root
    radius: height/5
    height: 64
    width: 128

    property alias text: buttonText.text
    property alias textColor: buttonText.color

    signal clicked()

    Text {
        id: buttonText
        color: "#ffffff"
        font.pointSize: height/4
        anchors.fill: parent
        wrapMode: Text.WrapAnywhere
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
        hoverEnabled: true
    }
    states: [
        State {
            name: "hovered"; when: mouseArea.containsMouse && !mouseArea.pressed
            PropertyChanges { target: root; opacity: 0.9 }
        },
        State {
            name: "pressed"; when: mouseArea.containsMouse && mouseArea.pressed
            PropertyChanges { target: buttonText; opacity: 0.5 }
        }
    ]
}
