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

#include <QObject>

#include "bzard_notification.h"

class BzardNotificationReceiver : public QObject {
	Q_OBJECT

  public:
	using QObject::QObject;
	virtual ~BzardNotificationReceiver() = default;

  signals:
	void notificationDroppedSignal(BzardNotification::IdT id,
	                               BzardNotification::ClosingReason reason);
	void actionInvokedSignal(BzardNotification::IdT id,
	                         const QString &actionKey);

  public slots:
	virtual void
	onCreateNotification(const BzardNotification &notification) = 0;
	virtual void onDropNotification(BzardNotification::IdT id) = 0;
};
