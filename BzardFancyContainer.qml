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
import QtQuick.Layouts

Item {
    id: root

    property real shownOpacity: 0.98

    property int showDuration: 120
    property real showScaleDivisor: 1.5

    property int dropDuration: 120
    property real dropScaleDivisor: 1.3

    property alias color: bg.color
    property alias bgImageSource: bgImage.source

    /* SIGNALS */

    signal closeClicked()

    /* FUNCTIONS */

    function animateShow() {
        showAnimation.start();
    }

    function animateDrop() {
        dropAnimation.start();
    }

    /* COMPONENTS */

    Rectangle {
        id: bg
        anchors.fill: parent
        Image {
            id: bgImage
            anchors.fill: parent
            visible: source != undefined
        }
    }

    ParallelAnimation {
        id: showAnimation
        PropertyAnimation {
            target: root; property: "scale"; from: 1/showScaleDivisor; to: 1
            duration: showDuration
        }
        PropertyAnimation {
            target: root; property: "opacity"; from: 0; to: shownOpacity
            duration: showDuration
        }
    }

    ParallelAnimation {
        id: dropAnimation
        PropertyAnimation {
            target: root; property: "scale"; to: 1/dropScaleDivisor
            duration: dropDuration
        }
        PropertyAnimation {
            target: root.parent; property: "y";
            to: root.parent.y - (root.height -root.height/dropScaleDivisor)
            duration: dropDuration
        }
        PropertyAnimation {
            target: root; property: "opacity"; from: shownOpacity; to: 0
            duration: dropDuration
        }
    }
}
