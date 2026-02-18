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

#include "bzard_dbus_service.h"

#include "bzard_config.h"

QString BzardDBusService::versionString() {
	return BzardConfig::applicationVersion();
}

QString BzardDBusService::appString() { return BzardConfig::applicationName(); }

BzardDBusService *
BzardDBusService::connectReceiver(BzardNotificationReceiver *receiver) {
	connect(this, &BzardDBusService::createNotificationSignal, receiver,
	        &BzardNotificationReceiver::onCreateNotification);
	connect(this, &BzardDBusService::dropNotificationSignal, receiver,
	        &BzardNotificationReceiver::onDropNotification);
	connect(receiver, &BzardNotificationReceiver::actionInvokedSignal, this,
	        &BzardDBusService::onActionInvoked);
	connect(receiver, &BzardNotificationReceiver::notificationDroppedSignal,
	        this, &BzardDBusService::onNotificationDropped);
	return this;
}

QStringList BzardDBusService::GetCapabilities() {
	auto capabilities = QStringList{} << "actions"
	                                  // << "action-icons"
	                                  << "body"
	                                  // << "body-hyperlinks"
	                                  // << "body-images"
	                                  << "body-markup"
	                                  // << "icon-multi"
	                                  << "icon-static"
	                                  << "persistence"
		  // << "sound"
		  ;
	return capabilities;
}

QString BzardDBusService::GetServerInformation(QString &vendor,
                                               QString &version,
                                               QString &specVersion) {
	specVersion = QString("1.2");
	version = versionString();
	vendor = QString("bzard.project");
	return QString("bzard");
}

uint32_t BzardDBusService::Notify(const QString &appName, uint32_t replacesId,
                                  const QString &appIcon,
                                  const QString &summary, const QString &body,
                                  const QStringList &actions,
                                  const QVariantMap &hints,
                                  uint32_t expireTimeout) {
	auto notification =
		  modify({replacesId, appName, body, summary, appIcon, actions, hints,
	              static_cast<BzardNotification::ExpireTimeout>(expireTimeout),
	              replacesId});
	emit createNotificationSignal(notification);
	return notification.id;
}

void BzardDBusService::CloseNotification(uint32_t id) {
	emit dropNotificationSignal(id);
}

void BzardDBusService::onNotificationDropped(
	  BzardNotification::IdT id, BzardNotification::ClosingReason reason) {
	emit notificationClosed(id, reason);
}

void BzardDBusService::onActionInvoked(BzardNotification::IdT id,
                                       const QString &actionKey) {
	emit actionInvoked(id, actionKey);
}

BzardNotification BzardDBusService::modify(BzardNotification notification) {
	qDebug() << "========*****========";
	qDebug() << notification;
	for (auto &m : modifers)
		m->modify(notification);
	qDebug() << notification;
	return notification;
}
