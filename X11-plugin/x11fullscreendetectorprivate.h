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

extern "C" {
#include <X11/Xlib.h>
}

#include <experimental/optional>
#include <string>
#include <vector>

#include <gsl/gsl>

#include <iqfullscreendetector.h>

class X11Property
{
	using byte = unsigned char;

      public:
	template <class T> using optional = std::experimental::optional<T>;

	X11Property(gsl::not_null<void *> data_ptr, size_t size, size_t items,
		    bool autoXFree = true);

	template <class T> std::vector<T> to() const
	{
		auto calculated_items_count = data.size() / sizeof(T);
		if (items_count != calculated_items_count)
			throw std::invalid_argument{
			    "X11Property: can't convert X11Property"};
		std::vector<T> ret;
		ret.resize(items_count);
		std::memcpy(ret.data(), data.data(), data.size());
		return ret;
	}

	template <class T> optional<T> toSingle() const
	{
		auto v = to<T>();
		return v.empty() ? optional<T>{} : optional<T>{v.front()};
	}

      private:
	std::vector<byte> data;
	size_t items_count;
};

class X11FullscreenDetectorPrivate final : public IQFullscreenDetector
{
	template <class T> using optional = X11Property::optional<T>;

      public:
	X11FullscreenDetectorPrivate();
	~X11FullscreenDetectorPrivate() final;

	bool fullscreenWindowsOnCurrentDesktop() const final;
	bool fullscreenWindows() const final;

      private:
	static constexpr auto CLIENT_LIST_STR = "_NET_CLIENT_LIST";
	static constexpr auto WM_DESKTOP_STR = "_NET_WM_DESKTOP";
	static constexpr auto WM_STATE_STR = "_NET_WM_STATE";
	static constexpr auto WM_STATE_FULLSCREEN_STR =
	    "_NET_WM_STATE_FULLSCREEN";
	static constexpr auto CURRENT_DESKTOP_STR = "_NET_CURRENT_DESKTOP";

	mutable Display *disp{nullptr};
	const Atom CLIENT_LIST;
	const Atom WM_DESKTOP;
	const Atom WM_STATE;
	const Atom WM_STATE_FULLSCREEN;
	const Atom CURRENT_DESKTOP;

	optional<X11Property> getProperty(Window win, Atom expetedType,
					  Atom propertyAtom) const;
	std::vector<Window> getClientList() const;

	optional<std::string> getAtomName(Atom atom) const;

	Atom internAtom(const char *atomName, bool onlyIfExists = true) const;

	optional<size_t> windowDesktop(Window win) const;
	optional<size_t> currentDesktop() const;
};
