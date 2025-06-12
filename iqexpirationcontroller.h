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

#include <memory>

#include <QObject>
#include <QTimer>

class IQExpirationController : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool expiration READ expiration WRITE setExpiration NOTIFY
		       expirationChanged)
	Q_PROPERTY(
	    int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
      public:
	using QObject::QObject;

	bool expiration() const;
	void setExpiration(bool expiration);

	int timeout() const;
	void setTimeout(int timeout);

      signals:
	void expired();
	void expirationChanged();
	void timeoutChanged();

      public slots:

      private:
	std::unique_ptr<QTimer> t;
};
