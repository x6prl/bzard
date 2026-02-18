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

#include "bzard_config.h"
#include "bzard_notification.h"

namespace BzardNotificationModifiers {

template <class T, class... A> std::unique_ptr<T> make(A &&...args) {
	return std::make_unique<T>(std::forward<A>(args)...);
}

struct IDGenerator final : public BzardNotificationModifier {
	BzardNotification::IdT lastId{0};

	void modify(BzardNotification &notification) final;
};

struct IconHandler final : public BzardNotificationModifier {
	void modify(BzardNotification &notification) final;
};

struct DefaultTimeout final : public BzardNotificationModifier,
							  public BzardConfigurable {
	DefaultTimeout();
	void modify(BzardNotification &notification) final;

  private:
	uint16_t defaultTimeout;
};

struct TitleToIcon final : public BzardNotificationModifier,
						   public BzardConfigurable {
	TitleToIcon();
	void modify(BzardNotification &notification) final;
};

struct BodyToTitleWhenTitleIsAppName final : public BzardNotificationModifier,
											 public BzardConfigurable {
	BodyToTitleWhenTitleIsAppName();
	void modify(BzardNotification &notification) final;
};

struct ReplaceMinusToDash final : public BzardNotificationModifier,
								  public BzardConfigurable {
	ReplaceMinusToDash();

	void modify(BzardNotification &notification) final;

  private:
	bool fixTitle, fixBody;

	static void replaceMinusToDash(QString &str);

	static constexpr auto MINUS_PATTERN{" - "};
	static constexpr auto REPLACE_TO{" — "};
};

} // namespace BzardNotificationModifiers
