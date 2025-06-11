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

#include "bzarddisposition.h"

#include <QApplication>

IQDisposition::IQDisposition(QObject *parent)
    : QObject{parent}, screen_{QApplication::screens().at(0)}
{
	connect(screen_, &QScreen::availableGeometryChanged, this,
		&IQDisposition::recalculateAvailableScreenGeometry);
}

const QScreen *IQDisposition::screen() const { return screen_; }

void IQDisposition::setExtraWindowSize(const QSize &value)
{
	extraWindowSize = value;
}

void IQDisposition::setMargins(const QMargins &value)
{
	margins = value;
	recalculateAvailableScreenGeometry();
}

void IQDisposition::setSpacing(int value) { spacing = value; }

void IQDisposition::recalculateAvailableScreenGeometry()
{
	auto screenGeometry = screen()->availableGeometry();
	availableScreenGeometry = screenGeometry - margins;
}
