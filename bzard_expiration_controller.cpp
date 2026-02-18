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

#include "bzard_expiration_controller.h"

bool BzardExpirationController::expiration() const {
	return timer ? timer->isActive() : false;
}

void BzardExpirationController::setExpiration(bool expiration) {
	if (timer) {
		if (expiration) {
			timer->start();
		} else {
			timer->stop();
		}
	}

	emit expirationChanged();
}

int BzardExpirationController::timeout() const {
	return timer ? timer->interval() : 0;
}

void BzardExpirationController::setTimeout(int timeout) {
	if (timeout) {
		if (!timer) {
			timer = std::make_unique<QTimer>(/*timer?*/);
			timer->setSingleShot(true);
			connect(timer.get(), &QTimer::timeout, [this] { emit expired(); });
		}

		timer->setInterval(timeout);
	} else {
		timer.reset(nullptr);
	}

	emit timeoutChanged();
}
