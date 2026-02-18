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

#include <memory>

#include <QColor>
#include <QObject>
#include <QUrl>

#include "bzard_config.h"

class NotificationsTheme;
class TrayIconTheme;
class HistoryWindowTheme;

class BzardThemes : public QObject {
	Q_OBJECT
	Q_PROPERTY(NotificationsTheme *notificationsTheme READ notificationsTheme
	                 CONSTANT)
	Q_PROPERTY(TrayIconTheme *trayIconTheme READ trayIconTheme CONSTANT)
	Q_PROPERTY(HistoryWindowTheme *historyWindowTheme READ historyWindowTheme
	                 CONSTANT)

  public:
	BzardThemes();
	static BzardThemes &instance();

	NotificationsTheme *notificationsTheme() const;
	TrayIconTheme *trayIconTheme() const;
	HistoryWindowTheme *historyWindowTheme() const;

  private:
	BZARD_CONFIG_VAR(THEME_NAME, "theme_name", "default")

	BzardConfig config;
	const QString themeName;
	std::shared_ptr<BzardConfig> themeConfig;
	std::unique_ptr<NotificationsTheme> notificationsTheme_;
	std::unique_ptr<TrayIconTheme> trayIconTheme_;
	std::unique_ptr<HistoryWindowTheme> historyWindowTheme_;

	QString themeConfigDir() const;
	QString themeConfigFile() const;
	void loadTheme(const QString &fileName);
	void registerThemeTypes() const;
};

class BzardTheme : public QObject {
	Q_OBJECT
  public:
	BzardTheme() = default;
	BzardTheme(const std::shared_ptr<BzardConfig> &config_,
	           const QString &themeDir_, QObject *parent = nullptr);

  protected:
	std::shared_ptr<BzardConfig> themeConfig;
	const QString themeDir;

	QUrl toRelativeUrl(const QString &str) const;
};

class NotificationsTheme : public BzardTheme {
	Q_OBJECT

	Q_PROPERTY(bool iconPosition READ iconPosition CONSTANT)
	Q_PROPERTY(uint fontSize READ fontSize CONSTANT)
	Q_PROPERTY(uint barFontSize READ barFontSize CONSTANT)
	Q_PROPERTY(uint iconSize READ iconSize CONSTANT)
	Q_PROPERTY(uint width READ width CONSTANT)
	Q_PROPERTY(uint height READ height CONSTANT)
	Q_PROPERTY(uint extraWindowWidth READ extraWindowWidth CONSTANT)
	Q_PROPERTY(uint extraWindowHeight READ extraWindowHeight CONSTANT)
	Q_PROPERTY(uint barHeight READ barHeight CONSTANT)
	Q_PROPERTY(uint expirationBarHeight READ expirationBarHeight CONSTANT)
	Q_PROPERTY(uint showAnimationDuration READ showAnimationDuration CONSTANT)
	Q_PROPERTY(uint dropAnimationDuration READ dropAnimationDuration CONSTANT)
	Q_PROPERTY(double closeButtonImageScale READ closeButtonImageScale CONSTANT)
	Q_PROPERTY(double extraButtonImageScale READ extraButtonImageScale CONSTANT)
	Q_PROPERTY(QColor bgColor READ bgColor CONSTANT)
	Q_PROPERTY(QColor barBgColor READ barBgColor CONSTANT)
	Q_PROPERTY(QColor barTextColor READ barTextColor CONSTANT)
	Q_PROPERTY(QColor expirationBarColor READ expirationBarColor CONSTANT)
	Q_PROPERTY(QColor titleTextColor READ titleTextColor CONSTANT)
	Q_PROPERTY(QColor bodyTextColor READ bodyTextColor CONSTANT)
	Q_PROPERTY(QColor buttonBgColor READ buttonBgColor CONSTANT)
	Q_PROPERTY(QColor buttonTextColor READ buttonTextColor CONSTANT)
	Q_PROPERTY(QColor extraBgColor READ extraBgColor CONSTANT)
	Q_PROPERTY(QColor extraUreadCircleColor READ extraUreadCircleColor CONSTANT)
	Q_PROPERTY(QColor extraUreadTextColor READ extraUreadTextColor CONSTANT)
	Q_PROPERTY(QUrl bgImage READ bgImage CONSTANT)
	Q_PROPERTY(QUrl closeButtonImage READ closeButtonImage CONSTANT)
	Q_PROPERTY(QUrl extraCloseButtonImage READ extraCloseButtonImage CONSTANT)
	Q_PROPERTY(
		  QUrl extraCloseAllButtonImage READ extraCloseAllButtonImage CONSTANT)
	Q_PROPERTY(QUrl extraCloseVisibleButtonImage READ
	                 extraCloseVisibleButtonImage CONSTANT)

