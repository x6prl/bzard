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

#pragma once

#include <queue>

#include <QObject>
#include <QPoint>
#include <QSize>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#include <gsl/gsl>
#pragma clang diagnostic pop

#include "iqconfig.h"
#include "iqdisposition.h"
#include "iqfullscreendetector.h"
#include "iqnotificationreceiver.h"

class IQNotifications final : public IQNotificationReceiver,
			      public IQConfigurable
{
	Q_OBJECT
	Q_PROPERTY(int extraNotifications READ extraNotificationsCount NOTIFY
		       extraNotificationsCountChanged)
	Q_PROPERTY(QSize extraWindowSize READ extraWindowSize CONSTANT)
	Q_PROPERTY(QPoint extraWindowPos READ extraWindowPos CONSTANT)
	Q_PROPERTY(bool closeAllByRightClick READ closeAllByRightClick CONSTANT)
	Q_PROPERTY(
	    bool closeVisibleByLeftClick READ closeVisibleByLeftClick CONSTANT)
	Q_PROPERTY(bool closeByLeftClick READ closeByLeftClick CONSTANT)
	Q_PROPERTY(bool dontShowWhenFullscreenAny READ dontShowWhenFullscreenAny
		       CONSTANT)

	/*
	 * Changable on-the-fly
	 */
	Q_PROPERTY(bool dontShowWhenFullscreenCurrentDesktop READ
		       dontShowWhenFullscreenCurrentDesktop WRITE
			   setDontShowWhenFullscreenCurrentDesktop NOTIFY
			       dontShowWhenFullscreenCurrentDesktopChanged)

	IQNotifications(IQDisposition::ptr_t disposition_,
			QObject *parent = nullptr);

      public:
	static gsl::not_null<IQNotifications *>
	get(IQDisposition::ptr_t disposition = nullptr);

	void
	setFullscreenDetector(std::unique_ptr<IQFullscreenDetector> detector_);

	QSize extraWindowSize() const;
	QPoint extraWindowPos() const;
	int extraNotificationsCount() const;
	bool closeAllByRightClick() const;
	bool closeVisibleByLeftClick() const;
	bool closeByLeftClick() const;
	bool dontShowWhenFullscreenAny() const;

	bool dontShowWhenFullscreenCurrentDesktop() const;
	void setDontShowWhenFullscreenCurrentDesktop(bool value);

      signals:
	// Signals to QML
	void extraNotificationsCountChanged();
	void createNotification(int notification_id, QSize size, QPoint pos,
				int expire_timeout, const QString &appName,
				const QString &body,
				const QString &title = QString{},
				const QString &iconUrl = QString{},
				const QStringList &actions = {});
	void dropNotification(int notification_id);
	void dropAllVisible();
	void moveNotification(int notification_id, QPoint pos);

	/*
	 * Property changed signals
	 */
	void dontShowWhenFullscreenCurrentDesktopChanged();

      public slots:
	void onCreateNotification(const IQNotification &notification) final;
	void onDropNotification(IQNotification::id_t id) final;

	// QML slots
	void onCloseButtonPressed(int id);
	void onActionButtonPressed(int id, const QString &action);
	void onExpired(int id);
	void onDropAll();
	void onDropStacked();
	void onDropVisible();

      private:
	IQ_CONF_VAR(CLOSE_ALL_BY_RIGHT_CLICK, "close_all_by_right_click", true)
	IQ_CONF_VAR(CLOSE_VISIBLE_BY_MIDDLE_CLICK,
		    "close_visible_by_middle_click", true)
	IQ_CONF_VAR(CLOSE_BY_LEFT_CLICK, "close_by_left_click", false)
	IQ_CONF_VAR(SPACING, "spacing", 0)
	IQ_CONF_FACTOR(GLOBAL_MARGINS, "global_margins", 0.02610966057441253264)
	IQ_CONF_FACTOR(EXTRA_WINDOW_WIDTH, "extra_window_width",
		       0.21961932650073206442)
	IQ_CONF_FACTOR(EXTRA_WINDOW_HEIGHT, "extra_window_height",
		       0.08355091383812010444 / 2)
	IQ_CONF_FACTOR(WIDTH, "width", 0.21961932650073206442)
	IQ_CONF_FACTOR(HEIGHT, "height", 0.28198433420365535248)
	IQ_CONF_VAR(DONT_SHOW_WHEN_FULLSCREEN_ANY,
		    "dont_show_when_fullscreen_any", false)

	/*
	 * Changable on-the-fly
	 */
	IQ_CONF_VAR(DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP,
		    "dont_show_when_fullscreen_current_desktop", false)

	IQDisposition::ptr_t disposition;
	std::queue<IQNotification> extraNotifications;
	std::unique_ptr<IQFullscreenDetector> fullscreenDetector;

	int spacing() const;
	QMargins margins() const;
	QSize windowSize() const;
	QSize windowSize(const QString &width_key, const QString &height_key,
			 double width_factor, double height_factor) const;
	bool createNotificationIfSpaceAvailable(const IQNotification &n);
	void checkExtraNotifications();
	bool shouldShowPopup() const;
};
