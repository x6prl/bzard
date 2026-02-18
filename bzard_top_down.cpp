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

#include "bzard_top_down.h"

BzardTopDown::BzardTopDown(QObject *parent) : BzardDisposition(parent) {
	recalculateAvailableScreenGeometry();
}

BzardTopDown::Optional<QPoint> BzardTopDown::poses(BzardNotification::IdT id,
                                                   QSize size) {
	// Already here, must be replaced
	auto currentPosition = dispositions.find(id);
	if (currentPosition != dispositions.end())
		return currentPosition->second.topLeft();

	// Okay, lets calc new position
	auto avail = availableGeometry();

	auto positionPoint =
		  avail.topRight() - QPoint{size.width() - 1
	                                // topRigth returns not correct coordinates
	                                // So we add 1 to result (-(-1))
	                                // Look at Qt's docs for more
	                                ,
	                                0};
	QRect position{positionPoint, size};
	if (avail.contains(position)) {
		dispositions[id] = position;
		return {positionPoint};
	} else {
		return {};
	}
}

QPoint BzardTopDown::externalWindowPosition() const {
	auto positionPoint = availableScreenGeometry.bottomRight() -
	                     QPoint{extraWindowSize.width() - 1
	                            // topRigth returns not correct coordinates
	                            // So we add 1 to result (-(-1))
	                            // Look at Qt's docs for more
	                            ,
	                            0};
	return positionPoint;
}

void BzardTopDown::setExtraWindowSize(const QSize &VALUE) {
	BzardDisposition::setExtraWindowSize(VALUE);
	recalculateAvailableScreenGeometry();
}

void BzardTopDown::setSpacing(int value) {
	BzardDisposition::setSpacing(value);
	recalculateAvailableScreenGeometry();
}

void BzardTopDown::remove(BzardNotification::IdT id) {
	static auto recalculateDispositions = [this](const auto &TO_REMOVE_IT)
		  -> std::map<BzardNotification::IdT, QPoint> {
		auto end = dispositions.end();
		const QRect &TO_REMOVE = TO_REMOVE_IT->second;
		auto moveUpFor = spacing + TO_REMOVE.height();

		auto remove = std::next(TO_REMOVE_IT);
		if (remove == end) {
			return {};
		}
		std::map<BzardNotification::IdT, QPoint> positionToMove;
		for (; remove != end; ++remove) {
			auto dId = remove->first;
			auto &dPosition = remove->second;
			dPosition.moveTop(dPosition.top() - moveUpFor);
			positionToMove.emplace(dId, dPosition.topLeft());
		}
		return positionToMove;
	};

	auto position = dispositions.find(id);
	if (position != dispositions.end()) {
		// Just calculate all
		auto positionToMove = recalculateDispositions(position);
		// Now move
		for (auto &move : positionToMove)
			emit moveNotification(move.first, move.second);

		// pos_it will not be invalidated even
		// if posess will be called before next line
		dispositions.erase(position);
	} else {
		//		throw std::runtime_error{
		//		    "BzardTopDown: can't find old disposition"};
	}
}

void BzardTopDown::removeAll() { dispositions.clear(); }

void BzardTopDown::recalculateAvailableScreenGeometry() {
	BzardDisposition::recalculateAvailableScreenGeometry();
	auto extraBottomMargin = extraWindowSize.height() + spacing;
	availableScreenGeometry -= QMargins{0, 0, 0, extraBottomMargin};
}

QRect BzardTopDown::availableGeometry() const {
	auto result = availableScreenGeometry;
	result.adjust(0, dispositions.empty() ? 0 : spacing, 0, 0);

	if (dispositions.empty())
		return result;

	const auto &LAST_OBJECT = *std::crbegin(dispositions);

	// Top margin already included in availableScreenGeometry
	auto bottomMargin = LAST_OBJECT.second.bottom() - margins.top();
	result.adjust(0, bottomMargin, 0, 0);
	return result;
}