  public:
	using BzardTheme::BzardTheme;

	// True to move to left side, false for top
	bool iconPosition() const;
	uint fontSize() const;
	uint barFontSize() const;
	uint iconSize() const;
	uint width() const;
	uint height() const;
	uint extraWindowWidth() const;
	uint extraWindowHeight() const;
	uint barHeight() const;
	uint expirationBarHeight() const;
	uint showAnimationDuration() const;
	uint dropAnimationDuration() const;
	double closeButtonImageScale() const;
	double extraButtonImageScale() const;
	QColor bgColor() const;
	QColor barBgColor() const;
	QColor barTextColor() const;
	QColor expirationBarColor() const;
	QColor titleTextColor() const;
	QColor bodyTextColor() const;
	QColor buttonBgColor() const;
	QColor buttonTextColor() const;
	QColor extraBgColor() const;
	QColor extraUreadCircleColor() const;
	QColor extraUreadTextColor() const;
	QUrl bgImage() const;
	QUrl closeButtonImage() const;
	QUrl extraCloseButtonImage() const;
	QUrl extraCloseAllButtonImage() const;
	QUrl extraCloseVisibleButtonImage() const;

  private:
	BZARD_CONFIG_VAR(ICON_POSITION, "popup_notifications/icon_position", "top")

	BZARD_CONFIG_VAR(FONT_SIZE, "popup_notifications/font_size", 0)
	BZARD_CONFIG_VAR(BAR_FONT_SIZE, "popup_notifications/bar_font_size", 0)
	BZARD_CONFIG_VAR(ICON_SIZE, "popup_notifications/icon_size", 0)
	BZARD_CONFIG_VAR(WIDTH, "popup_notifications/width", 0)
	BZARD_CONFIG_VAR(HEIGHT, "popup_notifications/height", 0)
	BZARD_CONFIG_VAR(EXTRA_WINDOW_WIDTH,
	                 "popup_notifications/extra_window_width", 0)
	BZARD_CONFIG_VAR(EXTRA_WINDOW_HEIGHT,
	                 "popup_notifications/extra_window_height", 0)
	BZARD_CONFIG_VAR(BAR_HEIGHT, "popup_notifications/bar_height", 0)
	BZARD_CONFIG_VAR(EXPIRATION_BAR_HEIGHT,
	                 "popup_notifications/expiration_bar_height", 0)
	BZARD_CONFIG_VAR(SHOW_DURATION,
	                 "popup_notifications/show_animation_duration", 120)
	BZARD_CONFIG_VAR(DROP_DURATION,
	                 "popup_notifications/drop_animation_duration", 120)
	BZARD_CONFIG_VAR(CLOSE_BUTTON_IMAGE_SCALE,
	                 "popup_notifications/close_button_image_scale", 0.4)
	BZARD_CONFIG_VAR(EXTRA_BUTTON_IMAGE_SCALE,
	                 "popup_notifications/extra_button_image_scale", 0.6)

