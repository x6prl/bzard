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

#include "bzardexpirationcontroller.h"

bool BzardExpirationController::expiration() const
{
	return t ? t->isActive() : false;
}

void BzardExpirationController::setExpiration(bool expiration)
{
	if (t) {
		if (expiration) {
			t->start();
		} else {
			t->stop();
		}
	}

	emit expirationChanged();
}

int BzardExpirationController::timeout() const { return t ? t->interval() : 0; }

void BzardExpirationController::setTimeout(int timeout)
{
	if (timeout) {
		if (!t) {
			t = std::make_unique<QTimer>();
			t->setSingleShot(true);
			connect(t.get(), &QTimer::timeout,
				[this] { emit expired(); });
		}

		t->setInterval(timeout);
	} else {
		t.reset(nullptr);
	}

	emit timeoutChanged();
}
