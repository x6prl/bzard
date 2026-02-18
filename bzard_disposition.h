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

#include <experimental/optional>
#include <memory>

#include <QObject>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QSize>

#include <bzard_notification.h>

class BzardDisposition : public QObject {
	Q_OBJECT
  public:
	using PtrT = std::unique_ptr<BzardDisposition>;
	template <class T> using Optional = std::experimental::optional<T>;

	explicit BzardDisposition(QObject *parent = nullptr);
	virtual ~BzardDisposition() = default;

	virtual Optional<QPoint> poses(BzardNotification::IdT id, QSize size) = 0;

	virtual QPoint externalWindowPosition() const = 0;

	const QScreen *screen() const;

	virtual void setExtraWindowSize(const QSize &VALUE);

	virtual void setMargins(const QMargins &VALUE);

	virtual void setSpacing(int value);

  public slots:
	virtual void remove(BzardNotification::IdT id) = 0;
	virtual void removeAll() = 0;

  signals:
	void moveNotification(BzardNotification::IdT id, QPoint position);

  protected:
	int spacing;
	QMargins margins;
	QSize extraWindowSize{0, 0};
	QRect availableScreenGeometry;

	virtual void recalculateAvailableScreenGeometry();

  private:
	const QScreen *screen_;
};
