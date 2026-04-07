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
import QtQuick.Window
import bzard 1.0

Window {
    id: root
    visible: false
    color: "transparent"
    // flags: Qt.Popup
    flags: Qt.FramelessWindowHint | Qt.Tool | Qt.WindowDoesNotAcceptFocus | Qt.BypassWindowManagerHint
    property string layer: "notifications"
    property int popupX: x
    property int popupY: y
    property int popupWidth: width
    property int popupHeight: height

    property alias dropDuration: destroyTimer.interval
    property int moveDuration: dropDuration
    property bool alive: false

    property int _newX: popupX
    property int _newY: popupY

    /* FUNCTIONS */

    function applyPopupGeometry() {
        if (x !== popupX)
            x = popupX;
        if (y !== popupY)
            y = popupY;
        if (width !== popupWidth)
            width = popupWidth;
        if (height !== popupHeight)
            height = popupHeight;
    }

    function configureWaylandLayerShell(targetX, targetY, targetWidth, targetHeight) {
        const effectiveX = (targetX === undefined) ? popupX : targetX;
        const effectiveY = (targetY === undefined) ? popupY : targetY;
        const effectiveWidth = (targetWidth === undefined) ? popupWidth : targetWidth;
        const effectiveHeight = (targetHeight === undefined) ? popupHeight : targetHeight;
        BzardWaylandLayerShell.configure(
                    root,
                    layer,
                    effectiveX, effectiveY,
                    effectiveWidth, effectiveHeight);
    }

    function syncGeometryAndConfigureIfNeeded() {
        applyPopupGeometry();
        if (alive || visible)
            configureWaylandLayerShell();
    }

    function show() {
        alive = true;
        applyPopupGeometry();
        configureWaylandLayerShell();
        visible = true;
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

    onPopupXChanged: syncGeometryAndConfigureIfNeeded()
    onPopupYChanged: syncGeometryAndConfigureIfNeeded()
    onPopupWidthChanged: syncGeometryAndConfigureIfNeeded()
    onPopupHeightChanged: syncGeometryAndConfigureIfNeeded()
    onLayerChanged: syncGeometryAndConfigureIfNeeded()
    onVisibleChanged: {
        if (visible)
            syncGeometryAndConfigureIfNeeded();
    }
    Component.onCompleted: {
        applyPopupGeometry();
        if (visible)
            configureWaylandLayerShell();
    }

    Timer {
        id: destroyTimer
        running: false
        repeat: false
        onTriggered: {
            root.close()
            root.destroy()
        }
    }

    ParallelAnimation {
        id: moveAnimation
        PropertyAnimation {
            target: root; property: "popupX"; to: _newX
            duration: moveDuration
        }
        PropertyAnimation {
            target: root; property: "popupY"; to: _newY
            duration: moveDuration
        }
    }
}
