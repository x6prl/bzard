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
    height: BzardThemes.historyWindowTheme.height ?
                BzardThemes.historyWindowTheme.height :
                calcHeight
    width: BzardThemes.historyWindowTheme.width ?
               BzardThemes.historyWindowTheme.width :
               calcWidth
    x: BzardThemes.historyWindowTheme.x
    y: BzardThemes.historyWindowTheme.y

    function onFocusObjectChanged () {
        console.log("debug completed")
        // console.log(focus)
    }

    property var closeCallback: function () {
        console.log("No close callback provided!")
    }

    property int calcHeight: Screen.height
    property int calcWidth: Screen.desktopAvailableWidth / 4

    property int barHeight: 32

    BzardFancyContainer {
        id: container
        anchors.fill: parent

        color: BzardThemes.historyWindowTheme.bgColor
        bgImageSource: BzardThemes.historyWindowTheme.bgImage


        BzardNotificationBar {
            id: bar
            z: 1
            visible: height
            height: BzardThemes.historyWindowTheme.barHeight
            closeButtonImageSource: BzardThemes.historyWindowTheme.closeIcon
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: BzardThemes.historyWindowTheme.barBgColor
            text: BzardThemes.historyWindowTheme.windowTitle
            textFontSize: BzardThemes.historyWindowTheme.barFontSize
            textColor: BzardThemes.historyWindowTheme.barTextColor
            onCloseClicked: {
                closeCallback();
                root.drop();
            }
        }

        ListView {
            id: listView
            highlightFollowsCurrentItem: false
            focus: true
            anchors.top: bar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            model: BzardHistory.model
            delegate: BzardHistoryNotification {
                height: BzardThemes.historyWindowTheme.notificationHeight ?
                            BzardThemes.historyWindowTheme.notificationHeight : 70
                color: BzardThemes.historyWindowTheme.nbgColor
                appColor: BzardThemes.historyWindowTheme.nappTextColor
                titleColor: BzardThemes.historyWindowTheme.ntitleTextColor
                bodyColor: BzardThemes.historyWindowTheme.nbodyTextColor

                appSize: BzardThemes.historyWindowTheme.nappFontSize?
                             BzardThemes.historyWindowTheme.nappFontSize :
                             height * 0.08333333333333333333
                titleSize: BzardThemes.historyWindowTheme.ntitleFontSize ?
                               BzardThemes.historyWindowTheme.ntitleFontSize :
                               height * 0.125
                bodySize: BzardThemes.historyWindowTheme.nbodyFontSize ?
                              BzardThemes.historyWindowTheme.nbodyFontSize :
                              height * 0.11111111111111111111
                // TODO: qt.qml.context: qrc:/BzardHistoryWindow.qml:102:17
                // Parameter "index" is not declared.
                // Injection of parameters into signal handlers is deprecated.
                // Use JavaScript functions with formal parameters instead.
                onRemoveNotification: BzardHistory.remove(index)
            }
        }
    }
}
