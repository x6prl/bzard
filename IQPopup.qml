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
import QtQuick.Window 2.1

Window {
    id: root
    visible: true
    color: "transparent"
    flags: Qt.Popup

    property alias dropDuration: destroyTimer.interval
    property int moveDuration: dropDuration
    property bool alive: false

    property int _newX: x
    property int _newY: y

    /* FUNCTIONS */

    function show() {
        alive = true;
        container.animateShow();
    }

    function drop() {
        alive = false;
        container.animateDrop();
        destroyTimer.start();
    }

    function move(newX, newY) {
        _newX = newX;
        _newY = newY;
        moveAnimation.start();
    }

    /* COMPONENTS */

    Timer {
        id: destroyTimer
        running: false
        repeat: false
        onTriggered: close()
    }

    ParallelAnimation {
        id: moveAnimation
        PropertyAnimation {
            target: root; property: "x"; to: _newX
            duration: moveDuration
        }
        PropertyAnimation {
            target: root; property: "y"; to: _newY
            duration: moveDuration
        }
    }
}
