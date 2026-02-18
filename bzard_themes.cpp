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

#include "bzard_themes.h"

#include <QApplication>
#include <QScreen>
#include <QtQml/qqml.h>

namespace {
static QRect availableGeometry() {
	QScreen *screen = QApplication::screens().at(0);
	return screen->availableGeometry();
}
} // namespace

BzardThemes::BzardThemes()
	  : config{"theme"},
		themeName{config.value(CONFIG_THEME_NAME, CONFIG_THEME_NAME_DEFAULT)
                        .toString()} {
	registerThemeTypes();
	loadTheme(themeConfigFile());
	auto themeDir = BzardConfig::configDir() + '/' + themeConfigDir();
	notificationsTheme_ =
		  std::make_unique<NotificationsTheme>(themeConfig, themeDir);
	trayIconTheme_ = std::make_unique<TrayIconTheme>(themeConfig, themeDir);
	historyWindowTheme_ =
		  std::make_unique<HistoryWindowTheme>(themeConfig, themeDir);
}

BzardThemes &BzardThemes::instance() {
	static BzardThemes instance;
	return instance;
}

NotificationsTheme *BzardThemes::notificationsTheme() const {
	return notificationsTheme_.get();
}

TrayIconTheme *BzardThemes::trayIconTheme() const {
	return trayIconTheme_.get();
}

HistoryWindowTheme *BzardThemes::historyWindowTheme() const {
	return historyWindowTheme_.get();
}

QString BzardThemes::themeConfigDir() const { return "themes/" + themeName; }

QString BzardThemes::themeConfigFile() const {
	return themeConfigDir() + "/theme";
}

void BzardThemes::loadTheme(const QString &fileName) {
	themeConfig = std::make_shared<BzardConfig>(QString{}, fileName);
}

void BzardThemes::registerThemeTypes() const {
	qmlRegisterType<NotificationsTheme>("bzard", 1, 0, "NotificationsTheme");
	qmlRegisterType<TrayIconTheme>("bzard", 1, 0, "TrayIconTheme");
	qmlRegisterType<HistoryWindowTheme>("bzard", 1, 0, "HistoryWindowTheme");
}

BzardTheme::BzardTheme(const std::shared_ptr<BzardConfig> &config_,
                       const QString &themeDir_, QObject *parent)
	  : QObject(parent), themeConfig{config_}, themeDir{themeDir_} {}

QUrl BzardTheme::toRelativeUrl(const QString &str) const {
	return "file:///" + themeDir + '/' + str;
}

/*
 *
 * THEMES NEXT
 *
 */

