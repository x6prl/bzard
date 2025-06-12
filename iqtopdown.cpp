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

#include "iqtopdown.h"

IQTopDown::IQTopDown(QObject *parent) : IQDisposition(parent)
{
	recalculateAvailableScreenGeometry();
}

IQTopDown::optional<QPoint> IQTopDown::poses(IQNotification::id_t id,
					     QSize size)
{
	// Already here, must be replaced
	auto current_pos_it = dispositions.find(id);
	if (current_pos_it != dispositions.end())
		return current_pos_it->second.topLeft();

	// Okay, lets calc new position
	auto avail = availableGeometry();

	auto pos_point = avail.topRight() -
			 QPoint{size.width() - 1
				// topRigth returns not correct coordinates
				// So we add 1 to result (-(-1))
				// Look at Qt's docs for more
				,
				0};
	QRect pos{pos_point, size};
	if (avail.contains(pos)) {
		dispositions[id] = pos;
		return {pos_point};
	} else {
		return {};
	}
}

QPoint IQTopDown::externalWindowPos() const
{
	auto pos_point = availableScreenGeometry.bottomRight() -
			 QPoint{extraWindowSize.width() - 1
				// topRigth returns not correct coordinates
				// So we add 1 to result (-(-1))
				// Look at Qt's docs for more
				,
				0};
	return pos_point;
}

void IQTopDown::setExtraWindowSize(const QSize &value)
{
	IQDisposition::setExtraWindowSize(value);
	recalculateAvailableScreenGeometry();
}

void IQTopDown::setSpacing(int value)
{
	IQDisposition::setSpacing(value);
	recalculateAvailableScreenGeometry();
}

void IQTopDown::remove(IQNotification::id_t id)
{
	static auto recalculateDispositions = [this](const auto &to_remove_it)
	    -> std::map<IQNotification::id_t, QPoint> {
		    auto end = dispositions.end();
		    const QRect &to_remove = to_remove_it->second;
		    auto move_up_for = spacing + to_remove.height();

		    auto it = std::next(to_remove_it);
		    if (it == end) {
			    return {}; // Nothing to move up
		    }
		    std::map<IQNotification::id_t, QPoint> posToMove;
		    for (; it != end; ++it) {
			    auto d_id = it->first;
			    auto &d_pos = it->second;
			    d_pos.moveTop(d_pos.top() - move_up_for);
			    posToMove.emplace(d_id, d_pos.topLeft());
		    }
		    return posToMove;
	    };

	auto pos_it = dispositions.find(id);
	if (pos_it != dispositions.end()) {
		// Just calculate all
		auto posToMove = recalculateDispositions(pos_it);
		// Now move
		for (auto &p : posToMove)
			emit moveNotification(p.first, p.second);

		// pos_it will not be invalidated even
		// if posess will be called before next line
		dispositions.erase(pos_it);
	} else {
		//		throw std::runtime_error{
		//		    "IQTopDown: can't find old disposition"};
	}
}

void IQTopDown::removeAll() { dispositions.clear(); }

void IQTopDown::recalculateAvailableScreenGeometry()
{
	IQDisposition::recalculateAvailableScreenGeometry();
	auto extra_bottom_margin = extraWindowSize.height() + spacing;
	availableScreenGeometry -= QMargins{0, 0, 0, extra_bottom_margin};
}

QRect IQTopDown::availableGeometry() const
{
	auto ret = availableScreenGeometry;
	ret.adjust(0, dispositions.empty() ? 0 : spacing, 0, 0);

	if (dispositions.empty())
		return ret;

	const auto &last_object = *std::crbegin(dispositions);

	// Top margin already included in availableScreenGeometry
	auto bot_margin = last_object.second.bottom() - margins.top();
	ret.adjust(0, bot_margin, 0, 0);
	return ret;
}
