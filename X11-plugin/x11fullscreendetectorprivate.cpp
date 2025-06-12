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

#include "x11fullscreendetectorprivate.h"

extern "C" {
#include <X11/Xatom.h>
}

#include <cstring>
#include <iostream>
#include <stdexcept>

X11Property::X11Property(gsl::not_null<void *> data_ptr, size_t size,
			 size_t items, bool autoXFree)
    : items_count{items}
{
	data.resize(size);
	std::memcpy(data.data(), data_ptr, size);
	if (autoXFree)
		XFree(data_ptr);
}

X11FullscreenDetectorPrivate::X11FullscreenDetectorPrivate()
    : disp{XOpenDisplay(nullptr)},
      CLIENT_LIST{disp ? internAtom(CLIENT_LIST_STR) : 0},
      WM_DESKTOP{disp ? internAtom(WM_DESKTOP_STR) : 0},
      WM_STATE{disp ? internAtom(WM_STATE_STR) : 0},
      WM_STATE_FULLSCREEN{disp ? internAtom(WM_STATE_FULLSCREEN_STR) : 0},
      CURRENT_DESKTOP{disp ? internAtom(CURRENT_DESKTOP_STR) : 0}
{
	if (!disp) {
		throw std::runtime_error{
		    "X11FullscreenDetector: Cannot open display."};
	}
	auto net_sup = internAtom("_NET_SUPPORTED");
	auto sup = getProperty(DefaultRootWindow(disp), XA_ATOM, net_sup);
	if (sup) {
		auto as = sup->to<Atom>();
		for (auto i : as) {
			auto n = getAtomName(i);
			if (n)
				std::cout << "Supported: " << *n << "\n";
		}
	}
}

X11FullscreenDetectorPrivate::~X11FullscreenDetectorPrivate()
{
	XCloseDisplay(disp);
}

bool X11FullscreenDetectorPrivate::fullscreenWindowsOnCurrentDesktop() const
{
	auto current_desktop = *currentDesktop();
	auto windows = getClientList();
	for (auto w : windows) {
		auto wm_state_prop = getProperty(w, XA_ATOM, WM_STATE);
		if (wm_state_prop) {
			auto as = wm_state_prop->to<Atom>();
			for (auto &a : as) {
				if (a == WM_STATE_FULLSCREEN) {
					auto win_desktop_o = windowDesktop(w);
					if (win_desktop_o &&
					    *win_desktop_o == current_desktop) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool X11FullscreenDetectorPrivate::fullscreenWindows() const
{
	auto windows = getClientList();
	for (auto w : windows) {
		auto wm_state_prop = getProperty(w, XA_ATOM, WM_STATE);
		if (wm_state_prop) {
			auto as = wm_state_prop->to<Atom>();
			for (auto &a : as) {
				if (a == WM_STATE_FULLSCREEN) {
					return true;
				}
			}
		}
	}
	return false;
}

X11FullscreenDetectorPrivate::optional<X11Property>
X11FullscreenDetectorPrivate::getProperty(Window win, Atom expectedType,
					  Atom propertyAtom) const
{
	static constexpr auto MAX_PROPERTY_VALUE_LEN = 1024;

	Atom prop_type;
	int prop_format;
	size_t items_count;
	size_t prop_bytes_after;
	unsigned char *prop_ptr;

	if (XGetWindowProperty(disp, win, propertyAtom, 0,
			       MAX_PROPERTY_VALUE_LEN, False, expectedType,
			       &prop_type, &prop_format, &items_count,
			       &prop_bytes_after, &prop_ptr) != Success) {
		std::cerr << "X11FullscreenDetector: can't get property"
			  << std::endl;
		return {};
	}
	if (prop_ptr == nullptr) {
		std::cerr << "X11FullscreenDetector: no property" << std::endl;
		return {};
	}
	if (prop_type != expectedType) {
		XFree(prop_ptr);
		std::cerr << "X11FullscreenDetector: invalid type, expected "
			  << expectedType << " got " << prop_type << std::endl;
		return {};
	}

	auto size = (prop_format / (32 / sizeof(Atom))) * items_count;
	X11Property prop{prop_ptr, size, items_count};

	return {prop};
}

std::vector<Window> X11FullscreenDetectorPrivate::getClientList() const
{
	auto client_list =
	    getProperty(DefaultRootWindow(disp), XA_WINDOW, CLIENT_LIST);
	if (!client_list)
		throw std::runtime_error{
		    "X11FullscreenDetector: cannot get windows."};

	return client_list->to<Window>();
}

X11FullscreenDetectorPrivate::optional<std::string>
X11FullscreenDetectorPrivate::getAtomName(Atom atom) const
{
	auto n = XGetAtomName(disp, atom);
	if (n == nullptr) {
		return {};
	} else {
		optional<std::string> ret{std::string{n}};
		XFree(n);
		return ret;
	}
}

Atom X11FullscreenDetectorPrivate::internAtom(const char *atomName,
					      bool onlyIfExists) const
{
	return XInternAtom(disp, atomName, onlyIfExists ? True : False);
}

X11FullscreenDetectorPrivate::optional<size_t>
X11FullscreenDetectorPrivate::windowDesktop(Window win) const
{
	auto win_desktop_prop = getProperty(win, XA_CARDINAL, WM_DESKTOP);
	if (win_desktop_prop) {
		return win_desktop_prop->toSingle<Atom>();
	}
	return {};
}

X11FullscreenDetectorPrivate::optional<size_t>
X11FullscreenDetectorPrivate::currentDesktop() const
{
	auto cur_desktop_prop =
	    getProperty(DefaultRootWindow(disp), XA_CARDINAL, CURRENT_DESKTOP);
	if (cur_desktop_prop) {
		return cur_desktop_prop->toSingle<Atom>();
	}
	return {};
}
