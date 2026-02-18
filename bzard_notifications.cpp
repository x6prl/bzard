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

#include "bzard_notifications.h"

#include "bzard_themes.h"

#include <experimental/optional>
#include <memory>
#include <utility>

#include <QString>
#include <QStringList>
#include <QTimer>

template <class T> using optional = std::experimental::optional<T>;

BzardNotifications::BzardNotifications(BzardDisposition::PtrT disposition_,
                                       QObject *parent)
	  : BzardNotificationReceiver(parent),
		BzardConfigurable{"popup_notifications"},
		disposition{std::move(disposition_)} {
	if (!disposition)
		throw std::invalid_argument{"BzardNotifications: provide disposition"};
	disposition->setExtraWindowSize(extraWindowSize());
	disposition->setSpacing(spacing());
	disposition->setMargins(margins());

	connect(this, &BzardNotifications::dropNotification, disposition.get(),
	        &BzardDisposition::remove);
	connect(disposition.get(), &BzardDisposition::moveNotification,
	        [this](BzardNotification::IdT id, QPoint position) {
				emit moveNotification(static_cast<int>(id), position);
				checkExtraNotifications();
			});
}

BzardNotifications *
BzardNotifications::get(BzardDisposition::PtrT disposition) {
	static BzardNotifications *ptr_{nullptr};
	if (!ptr_) {
		if (!disposition)
			throw std::invalid_argument("BzardDisposition: call get "
			                            "with valid disposition "
			                            "first");

		ptr_ = new BzardNotifications{std::move(disposition)};
	}
	return ptr_;
}

void BzardNotifications::setFullscreenDetector(
	  std::unique_ptr<BzardFullscreenDetector> detector) {
	fullscreenDetector = std::move(detector);
}

int BzardNotifications::extraNotificationsCount() const {
	return static_cast<int>(extraNotifications.size());
}

bool BzardNotifications::closeAllByRightClick() const {
	return config
	      .value(CONFIG_CLOSE_ALL_BY_RIGHT_CLICK,
	             CONFIG_CLOSE_ALL_BY_RIGHT_CLICK_DEFAULT)
	      .toBool();
}

bool BzardNotifications::closeVisibleByLeftClick() const {
	return config
	      .value(CONFIG_CLOSE_VISIBLE_BY_MIDDLE_CLICK,
	             CONFIG_CLOSE_VISIBLE_BY_MIDDLE_CLICK_DEFAULT)
	      .toBool();
}

bool BzardNotifications::closeByLeftClick() const {
	return config
	      .value(CONFIG_CLOSE_BY_LEFT_CLICK, CONFIG_CLOSE_BY_LEFT_CLICK_DEFAULT)
	      .toBool();
}

bool BzardNotifications::dontShowWhenFullscreenAny() const {
	return config
	      .value(CONFIG_DONT_SHOW_WHEN_FULLSCREEN_ANY,
	             CONFIG_DONT_SHOW_WHEN_FULLSCREEN_ANY_DEFAULT)
	      .toBool();
}

bool BzardNotifications::dontShowWhenFullscreenCurrentDesktop() const {
	return config
	      .value(CONFIG_DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP,
	             CONFIG_DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP_DEFAULT)
	      .toBool();
}

void BzardNotifications::setDontShowWhenFullscreenCurrentDesktop(bool value) {
	config.setValue(CONFIG_DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP, value);
	emit dontShowWhenFullscreenCurrentDesktopChanged();
}

void BzardNotifications::onCreateNotification(
	  const BzardNotification &NOTIFICATION) {
	if (!shouldShowPopup())
		return;
	if (!createNotificationIfSpaceAvailable(NOTIFICATION)) {
		extraNotifications.push(NOTIFICATION);
		emit extraNotificationsCountChanged();
	}
}

void BzardNotifications::onDropNotification(BzardNotification::IdT id) {
	emit dropNotification(static_cast<int>(id));
	emit notificationDroppedSignal(id,
	                               BzardNotification::CR_NOTIFICATION_CLOSED);
}

void BzardNotifications::onCloseButtonPressed(int id) {
	emit dropNotification(id);
	emit notificationDroppedSignal(
		  static_cast<BzardNotification::IdT>(id),
		  BzardNotification::CR_NOTIFICATION_DISMISSED);
	checkExtraNotifications();
}

void BzardNotifications::onActionButtonPressed(int id, const QString &action) {
	emit dropNotification(id);
	emit actionInvokedSignal(static_cast<BzardNotification::IdT>(id), action);
	emit notificationDroppedSignal(
		  static_cast<BzardNotification::IdT>(id),
		  BzardNotification::CR_NOTIFICATION_DISMISSED);
}

