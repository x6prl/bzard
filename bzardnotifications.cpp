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

#include "bzardnotifications.h"

#include <experimental/optional>
#include <memory>
#include <utility>

#include <QString>
#include <QStringList>
#include <QTimer>

template <class T> using optional = std::experimental::optional<T>;

BzardNotifications::BzardNotifications(IQDisposition::ptr_t disposition_,
				 QObject *parent)
    : IQNotificationReceiver(parent), IQConfigurable{"popup_notifications"},
      disposition{std::move(disposition_)}
{
	if (!disposition)
		throw std::invalid_argument{
		    "BzardNotifications: provide disposition"};
	disposition->setExtraWindowSize(extraWindowSize());
	disposition->setSpacing(spacing());
	disposition->setMargins(margins());

	connect(this, &BzardNotifications::dropNotification, disposition.get(),
		&IQDisposition::remove);
	connect(disposition.get(), &IQDisposition::moveNotification,
		[this](IQNotification::id_t id, QPoint pos) {
			emit moveNotification(static_cast<int>(id), pos);
			checkExtraNotifications();
		});
}

BzardNotifications *
BzardNotifications::get(IQDisposition::ptr_t disposition)
{
	static BzardNotifications *ptr_{nullptr};
	if (!ptr_) {
		if (!disposition)
			throw std::invalid_argument("IQDisposition: call get "
						    "with valid disposition "
						    "first");

		ptr_ = new BzardNotifications{std::move(disposition)};
	}
    return ptr_;
}

void BzardNotifications::setFullscreenDetector(
    std::unique_ptr<IQFullscreenDetector> detector)
{
	fullscreenDetector = std::move(detector);
}

int BzardNotifications::extraNotificationsCount() const
{
	return static_cast<int>(extraNotifications.size());
}

bool BzardNotifications::closeAllByRightClick() const
{
	return config
	    .value(CONFIG_CLOSE_ALL_BY_RIGHT_CLICK,
		   CONFIG_CLOSE_ALL_BY_RIGHT_CLICK_DEFAULT)
	    .toBool();
}

bool BzardNotifications::closeVisibleByLeftClick() const
{
	return config
	    .value(CONFIG_CLOSE_VISIBLE_BY_MIDDLE_CLICK,
		   CONFIG_CLOSE_VISIBLE_BY_MIDDLE_CLICK_DEFAULT)
	    .toBool();
}

bool BzardNotifications::closeByLeftClick() const
{
	return config
	    .value(CONFIG_CLOSE_BY_LEFT_CLICK,
		   CONFIG_CLOSE_BY_LEFT_CLICK_DEFAULT)
	    .toBool();
}

bool BzardNotifications::dontShowWhenFullscreenAny() const
{
	return config
	    .value(CONFIG_DONT_SHOW_WHEN_FULLSCREEN_ANY,
		   CONFIG_DONT_SHOW_WHEN_FULLSCREEN_ANY_DEFAULT)
	    .toBool();
}

bool BzardNotifications::dontShowWhenFullscreenCurrentDesktop() const
{
	return config
	    .value(CONFIG_DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP,
		   CONFIG_DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP_DEFAULT)
	    .toBool();
}

void BzardNotifications::setDontShowWhenFullscreenCurrentDesktop(bool value)
{
	config.setValue(CONFIG_DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP,
			value);
	emit dontShowWhenFullscreenCurrentDesktopChanged();
}

void BzardNotifications::onCreateNotification(const IQNotification &n)
{
	if (!shouldShowPopup())
		return;
	if (!createNotificationIfSpaceAvailable(n)) {
		extraNotifications.push(n);
		emit extraNotificationsCountChanged();
	}
}

void BzardNotifications::onDropNotification(IQNotification::id_t id)
{
	emit dropNotification(static_cast<int>(id));
	emit notificationDroppedSignal(id,
				       IQNotification::CR_NOTIFICATION_CLOSED);
}

void BzardNotifications::onCloseButtonPressed(int id)
{
	emit dropNotification(id);
	emit notificationDroppedSignal(
	    static_cast<IQNotification::id_t>(id),
	    IQNotification::CR_NOTIFICATION_DISMISSED);
	checkExtraNotifications();
}

void BzardNotifications::onActionButtonPressed(int id, const QString &action)
{
	emit dropNotification(id);
	emit actionInvokedSignal(static_cast<IQNotification::id_t>(id), action);
	emit notificationDroppedSignal(
	    static_cast<IQNotification::id_t>(id),
	    IQNotification::CR_NOTIFICATION_DISMISSED);
}

void BzardNotifications::onExpired(int id)
{
	emit dropNotification(id);
	emit notificationDroppedSignal(static_cast<IQNotification::id_t>(id),
				       IQNotification::CR_NOTIFICATION_EXPIRED);
}

