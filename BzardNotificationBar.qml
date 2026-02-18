/*
 *     This file is part of bzard.
 *
 * bzard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * bzard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bzard.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick

Item {
    id: root
    signal closeClicked()

    property real elementsScale: 0.4
    property alias color: bg.color
    property alias closeButtonImageSource: closeImage.source
    property alias textColor: barText.color
    property alias textFontSize: barText.font.pointSize
    property string text: ""

    Rectangle {
        id: bg
        anchors.fill: parent
    }

    Text {
        id: barText
        text: root.text.length === 0 ? qsTr("Notification") : root.text
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors.fill: parent
        anchors.leftMargin: closeImage.width*0.4
    }

    MouseArea {
        id: closeBtn
        height: root.height
        width: height
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        hoverEnabled: true

        Image {
            id: closeImage
            scale: elementsScale
            anchors.fill: parent
            opacity: parent.containsMouse ? 0.85 : 1
        }
        onClicked: closeClicked()
    }

}
