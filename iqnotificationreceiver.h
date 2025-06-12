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

#pragma once

#include <QObject>

#include "iqnotification.h"

class IQNotificationReceiver : public QObject
{
	Q_OBJECT

      public:
	using QObject::QObject;
	virtual ~IQNotificationReceiver() = default;

      signals:
	void notificationDroppedSignal(IQNotification::id_t id,
				       IQNotification::ClosingReason reason);
	void actionInvokedSignal(IQNotification::id_t id,
				 const QString &action_key);

      public slots:
	virtual void
	onCreateNotification(const IQNotification &notification) = 0;
	virtual void onDropNotification(IQNotification::id_t id) = 0;
};
