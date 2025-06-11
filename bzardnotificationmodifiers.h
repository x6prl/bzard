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

#include "bzardconfig.h"
#include "bzardnotification.h"

namespace BzardNotificationModifiers
{

template <class T, class... A> std::unique_ptr<T> make(A &&... args)
{
	return std::make_unique<T>(std::forward<A>(args)...);
}

struct IDGenerator final : public IQNotificationModifier {
	IQNotification::id_t last_id{0};

	void modify(IQNotification &notification) final;
};

struct IconHandler final : public IQNotificationModifier {
	void modify(IQNotification &notification) final;
};

struct DefaultTimeout final : public IQNotificationModifier,
			      public IQConfigurable {
	DefaultTimeout();
	void modify(IQNotification &notification) final;

      private:
	uint16_t defaultTimeout;
};

struct TitleToIcon final : public IQNotificationModifier,
			   public IQConfigurable {
	TitleToIcon();
	void modify(IQNotification &notification) final;
};

struct BodyToTitleWhenTitleIsAppName final : public IQNotificationModifier,
					     public IQConfigurable {
	BodyToTitleWhenTitleIsAppName();
	void modify(IQNotification &notification) final;
};

struct ReplaceMinusToDash final : public IQNotificationModifier,
				  public IQConfigurable {
	ReplaceMinusToDash();

	void modify(IQNotification &notification) final;

      private:
	bool fixTitle, fixBody;

	static void replaceMinusToDash(QString &str);

	static constexpr auto minusPattern{" - "};
	static constexpr auto replaceTo{" — "};
};

} // namespace BzardNotificationModifiers
