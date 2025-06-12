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
import IQNotifier 1.0

IQPopup {
    id: root
    dropDuration: container.dropDuration

    property int notification_id: 0
    property alias appName: container.appName
    property alias body: container.body
    property alias iconUrl: container.iconUrl
    property alias buttons: container.buttons
    property alias expireTimeout: expiration_controller.timeout

    IQExpirationController{
        id: expiration_controller
        onExpired: IQNotifications.onExpired(notification_id)
        expiration: alive && !mouseArea.containsMouse
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
        onClicked: {
            var rightPressed = mouse.button & Qt.RightButton;
            if (rightPressed && IQNotifications.closeAllByRightClick) {
                IQNotifications.onDropAll()
            }
            var middlePressed = mouse.button & Qt.MiddleButton;
            if (middlePressed && IQNotifications.closeVisibleByLeftClick) {
                return IQNotifications.onDropVisible()
            }
            var leftPressed = mouse.button & Qt.LeftButton;
            if (leftPressed && IQNotifications.closeByLeftClick) {
                return IQNotifications.onCloseButtonPressed(notification_id)
            }
        }
    }

    IQNotificationContainer {
        id: container
        referenceHeight: root.height
        expireTimeout: expiration_controller.timeout - showDuration
        expiration: expiration_controller.expiration
        title: root.title
        anchors.fill: parent
        onCloseClicked: IQNotifications.onCloseButtonPressed(notification_id)
        onButtonClicked: IQNotifications.onActionButtonPressed(notification_id, button)
    }
}