void BzardNotifications::onDropAll()
{
	onDropStacked();
	onDropVisible();
}

void BzardNotifications::onDropStacked()
{
	decltype(extraNotifications) empty;
	std::swap(extraNotifications, empty);
	emit extraNotificationsCountChanged();
}

void BzardNotifications::onDropVisible()
{
	emit dropAllVisible();
	disposition->removeAll();
	checkExtraNotifications();
}

int BzardNotifications::spacing() const
{
	return config.value(CONFIG_SPACING, CONFIG_SPACING_DEFAULT).toInt();
}

QMargins BzardNotifications::margins() const
{
	static auto string_list_to_margins =
	    [](QStringList list) -> optional<QMargins> {
		enum Sides { LEFT = 0, TOP, RIGHT, BOTTOM, SIZE };

		if (list.size() != SIZE)
			return {};

		auto get = [&list](auto index) {
			bool ok{true};
			auto value = list[index].toInt(&ok);
			if (!ok)
				throw std::logic_error{"IQConfig: "
						       "ui::global_margins "
						       "wrong value!"};
			return value;
		};

		QMargins ret;
		ret.setLeft(get(LEFT));
		ret.setTop(get(TOP));
		ret.setRight(get(RIGHT));
		ret.setBottom(get(BOTTOM));
		return {ret};
	};

	auto config_field = config.value(CONFIG_GLOBAL_MARGINS);
	auto config_margin =
	    string_list_to_margins(config_field.toStringList());
	if (config_margin) {
		return *config_margin;
	} else {
		auto screen = disposition->screen()->availableSize();
		auto margin = GLOBAL_MARGINS_DEFAULT_FACTOR * screen.height();
		auto m = static_cast<int>(margin);
		return {m, m, m, m};
	}
}

QSize BzardNotifications::windowSize() const
{
	return windowSize(CONFIG_WIDTH, CONFIG_HEIGHT, WIDTH_DEFAULT_FACTOR,
			  HEIGHT_DEFAULT_FACTOR);
}

QSize BzardNotifications::windowSize(const QString &width_key,
				  const QString &height_key,
				  double width_factor,
				  double height_factor) const
{
	// TODO: cache values

	auto get_window_size_from_config =
	    [width_key, height_key](const auto &config) -> optional<QSize> {
		auto get = [&config](auto key) {
			bool ok{true};
			auto value = config.value(key, 0).toInt(&ok);
			if (!ok || value < 0)
				throw std::logic_error{"IQConfig: window or "
						       "extra_window size "
						       "wrong value!"};
			return value;
		};

		auto w = get(width_key);
		auto h = get(height_key);

		if (!w || !h)
			return {};
		else
			return {QSize{w, h}};
	};

	auto config_size = get_window_size_from_config(config);
	if (config_size) {
		return *config_size;
	} else {
		auto screen = disposition->screen()->availableSize();
		auto w = width_factor * screen.width();
		auto h = height_factor * screen.height();
		return QSize{static_cast<int>(w), static_cast<int>(h)};
	}
}

QSize BzardNotifications::extraWindowSize() const
{
	return windowSize(CONFIG_EXTRA_WINDOW_WIDTH, CONFIG_EXTRA_WINDOW_HEIGHT,
			  EXTRA_WINDOW_WIDTH_DEFAULT_FACTOR,
			  EXTRA_WINDOW_HEIGHT_DEFAULT_FACTOR);
}

QPoint BzardNotifications::extraWindowPos() const
{
	return disposition->externalWindowPos();
}

bool BzardNotifications::createNotificationIfSpaceAvailable(
    const IQNotification &n)
{
	auto size = windowSize();
	auto pos = disposition->poses(n.id, size);
	if (pos) {
		auto id = n.replaces_id ? n.replaces_id : n.id;
		emit createNotification(static_cast<int>(id), size, *pos,
					n.expire_timeout, n.application, n.body,
					n.title, n.icon_url, n.actions);
		return true;
	} else {
		return false;
	}
}

void BzardNotifications::checkExtraNotifications()
{
	while (!extraNotifications.empty() &&
	       createNotificationIfSpaceAvailable(extraNotifications.front())) {
		extraNotifications.pop();
		emit extraNotificationsCountChanged();
	}
}

bool BzardNotifications::shouldShowPopup() const
{
	if (fullscreenDetector) {
		if (dontShowWhenFullscreenCurrentDesktop()) {
			auto cd = fullscreenDetector
				      ->fullscreenWindowsOnCurrentDesktop();
			if (cd)
				return false;
		}
		if (dontShowWhenFullscreenAny()) {
			auto any = fullscreenDetector->fullscreenWindows();
			if (any)
				return false;
		}
	}
	return true;
}
