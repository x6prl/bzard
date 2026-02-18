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

#include <queue>

#include <QObject>
#include <QPoint>
#include <QSize>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic pop

#include "bzard_config.h"
#include "bzard_disposition.h"
#include "bzard_fullscreen_detector.h"
#include "bzard_notification_receiver.h"

class BzardNotifications final : public BzardNotificationReceiver,
								 public BzardConfigurable {
	Q_OBJECT
	Q_PROPERTY(int extraNotifications READ extraNotificationsCount NOTIFY
	                 extraNotificationsCountChanged)
	Q_PROPERTY(QSize extraWindowSize READ extraWindowSize CONSTANT)
	Q_PROPERTY(QPoint extraWindowPosition READ extraWindowPosition CONSTANT)
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

	BzardNotifications(BzardDisposition::PtrT disposition_,
	                   QObject *parent = nullptr);

  public:
	static BzardNotifications *
	get(BzardDisposition::PtrT disposition = nullptr);

	void
	setFullscreenDetector(std::unique_ptr<BzardFullscreenDetector> detector_);

	QSize extraWindowSize() const;
	QPoint extraWindowPosition() const;
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
	void createNotification(int notificationId, QSize size, QPoint position,
	                        int expireTimeout, const QString &APP_NAME,
	                        const QString &BODY,
	                        const QString &TITLE = QString{},
	                        const QString &ICON_URL = QString{},
	                        const QStringList &ACTIONS = {});
	void dropNotification(int notificatioId);
	void dropAllVisible();
	void moveNotification(int notificationId, QPoint position);

	/*
	 * Property changed signals
	 */
	void dontShowWhenFullscreenCurrentDesktopChanged();

  public slots:
	void onCreateNotification(const BzardNotification &NOTIFICATION) final;
	void onDropNotification(BzardNotification::IdT id) final;

	// QML slots
	void onCloseButtonPressed(int id);
	void onActionButtonPressed(int id, const QString &ACTION);
	void onExpired(int id);
	void onDropAll();
	void onDropStacked();
	void onDropVisible();

  private:
	BZARD_CONFIG_VAR(CLOSE_ALL_BY_RIGHT_CLICK, "close_all_by_right_click", true)
	BZARD_CONFIG_VAR(CLOSE_VISIBLE_BY_MIDDLE_CLICK,
	                 "close_visible_by_middle_click", true)
	BZARD_CONFIG_VAR(CLOSE_BY_LEFT_CLICK, "close_by_left_click", false)
	BZARD_CONFIG_VAR(SPACING, "spacing", 0)
	BZARD_CONFIG_FACTOR(GLOBAL_MARGINS, "global_margins",
	                    0.02610966057441253264)
	BZARD_CONFIG_VAR(DONT_SHOW_WHEN_FULLSCREEN_ANY,
	                 "dont_show_when_fullscreen_any", false)

	/*
	 * Changable on-the-fly
	 */
	BZARD_CONFIG_VAR(DONT_SHOW_WHEN_FULLSCREEN_CURRENT_DESKTOP,
	                 "dont_show_when_fullscreen_current_desktop", false)

	BzardDisposition::PtrT disposition;
	std::queue<BzardNotification> extraNotifications;
	std::unique_ptr<BzardFullscreenDetector> fullscreenDetector;

	static constexpr double WIDTH_DEFAULT_FACTOR = 0.21961932650073206442;
	static constexpr double HEIGHT_DEFAULT_FACTOR = 0.28198433420365535248;
	static constexpr double EXTRA_WINDOW_WIDTH_DEFAULT_FACTOR =
		  0.21961932650073206442;
	static constexpr double EXTRA_WINDOW_HEIGHT_DEFAULT_FACTOR =
		  0.08355091383812010444 / 2;

	int spacing() const;
	QMargins margins() const;
	QSize windowSize() const;
	QSize autoWindowSize(double widthFactor, double heightFactor) const;
	bool
	createNotificationIfSpaceAvailable(const BzardNotification &notification);
	void checkExtraNotifications();
	bool shouldShowPopup() const;
};
