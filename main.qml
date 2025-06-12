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

import QtQuick.Window 2.1
import QtQuick 2.5
import IQNotifier 1.0

QtObject {
    id: root
    property var notificationsMap: {'-1': Object} // that's ok
    property var cons: Connections {
        target: IQNotifications
        onCreateNotification: {
            if (notificationsMap[notification_id]) {
//                notificationsMap[notification_id].body = body;
//                notificationsMap[notification_id].title = title;
//                return;
                notification_id.expiration_timeout = 0;
                dropNotification(notification_id);
            }

            var n = root.createNotification(notification_id,
                                            size, pos,
                                            expire_timeout,
                                            appName,
                                            body, title,
                                            iconUrl, actions);
            n.show();
            root.addNotification(notification_id, n);
        }
        onDropNotification:
            root.dropNotification(notification_id);
        onDropAllVisible: root.dropAllVisible()
        onMoveNotification:
            root.moveNotification(notification_id, pos);
    }

    Component.onCompleted: {
        initExtraNotifications();
    }

    function actionsToButtons(actions) {
        var buttons = [];
        for (var i = 0; i < actions.length; i += 2) {
            buttons.push({
                             "action": actions[i],
                             "text": actions[i+1]
                         });
        }
        return buttons;
    }

    function createNotification(notification_id,
                                size, pos,
                                expire_timeout,
                                appName,
                                body,title,
                                iconUrl, actions) {
        var component = Qt.createComponent("IQNotification.qml");
        if (component.status !== Component.Ready) {
            if(component.status === Component.Error)
                console.debug("Error: "+ component.errorString());
            throw "Can't create notification!";
        }
        var options = {
            "notification_id": notification_id,
            "width": size.width,
            "height": size.height,
            "x": pos.x, "y": pos.y,
            "expireTimeout": expire_timeout,
            "appName": appName,
            "body": body,
            "title": title,
            "iconUrl": iconUrl,
            "buttons": actionsToButtons(actions)
        };
        var notification = component.createObject(root,
                                                  options);
        if (notification === null)
            throw "Error creating notification object";
        return notification;
    }

    function addNotification(notification_id, notification) {
        notificationsMap[notification_id] = notification;
    }

    function dropNotification(notification_id) {
        if (notificationsMap[notification_id] !== undefined) {
            notificationsMap[notification_id].drop();
            notificationsMap[notification_id] = undefined;
        }
    }

    function dropAllVisible() {
        Object.keys(notificationsMap).forEach(function(key) {
            if( key != '-1') {
//                IQNotifications.onDropNotification(key);
                dropNotification(key);
            }

        });
    }

    function moveNotification(notification_id, pos) {
        if (notificationsMap[notification_id] !== undefined) {
            notificationsMap[notification_id].move(pos.x, pos.y);
        }
    }

    function initExtraNotifications () {
        var component = Qt.createComponent("IQExtraNotifications.qml");
        if (component.status !== Component.Ready) {
            if(component.status === Component.Error)
                console.debug("Error: "+ component.errorString());
            throw "Can't create extra notifications window!";
        }
        var extraNotifications = component.createObject(root,
                                                  {});
    }

    /* TRAY STUFF */

    property var trayIconLoader__: Loader{
        id: trayIconLoader
        sourceComponent: IQHistory.isEnabled ? tray : undefined
    }

    function createHistoryWindow(cb) {
            var component = Qt.createComponent("IQHistoryWindow.qml");
            if (component.status !== Component.Ready) {
                if(component.status === Component.Error)
                    console.debug("Error: "+ component.errorString());
                throw "Can't create history window!";
            }
            return component.createObject(root, {closeCallback: cb});
    }

    property var tray__: Component{
        id: tray
        IQTrayIcon {
            property IQHistoryWindow history
            iconUrl: IQThemes.trayIconTheme.icon
            visible: iconUrl != undefined
            onLeftClick: {
                if (history == null || history == undefined) {
                    history = createHistoryWindow(function () {
                        history = null;
                    });
                    history.show()
                } else {
                    history.drop();
                    history = null;
                }
            }
        }
    }
}