	BZARD_CONFIG_VAR(BG_COLOR, "popup_notifications/bg_color", "#19202d")
	BZARD_CONFIG_VAR(BAR_BG_COLOR, "popup_notifications/bar_bg_color",
	                 "#262d3a")
	BZARD_CONFIG_VAR(BAR_TEXT_COLOR, "popup_notifications/bar_text_color",
	                 "#92969c")
	BZARD_CONFIG_VAR(EXPIRATION_BAR_COLOR,
	                 "popup_notifications/expiration_bar_color", "#30394a")
	BZARD_CONFIG_VAR(TITLE_TEXT_COLOR, "popup_notifications/title_text_color",
	                 "#ffffff")
	BZARD_CONFIG_VAR(BODY_TEXT_COLOR, "popup_notifications/body_text_color",
	                 "#92969c")
	BZARD_CONFIG_VAR(BUTTON_BG_COLOR, "popup_notifications/button_bg_color",
	                 "#343b4d")
	BZARD_CONFIG_VAR(BUTTON_TEXT_COLOR, "popup_notifications/button_text_color",
	                 "#ffffff")
	BZARD_CONFIG_VAR(EXTRA_BG_COLOR, "popup_notifications/extra_bg_color",
	                 "#262d3a")
	BZARD_CONFIG_VAR(EXTRA_UNREAD_CIRCLE_COLOR,
	                 "popup_notifications/extra_unread_circle_color", "#d74a37")
	BZARD_CONFIG_VAR(EXTRA_UNREAD_TEXT_COLOR,
	                 "popup_notifications/extra_unread_text_color", "#ffffff")

	BZARD_CONFIG_VAR(BG_IMAGE, "popup_notifications/bg_image", "")
	BZARD_CONFIG_VAR(CLOSE_BUTTON_IMAGE,
	                 "popup_notifications/close_button_image", "img/close.png")
	BZARD_CONFIG_VAR(EXTRA_CLOSE_BUTTON_IMAGE,
	                 "popup_notifications/extra_close_button_image",
	                 "img/close.png")
	BZARD_CONFIG_VAR(EXTRA_CLOSE_ALL_BUTTON_IMAGE,
	                 "popup_notifications/extra_close_all_button_image",
	                 "img/closeAll.png")
	BZARD_CONFIG_VAR(EXTRA_CLOSE_VISIBLE_BUTTON_IMAGE,
	                 "popup_notifications/extra_close_visible_button_image",
	                 "img/closeVisible.png")
};

class TrayIconTheme : public BzardTheme {
	Q_OBJECT
	Q_PROPERTY(QUrl icon READ icon CONSTANT)
  public:
	using BzardTheme::BzardTheme;

	QUrl icon() const;

  private:
	BZARD_CONFIG_VAR(ICON, "tray/icon", "img/warning.png")
};

class HistoryWindowTheme : public BzardTheme {
	Q_OBJECT
	Q_PROPERTY(QUrl closeIcon READ closeIcon CONSTANT)
	Q_PROPERTY(QUrl bgImage READ bgImage CONSTANT)
	Q_PROPERTY(QString windowTitle READ windowTitle CONSTANT)
	Q_PROPERTY(uint x READ x CONSTANT)
	Q_PROPERTY(uint y READ y CONSTANT)
	Q_PROPERTY(uint height READ height CONSTANT)
	Q_PROPERTY(uint width READ width CONSTANT)
	Q_PROPERTY(uint barHeight READ barHeight CONSTANT)
	Q_PROPERTY(uint notificationHeight READ notificationHeight CONSTANT)
	Q_PROPERTY(uint barFontSize READ barFontSize CONSTANT)
	Q_PROPERTY(uint nAppFontSize READ nAppFontSize CONSTANT)
	Q_PROPERTY(uint nTitleFontSize READ nTitleFontSize CONSTANT)
	Q_PROPERTY(uint nBodyFontSize READ nBodyFontSize CONSTANT)
	Q_PROPERTY(QString bgColor READ bgColor CONSTANT)
	Q_PROPERTY(QString barBgColor READ barBgColor CONSTANT)
	Q_PROPERTY(QString barTextColor READ barTextColor CONSTANT)
	Q_PROPERTY(QString nBgColor READ nBgColor CONSTANT)
	Q_PROPERTY(QString nAppTextColor READ nAppTextColor CONSTANT)
	Q_PROPERTY(QString nTitleTextColor READ nTitleTextColor CONSTANT)
	Q_PROPERTY(QString nBodyTextColor READ nBodyTextColor CONSTANT)

