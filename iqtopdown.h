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

#include <map>

#include <QObject>
#include <QPoint>

#include <iqdisposition.h>

class IQTopDown final : public IQDisposition
{
	Q_OBJECT

      public:
	using IQDisposition::optional;

	explicit IQTopDown(QObject *parent = nullptr);

	optional<QPoint> poses(IQNotification::id_t id, QSize size) final;

	QPoint externalWindowPos() const final;

	void setExtraWindowSize(const QSize &value) final;

	void setSpacing(int value) final;

      public slots:
	void remove(IQNotification::id_t id) final;
	void removeAll() final;

      private:
	std::map<IQNotification::id_t, QRect> dispositions;

	void recalculateAvailableScreenGeometry() final;
	QRect availableGeometry() const;
};
