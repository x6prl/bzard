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

#pragma once

#include <vector>

#include <QObject>
#include <QString>
#include <QStringList>

#include "bzard_config.h"
#include "bzard_notification_receiver.h"

class BzardDBusService : public QObject {
	Q_OBJECT

  public:
	static QString versionString();
	static QString appString();

	using QObject::QObject;

	BzardDBusService *connectReceiver(BzardNotificationReceiver *receiver);

	template <class T>
	typename std::enable_if_t<std::is_base_of<BzardConfigurable, T>::value,
	                          BzardDBusService *>
	addModifier(std::unique_ptr<T> modifier) {
		if (modifier->isEnabled())
			modifers.push_back(std::move(modifier));
		return this;
	}

	template <class T>
	typename std::enable_if_t<!std::is_base_of<BzardConfigurable, T>::value,
	                          BzardDBusService *>
	addModifier(std::unique_ptr<T> modifier) {
		modifers.push_back(std::move(modifier));
		return this;
	}

	// DBus interface
	QStringList GetCapabilities();

	QString GetServerInformation(QString &vendor, QString &version,
	                             QString &specVersion);

	uint32_t Notify(const QString &appName, uint32_t replacesId,
	                const QString &appIcon, const QString &summary,
	                const QString &body, const QStringList &actions,
	                const QVariantMap &hints, uint32_t expireTimeout);

	void CloseNotification(uint32_t id);

  signals:
	// DBus signals
	void actionInvoked(uint32_t notificationId, const QString &actionKey);

	void notificationClosed(uint32_t notificationId, uint32_t reason);

	// Internal signals
	void createNotificationSignal(const BzardNotification &notification);
	void dropNotificationSignal(BzardNotification::IdT id);

  public slots:
	void onNotificationDropped(BzardNotification::IdT id,
	                           BzardNotification::ClosingReason reason);
	void onActionInvoked(BzardNotification::IdT id, const QString &actionKey);

  private:
	std::vector<BzardNotificationModifier::PtrT> modifers;

	BzardNotification modify(BzardNotification notification);
};
