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

#include "bzardtrayicon.h"

namespace
{
QIcon urlToIcon(const QUrl &url)
{
	auto iconFileName = url.toString();
	iconFileName.replace("file:///", "");
	return QIcon{iconFileName};
}
} // anonymouse namespace

BzardTrayIcon::BzardTrayIcon(QObject *parent) : QSystemTrayIcon(parent)
{
	connect(this, &BzardTrayIcon::iconUrlChanged,
		[this] { setIcon(urlToIcon(iconUrl_)); });
	connect(this, &BzardTrayIcon::activated, [this](ActivationReason reason) {
		if (reason == Trigger)
			emit leftClick();
	});
}

QUrl BzardTrayIcon::iconUrl() const { return iconUrl_; }

void BzardTrayIcon::setIconUrl(const QUrl &iconUrl)
{
	iconUrl_ = iconUrl;
	emit iconUrlChanged();
}
