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

#include <memory>

#include <QString>
#include <QStringList>
#include <QVariantMap>

struct BzardNotification {
	using IdT = uint32_t;

	enum ClosingReason : uint32_t {
		CR_NOTIFICATION_EXPIRED = 1,
		CR_NOTIFICATION_DISMISSED,
		CR_NOTIFICATION_CLOSED,
		CR_NOTIFICATION_CLOSED_REASON_UNDEFINED
	};

	enum ExpireTimeout : int { ET_SERVER_DECIDES = -1, ET_FOREVER = 0 };

	IdT id;
	QString application;
	QString body;
	QString title;
	QString iconUrl;
	QStringList actions;
	QVariantMap hints;
	ExpireTimeout expireTimeout;

	IdT replacesId;

	operator QString() const;
};

struct BzardNotificationModifier {
	using PtrT = std::unique_ptr<BzardNotificationModifier>;

	virtual ~BzardNotificationModifier();
	virtual void modify(BzardNotification &notification) = 0;
};