  public:
	using BzardTheme::BzardTheme;
	QUrl closeIcon() const;
	QUrl bgImage() const;
	QString windowTitle() const;

	uint x() const;
	uint y() const;
	uint height() const;
	uint width() const;
	uint barHeight() const;
	uint notificationHeight() const;
	uint barFontSize() const;
	uint nAppFontSize() const;
	uint nTitleFontSize() const;
	uint nBodyFontSize() const;

	QString bgColor() const;
	QString barBgColor() const;
	QString barTextColor() const;
	QString nBgColor() const;
	QString nAppTextColor() const;
	QString nTitleTextColor() const;
	QString nBodyTextColor() const;

  private:
	enum WindowPosition {
		WP_UNDEFINED = 0,
		WP_LEFT_TOP,
		WP_LEFT_BOT,
		WP_RIGHT_BOT,
		WP_RIGHT_TOP
	};
	WindowPosition windowPosition() const;

	BZARD_CONFIG_VAR(CLOSE_ICON, "history_window/close_icon", "img/close.png")
	BZARD_CONFIG_VAR(BG_IMAGE, "history_window/bg_image", "")
	BZARD_CONFIG_VAR(WINDOW_TITLE, "history_window/window_title", "bzard")
	BZARD_CONFIG_VAR(WINDOW_POSITION, "history_window/window_position",
	                 WP_UNDEFINED)

	BZARD_CONFIG_VAR(X, "history_window/x", 0)
	BZARD_CONFIG_VAR(Y, "history_window/y", 0)
	BZARD_CONFIG_VAR(HEIGHT, "history_window/height", 0)
	BZARD_CONFIG_VAR(WIDTH, "history_window/width", 0)
	BZARD_CONFIG_VAR(BAR_HEIGHT, "history_window/bar_height", 32)
	BZARD_CONFIG_VAR(NOTIFICATION_HEIGHT, "history_window/notification_height",
	                 0)
	BZARD_CONFIG_VAR(BAR_FONT_SIZE, "history_window/bar_font_size", 0)
	BZARD_CONFIG_VAR(NAPP_FONT_SIZE, "history_window/napp_font_size", 0)
	BZARD_CONFIG_VAR(NTITLE_FONT_SIZE, "history_window/ntitle_font_size", 0)
	BZARD_CONFIG_VAR(NBODY_FONT_SIZE, "history_window/nbody_font_size", 0)

	BZARD_CONFIG_VAR(BG_COLOR, "history_window/bg_color", "#262d3a")
	BZARD_CONFIG_VAR(BAR_BG_COLOR, "history_window/bar_bg_color", "#262d3a")
	BZARD_CONFIG_VAR(BAR_TEXT_COLOR, "history_window/bar_text_color", "#92969c")
	BZARD_CONFIG_VAR(NBG_COLOR, "history_window/nbg_color", "#19202d")
	BZARD_CONFIG_VAR(NAPP_TEXT_COLOR, "history_window/napp_text_color",
	                 "#92969c")
	BZARD_CONFIG_VAR(NTITLE_TEXT_COLOR, "history_window/ntitle_text_color",
	                 "white")
	BZARD_CONFIG_VAR(NBODY_TEXT_COLOR, "history_window/nbody_text_color",
	                 "#92969c")
};
