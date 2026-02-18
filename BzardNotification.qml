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

BzardPopup {
    id: root
    dropDuration: container.dropDuration

    property int notification_id: 0
    property alias appName: container.appName
    property alias body: container.body
    property alias iconUrl: container.iconUrl
    property alias buttons: container.buttons
    property alias expireTimeout: expiration_controller.timeout

    BzardExpirationController{
        id: expiration_controller
        onExpired: BzardNotifications.onExpired(notification_id)
        expiration: alive && !mouseArea.containsMouse
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
        function onClicked (mouse) {
            var rightPressed = mouse.button & Qt.RightButton;
            if (rightPressed && BzardNotifications.closeAllByRightClick) {
                BzardNotifications.onDropAll()
            }
            var middlePressed = mouse.button & Qt.MiddleButton;
            if (middlePressed && BzardNotifications.closeVisibleByLeftClick) {
                return BzardNotifications.onDropVisible()
            }
            var leftPressed = mouse.button & Qt.LeftButton;
            if (leftPressed && BzardNotifications.closeByLeftClick) {
                return BzardNotifications.onCloseButtonPressed(notification_id)
            }
        }
    }

    BzardNotificationContainer {
        id: container
        referenceHeight: root.height
        expireTimeout: expiration_controller.timeout - showDuration
        expiration: expiration_controller.expiration
        title: root.title
        anchors.fill: parent
        onCloseClicked: BzardNotifications.onCloseButtonPressed(notification_id)
        onButtonClicked: BzardNotifications.onActionButtonPressed(notification_id, button)
    }
}
