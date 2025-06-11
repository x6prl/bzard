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
    visible: BzardNotifications.extraNotifications > 0
    x: BzardNotifications.extraWindowPos.x
    y: BzardNotifications.extraWindowPos.y
    width: BzardNotifications.extraWindowSize.width
    height: BzardNotifications.extraWindowSize.height

    property real buttonImageScale: BzardThemes.notificationsTheme.extraButtonImageScale
    property real unreadCircleScale: 0.6
    property color bgColor: BzardThemes.notificationsTheme.extraBgColor

    BzardFancyContainer {
        id: iQFancyContainer
        anchors.fill: parent
        Rectangle {
            id: bg
            color: bgColor
            anchors.fill: parent
        }
        Rectangle {
            id: circle
            scale: unreadCircleScale
            color: BzardThemes.notificationsTheme.extraUreadCircleColor
            height: parent.height
            width: height
            radius: width
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            Text {
                id: count
                color: BzardThemes.notificationsTheme.extraUreadTextColor
                font.pointSize: parent.height/2.5
                text: BzardNotifications.extraNotifications
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            id: closeVisible
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: closeExtra.left
            height: root.height
            anchors.rightMargin: 0
            width: height
            hoverEnabled: true
            Image {
                scale: buttonImageScale
                anchors.fill: parent
                source: BzardThemes.notificationsTheme.extraCloseVisibleButtonImage
                opacity: parent.containsMouse ? 0.85 : 1
            }
            onClicked: BzardNotifications.onDropVisible()
        }

        MouseArea {
            id: closeExtra
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: closeAll.left
            height: root.height
            anchors.rightMargin: 0
            width: height
            hoverEnabled: true
            Image {
                scale: buttonImageScale
                anchors.fill: parent
                source: BzardThemes.notificationsTheme.extraCloseButtonImage
                opacity: parent.containsMouse ? 0.85 : 1
            }
            onClicked: BzardNotifications.onDropStacked()
        }

        MouseArea {
            id: closeAll
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            width: height
            hoverEnabled: true
            Image {
                scale: buttonImageScale
                anchors.fill: parent
                source: BzardThemes.notificationsTheme.extraCloseAllButtonImage
                opacity: parent.containsMouse ? 0.85 : 1
            }
            onClicked: BzardNotifications.onDropAll()
        }
    }
}
