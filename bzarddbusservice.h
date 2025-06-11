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

#include "bzardconfig.h"
#include "bzardnotificationreceiver.h"

class BzardDBusService : public QObject
{
	Q_OBJECT

      public:
	static QString versionString();
	static QString appString();

	using QObject::QObject;

    BzardDBusService *
	connectReceiver(IQNotificationReceiver *receiver);

	/*
	 * For configurable modifiers we should check is it enabled in config
	 */
	template <class T>
	typename std::enable_if_t<std::is_base_of<IQConfigurable, T>::value,
                  BzardDBusService *>
	addModifier(std::unique_ptr<T> modifier)
	{
		if (modifier->isEnabled())
			modifers.push_back(std::move(modifier));
		return this;
	}

	template <class T>
	typename std::enable_if_t<!std::is_base_of<IQConfigurable, T>::value,
                  BzardDBusService *>
	addModifier(std::unique_ptr<T> modifier)
	{
		modifers.push_back(std::move(modifier));
		return this;
	}

	// DBus interface
	QStringList GetCapabilities();

	QString GetServerInformation(QString &vendor, QString &version,
				     QString &spec_version);

	uint32_t Notify(const QString &app_name, uint32_t replaces_id,
			const QString &app_icon, const QString &summary,
			const QString &body, const QStringList &actions,
			const QVariantMap &hints, uint32_t expire_timeout);

	void CloseNotification(uint32_t id);

      signals:
	// DBus signals
	void ActionInvoked(uint32_t notification_id, const QString &action_key);

	void NotificationClosed(uint32_t notification_id, uint32_t reason);

	// Internal signals
	void createNotificationSignal(const IQNotification &notification);
	void dropNotificationSignal(IQNotification::id_t id);

      public slots:
	void onNotificationDropped(IQNotification::id_t id,
				   IQNotification::ClosingReason reason);
	void onActionInvoked(IQNotification::id_t id,
			     const QString &action_key);

      private:
	std::vector<IQNotificationModifier::ptr_t> modifers;

	IQNotification modify(IQNotification notification);
};
