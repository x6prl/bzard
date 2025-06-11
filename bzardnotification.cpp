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

#include "bzardnotification.h"

IQNotification::operator QString() const
{
	QString ret;
	ret += "#" + QString::number(id);
	if (replaces_id)
		ret += "→" + QString::number(replaces_id);
	ret += '|' + application;
	ret += '|' + body;
	ret += '|' + title;
	ret += '|' + icon_url;
	ret += "|t" + QString::number(expire_timeout);
	return ret;
}

IQNotificationModifier::~IQNotificationModifier() {}
