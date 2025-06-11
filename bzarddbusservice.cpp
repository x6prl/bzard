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

#include "bzarddbusservice.h"

#include "bzardconfig.h"

QString BzardDBusService::versionString()
{
	return IQConfig::applicationVersion();
}

QString BzardDBusService::appString() { return IQConfig::applicationName(); }

BzardDBusService *
BzardDBusService::connectReceiver(IQNotificationReceiver *receiver)
{
	connect(this, &BzardDBusService::createNotificationSignal, receiver,
		&IQNotificationReceiver::onCreateNotification);
	connect(this, &BzardDBusService::dropNotificationSignal, receiver,
		&IQNotificationReceiver::onDropNotification);
	connect(receiver, &IQNotificationReceiver::actionInvokedSignal, this,
		&BzardDBusService::onActionInvoked);
	connect(receiver, &IQNotificationReceiver::notificationDroppedSignal,
		this, &BzardDBusService::onNotificationDropped);
	return this;
}

QStringList BzardDBusService::GetCapabilities()
{
	auto caps = QStringList{} << "actions"
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
	return caps;
}

QString BzardDBusService::GetServerInformation(QString &vendor, QString &version,
					    QString &spec_version)
{
	spec_version = QString("1.2");
	version = versionString();
	vendor = QString("viktor.filnkov");
	return QString("iq-notifier");
}

uint32_t BzardDBusService::Notify(const QString &app_name, uint32_t replaces_id,
			       const QString &app_icon, const QString &summary,
			       const QString &body, const QStringList &actions,
			       const QVariantMap &hints,
			       uint32_t expire_timeout)
{
	auto notification = modify(
	    {replaces_id, app_name, body, summary, app_icon, actions, hints,
	     static_cast<IQNotification::ExpireTimeout>(expire_timeout),
	     replaces_id});
	emit createNotificationSignal(notification);
	return notification.id;
}

void BzardDBusService::CloseNotification(uint32_t id)
{
	emit dropNotificationSignal(id);
}

void BzardDBusService::onNotificationDropped(IQNotification::id_t id,
					  IQNotification::ClosingReason reason)
{
	emit NotificationClosed(id, reason);
}

void BzardDBusService::onActionInvoked(IQNotification::id_t id,
				    const QString &action_key)
{
	emit ActionInvoked(id, action_key);
}

IQNotification BzardDBusService::modify(IQNotification notification)
{
    qDebug() << "========*****========";
    qDebug() << notification;
	for (auto &m : modifers)
		m->modify(notification);
    qDebug() << notification;
	return notification;
}
