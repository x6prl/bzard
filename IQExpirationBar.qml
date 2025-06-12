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
    width: initialWidth

    property bool runnig: false
    property int expireTimeout: 0
    property int initialWidth: parent.width

    function restart() {
        anim.restart();
    }

    visible: runnig

    PropertyAnimation {
        id: anim
        target: root; property: "width"; from: initialWidth; to: 0
        duration: expireTimeout < 0 ? 0 : expireTimeout
        running: root.runnig && expireTimeout > 0
    }
}