void BzardNotifications::onExpired(int id) {
	emit dropNotification(id);
	emit notificationDroppedSignal(static_cast<BzardNotification::IdT>(id),
	                               BzardNotification::CR_NOTIFICATION_EXPIRED);
}

void BzardNotifications::onDropAll() {
	onDropStacked();
	onDropVisible();
}

void BzardNotifications::onDropStacked() {
	decltype(extraNotifications) empty;
	std::swap(extraNotifications, empty);
	emit extraNotificationsCountChanged();
}

void BzardNotifications::onDropVisible() {
	emit dropAllVisible();
	disposition->removeAll();
	checkExtraNotifications();
}

int BzardNotifications::spacing() const {
	return config.value(CONFIG_SPACING, CONFIG_SPACING_DEFAULT).toInt();
}

QMargins BzardNotifications::margins() const {
	static auto stringListToMargins =
		  [](QStringList list) -> optional<QMargins> {
		enum Sides { LEFT = 0, TOP, RIGHT, BOTTOM, SIZE };

		if (list.size() != SIZE)
			return {};

		auto get = [&list](auto index) {
			bool ok{true};
			auto value = list[index].toInt(&ok);
			if (!ok)
				throw std::logic_error{"BzardConfig: "
				                       "ui::global_margins "
				                       "wrong value!"};
			return value;
		};

		QMargins result;
		result.setLeft(get(LEFT));
		result.setTop(get(TOP));
		result.setRight(get(RIGHT));
		result.setBottom(get(BOTTOM));
		return {result};
	};

	auto configField = config.value(CONFIG_GLOBAL_MARGINS);
	auto configMargin = stringListToMargins(configField.toStringList());
	if (configMargin) {
		return *configMargin;
	} else {
		auto screen = disposition->screen()->availableSize();
		auto margin = GLOBAL_MARGINS_DEFAULT_FACTOR * screen.height();
		auto margin_ = static_cast<int>(margin);
		return {margin_, margin_, margin_, margin_};
	}
}

QSize BzardNotifications::windowSize() const {
	auto notificationsTheme = BzardThemes::instance().notificationsTheme();
	if (notificationsTheme) {
		auto width = notificationsTheme->width();
		auto height = notificationsTheme->height();
		if (width > 0 && height > 0)
			return QSize{static_cast<int>(width), static_cast<int>(height)};
	}
	return autoWindowSize(WIDTH_DEFAULT_FACTOR, HEIGHT_DEFAULT_FACTOR);
}

QSize BzardNotifications::extraWindowSize() const {
	auto notificationsTheme = BzardThemes::instance().notificationsTheme();
	if (notificationsTheme) {
		auto width = notificationsTheme->extraWindowWidth();
		auto height = notificationsTheme->extraWindowHeight();
		if (width > 0 && height > 0)
			return QSize{static_cast<int>(width), static_cast<int>(height)};
	}
	return autoWindowSize(EXTRA_WINDOW_WIDTH_DEFAULT_FACTOR,
	                     EXTRA_WINDOW_HEIGHT_DEFAULT_FACTOR);
}

QPoint BzardNotifications::extraWindowPosition() const {
	return disposition->externalWindowPosition();
}

QSize BzardNotifications::autoWindowSize(double widthFactor,
                                        double heightFactor) const {
	auto screen = disposition->screen()->availableSize();
	auto width = widthFactor * screen.width();
	auto height = heightFactor * screen.height();
	return QSize{static_cast<int>(width), static_cast<int>(height)};
}

bool BzardNotifications::createNotificationIfSpaceAvailable(
	  const BzardNotification &notification) {
	auto size = windowSize();
	auto position = disposition->poses(notification.id, size);
	if (position) {
		auto id = notification.replacesId ? notification.replacesId
		                                  : notification.id;
		emit createNotification(
			  static_cast<int>(id), size, *position, notification.expireTimeout,
			  notification.application, notification.body, notification.title,
			  notification.iconUrl, notification.actions);
		return true;
	} else {
		return false;
	}
}

void BzardNotifications::checkExtraNotifications() {
	while (!extraNotifications.empty() &&
	       createNotificationIfSpaceAvailable(extraNotifications.front())) {
		extraNotifications.pop();
		emit extraNotificationsCountChanged();
	}
}

bool BzardNotifications::shouldShowPopup() const {
	if (fullscreenDetector) {
		if (dontShowWhenFullscreenCurrentDesktop()) {
			auto currenDesktop =
				  fullscreenDetector->fullscreenWindowsOnCurrentDesktop();
			if (currenDesktop)
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
