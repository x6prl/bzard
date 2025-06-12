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
    height: IQThemes.historyWindowTheme.height ?
                IQThemes.historyWindowTheme.height :
                calcHeight
    width: IQThemes.historyWindowTheme.width ?
               IQThemes.historyWindowTheme.width :
               calcWidth
    x: IQThemes.historyWindowTheme.x
    y: IQThemes.historyWindowTheme.y

    property var closeCallback: function () {
        console.log("No close callback provided!")
    }

    property int calcHeight: Screen.height
    property int calcWidth: Screen.desktopAvailableWidth / 4

    property int barHeight: 32

    IQFancyContainer {
        id: container
        anchors.fill: parent

        color: IQThemes.historyWindowTheme.bgColor
        bgImageSource: IQThemes.historyWindowTheme.bgImage


        IQNotificationBar {
            id: bar
            z: 1
            visible: height
            height: IQThemes.historyWindowTheme.barHeight
            closeButtonImageSource: IQThemes.historyWindowTheme.closeIcon
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: IQThemes.historyWindowTheme.barBgColor
            text: IQThemes.historyWindowTheme.windowTitle
            textFontSize: IQThemes.historyWindowTheme.barFontSize
            textColor: IQThemes.historyWindowTheme.barTextColor
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

            model: IQHistory.model
            delegate: IQHistoryNotification{
                height: IQThemes.historyWindowTheme.notificationHeight ?
                            IQThemes.historyWindowTheme.notificationHeight : 70
                color: IQThemes.historyWindowTheme.nbgColor
                appColor: IQThemes.historyWindowTheme.nappTextColor
                titleColor: IQThemes.historyWindowTheme.ntitleTextColor
                bodyColor: IQThemes.historyWindowTheme.nbodyTextColor

                appSize: IQThemes.historyWindowTheme.nappFontSize?
                             IQThemes.historyWindowTheme.nappFontSize :
                             height * 0.08333333333333333333
                titleSize: IQThemes.historyWindowTheme.ntitleFontSize ?
                               IQThemes.historyWindowTheme.ntitleFontSize :
                               height * 0.125
                bodySize: IQThemes.historyWindowTheme.nbodyFontSize ?
                              IQThemes.historyWindowTheme.nbodyFontSize :
                              height * 0.11111111111111111111
                onRemoveNotification: IQHistory.remove(index)
            }
        }
    }
}
