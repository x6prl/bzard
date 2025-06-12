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

#include "iqtrayicon.h"

namespace
{
QIcon urlToIcon(const QUrl &url)
{
	auto iconFileName = url.toString();
	iconFileName.replace("file:///", "");
	return QIcon{iconFileName};
}
} // anonymouse namespace

IQTrayIcon::IQTrayIcon(QObject *parent) : QSystemTrayIcon(parent)
{
	connect(this, &IQTrayIcon::iconUrlChanged,
		[this] { setIcon(urlToIcon(iconUrl_)); });
	connect(this, &IQTrayIcon::activated, [this](ActivationReason reason) {
		if (reason == Trigger)
			emit leftClick();
	});
}

QUrl IQTrayIcon::iconUrl() const { return iconUrl_; }

void IQTrayIcon::setIconUrl(const QUrl &iconUrl)
{
	iconUrl_ = iconUrl;
	emit iconUrlChanged();
}