#define BZARD_THEME_UINT(KEY__)                                                \
	themeConfig->value(KEY__, KEY__##_DEFAULT).toUInt()

#define BZARD_THEME_DOUBLE(KEY__)                                              \
	themeConfig->value(KEY__, KEY__##_DEFAULT).toDouble()

#define BZARD_THEME_STRING(KEY__)                                              \
	themeConfig->value(KEY__, KEY__##_DEFAULT).toString()

#define BZARD_THEME_COLOR(KEY__) BZARD_THEME_STRING(KEY__)

#define BZARD_THEME_IMAGE(KEY__)                                               \
	auto str = BZARD_THEME_STRING(KEY__);                                      \
	if (str == KEY__##_DEFAULT)                                                \
		return str;                                                            \
	auto url = toRelativeUrl(str);                                             \
	if (url.isValid())                                                         \
		return url;                                                            \
	else                                                                       \
		return QString{KEY__##_DEFAULT};

bool NotificationsTheme::iconPosition() const {
	auto pos = /*!!!!!*/
		  themeConfig->value(CONFIG_ICON_POSITION, CONFIG_ICON_POSITION_DEFAULT)
				.toString();
	return pos.compare("left", Qt::CaseInsensitive) == 0;
}

uint NotificationsTheme::fontSize() const {
	return BZARD_THEME_UINT(CONFIG_FONT_SIZE);
}

uint NotificationsTheme::barFontSize() const {
	return BZARD_THEME_UINT(CONFIG_BAR_FONT_SIZE);
}

uint NotificationsTheme::iconSize() const {
	return BZARD_THEME_UINT(CONFIG_ICON_SIZE);
}

uint NotificationsTheme::width() const {
	return BZARD_THEME_UINT(CONFIG_WIDTH);
}

uint NotificationsTheme::height() const {
	return BZARD_THEME_UINT(CONFIG_HEIGHT);
}

uint NotificationsTheme::extraWindowWidth() const {
	return BZARD_THEME_UINT(CONFIG_EXTRA_WINDOW_WIDTH);
}

uint NotificationsTheme::extraWindowHeight() const {
	return BZARD_THEME_UINT(CONFIG_EXTRA_WINDOW_HEIGHT);
}

uint NotificationsTheme::barHeight() const {
	return BZARD_THEME_UINT(CONFIG_BAR_HEIGHT);
}

uint NotificationsTheme::expirationBarHeight() const {
	return BZARD_THEME_UINT(CONFIG_EXPIRATION_BAR_HEIGHT);
}

uint NotificationsTheme::showAnimationDuration() const {
	return BZARD_THEME_UINT(CONFIG_SHOW_DURATION);
}

uint NotificationsTheme::dropAnimationDuration() const {
	return BZARD_THEME_UINT(CONFIG_DROP_DURATION);
}

double NotificationsTheme::closeButtonImageScale() const {
	return BZARD_THEME_DOUBLE(CONFIG_CLOSE_BUTTON_IMAGE_SCALE);
}

double NotificationsTheme::extraButtonImageScale() const {
	return BZARD_THEME_DOUBLE(CONFIG_EXTRA_BUTTON_IMAGE_SCALE);
}

QColor NotificationsTheme::bgColor() const {
	return BZARD_THEME_COLOR(CONFIG_BG_COLOR);
}

QColor NotificationsTheme::barBgColor() const {
	return BZARD_THEME_COLOR(CONFIG_BAR_BG_COLOR);
}

QColor NotificationsTheme::barTextColor() const {
	return BZARD_THEME_COLOR(CONFIG_BAR_TEXT_COLOR);
}

QColor NotificationsTheme::expirationBarColor() const {
	return BZARD_THEME_COLOR(CONFIG_EXPIRATION_BAR_COLOR);
}

QColor NotificationsTheme::titleTextColor() const {
	return BZARD_THEME_COLOR(CONFIG_TITLE_TEXT_COLOR);
}

QColor NotificationsTheme::bodyTextColor() const {
	return BZARD_THEME_COLOR(CONFIG_BODY_TEXT_COLOR);
}

QColor NotificationsTheme::buttonBgColor() const {
	return BZARD_THEME_COLOR(CONFIG_BUTTON_BG_COLOR);
}

QColor NotificationsTheme::buttonTextColor() const {
	return BZARD_THEME_COLOR(CONFIG_BUTTON_TEXT_COLOR);
}

QColor NotificationsTheme::extraBgColor() const {
	return BZARD_THEME_COLOR(CONFIG_EXTRA_BG_COLOR);
}

QColor NotificationsTheme::extraUreadCircleColor() const {
	return BZARD_THEME_COLOR(CONFIG_EXTRA_UNREAD_CIRCLE_COLOR);
}

QColor NotificationsTheme::extraUreadTextColor() const {
	return BZARD_THEME_COLOR(CONFIG_EXTRA_UNREAD_TEXT_COLOR);
}

QUrl NotificationsTheme::bgImage() const {BZARD_THEME_IMAGE(CONFIG_BG_IMAGE)}

QUrl NotificationsTheme::closeButtonImage() const {
	  BZARD_THEME_IMAGE(CONFIG_CLOSE_BUTTON_IMAGE)}

QUrl NotificationsTheme::extraCloseButtonImage() const {
	  BZARD_THEME_IMAGE(CONFIG_EXTRA_CLOSE_BUTTON_IMAGE)}

QUrl NotificationsTheme::extraCloseAllButtonImage() const {
	  BZARD_THEME_IMAGE(CONFIG_EXTRA_CLOSE_ALL_BUTTON_IMAGE)}

QUrl NotificationsTheme::extraCloseVisibleButtonImage() const {
	  BZARD_THEME_IMAGE(CONFIG_EXTRA_CLOSE_VISIBLE_BUTTON_IMAGE)}

QUrl TrayIconTheme::icon() const {BZARD_THEME_IMAGE(CONFIG_ICON)}

QUrl HistoryWindowTheme::closeIcon() const {
	  BZARD_THEME_IMAGE(CONFIG_CLOSE_ICON)}

QUrl HistoryWindowTheme::bgImage() const {BZARD_THEME_IMAGE(CONFIG_BG_IMAGE)}

QString HistoryWindowTheme::windowTitle() const {
	return BZARD_THEME_STRING(CONFIG_WINDOW_TITLE);
}

uint HistoryWindowTheme::x() const {
	switch (windowPosition()) {
	case WP_LEFT_BOT:
	case WP_LEFT_TOP:
		return availableGeometry().left();
		break;
	case WP_RIGHT_BOT:
	case WP_RIGHT_TOP:
		return availableGeometry().x() + availableGeometry().width() - width();
		break;
	default:
		break;
	}
	return BZARD_THEME_UINT(CONFIG_X);
}

uint HistoryWindowTheme::y() const {
	switch (windowPosition()) {
	case WP_RIGHT_TOP:
	case WP_LEFT_TOP:
		return availableGeometry().top();
		break;
	case WP_RIGHT_BOT:
	case WP_LEFT_BOT:
		return availableGeometry().y() + availableGeometry().height() -
		       height();
		break;
	default:
		break;
	}
	return BZARD_THEME_UINT(CONFIG_Y);
}

uint HistoryWindowTheme::height() const {
	return BZARD_THEME_UINT(CONFIG_HEIGHT);
}

uint HistoryWindowTheme::width() const {
	return BZARD_THEME_UINT(CONFIG_WIDTH);
}

uint HistoryWindowTheme::barHeight() const {
	return BZARD_THEME_UINT(CONFIG_BAR_HEIGHT);
}

uint HistoryWindowTheme::notificationHeight() const {
	return BZARD_THEME_UINT(CONFIG_NOTIFICATION_HEIGHT);
}

uint HistoryWindowTheme::barFontSize() const {
	return BZARD_THEME_UINT(CONFIG_BAR_FONT_SIZE);
}

uint HistoryWindowTheme::nAppFontSize() const {
	return BZARD_THEME_UINT(CONFIG_NAPP_FONT_SIZE);
}

uint HistoryWindowTheme::nTitleFontSize() const {
	return BZARD_THEME_UINT(CONFIG_NTITLE_FONT_SIZE);
}

uint HistoryWindowTheme::nBodyFontSize() const {
	return BZARD_THEME_UINT(CONFIG_NBODY_FONT_SIZE);
}

QString HistoryWindowTheme::bgColor() const {
	return BZARD_THEME_STRING(CONFIG_BG_COLOR);
}

QString HistoryWindowTheme::barBgColor() const {
	return BZARD_THEME_STRING(CONFIG_BAR_BG_COLOR);
}

QString HistoryWindowTheme::barTextColor() const {
	return BZARD_THEME_STRING(CONFIG_BAR_TEXT_COLOR);
}

QString HistoryWindowTheme::nBgColor() const {
	return BZARD_THEME_STRING(CONFIG_NBG_COLOR);
}

QString HistoryWindowTheme::nAppTextColor() const {
	return BZARD_THEME_STRING(CONFIG_NAPP_TEXT_COLOR);
}

QString HistoryWindowTheme::nTitleTextColor() const {
	return BZARD_THEME_STRING(CONFIG_NTITLE_TEXT_COLOR);
}

QString HistoryWindowTheme::nBodyTextColor() const {
	return BZARD_THEME_STRING(CONFIG_NBODY_TEXT_COLOR);
}

HistoryWindowTheme::WindowPosition HistoryWindowTheme::windowPosition() const {
	return static_cast<HistoryWindowTheme::WindowPosition>(
		  BZARD_THEME_UINT(CONFIG_WINDOW_POSITION));
}

#undef BZARD_THEME_IMAGE
#undef BZARD_THEME_STRING
#undef BZARD_THEME_COLOR
#undef BZARD_THEME_DOUBLE
#undef BZARD_THEME_UINT
