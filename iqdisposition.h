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

#include <experimental/optional>
#include <memory>

#include <QObject>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QSize>

#include <iqnotification.h>

class IQDisposition : public QObject
{
	Q_OBJECT
      public:
	using ptr_t = std::unique_ptr<IQDisposition>;
	template <class T> using optional = std::experimental::optional<T>;

	explicit IQDisposition(QObject *parent = nullptr);
	virtual ~IQDisposition() = default;

	virtual optional<QPoint> poses(IQNotification::id_t id, QSize size) = 0;

	virtual QPoint externalWindowPos() const = 0;

	const QScreen *screen() const;

	virtual void setExtraWindowSize(const QSize &value);

	virtual void setMargins(const QMargins &value);

	virtual void setSpacing(int value);

      public slots:
	virtual void remove(IQNotification::id_t id) = 0;
	virtual void removeAll() = 0;

      signals:
	void moveNotification(IQNotification::id_t id, QPoint pos);

      protected:
	int spacing;
	QMargins margins;
	QSize extraWindowSize{0, 0};
	QRect availableScreenGeometry;

	virtual void recalculateAvailableScreenGeometry();

      private:
	const QScreen *screen_;
};
