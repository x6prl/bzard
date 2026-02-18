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

#include "bzard_notification_modifiers.h"

#include <map>

#include <QDBusArgument>
#include <QFile>
#include <QIcon>
#include <QUrl>

#include <qt6xdg/XdgDirs>
#include <qt6xdg/XdgIcon>

/*
 * Best way found
 * Using private pointers for notification fields
 * in base class lead to sex with '(*ptr)'
 */
#define NOTIFICATION_TO_REFS(NOTIFICATION__)                                   \
	id_t &id = NOTIFICATION__.id;                                              \
	Q_UNUSED(id);                                                              \
	QString &application = NOTIFICATION__.application;                         \
	Q_UNUSED(application);                                                     \
	QString &body = NOTIFICATION__.body;                                       \
	Q_UNUSED(body);                                                            \
	QString &title = NOTIFICATION__.title;                                     \
	Q_UNUSED(title);                                                           \
	QString &iconUrl = NOTIFICATION__.iconUrl;                                 \
	Q_UNUSED(iconUrl);                                                         \
	QStringList &actions = NOTIFICATION__.actions;                             \
	Q_UNUSED(actions);                                                         \
	QVariantMap &hints = NOTIFICATION__.hints;                                 \
	Q_UNUSED(hints);                                                           \
	BzardNotification::ExpireTimeout &expireTimeout =                          \
		  NOTIFICATION__.expireTimeout;                                        \
	Q_UNUSED(expireTimeout);                                                   \
	id_t &replacesId = NOTIFICATION__.replacesId;                              \
	Q_UNUSED(replacesId);

namespace {

template <class K, class V> using MapT = std::map<K, V>;
static MapT<uintmax_t, QString> cachedImages;

bool isCached(uintmax_t hash) {
	return cachedImages.find(hash) != cachedImages.end();
}

QString imageFilenameFromHash(uintmax_t hash) {
	auto result = XdgDirs::cacheHome() + "/bzard-cached_" +
	              QString::number(hash) + ".png";
	return result;
}

/*
 * No ret due to we want to reuse var
 */
void toQmlAbsolutePath(QString &path) {
	if (path.isEmpty())
		return;
	if (path[0] == '/')
		path.insert(0, "file://");
}

template <class T> bool cacheImage(const T &img, uintmax_t hash) {
	auto fileName = imageFilenameFromHash(hash);
	if (img.save(fileName))
		cachedImages[hash] = std::move(fileName);
	else
		return false;
	return true;
}

QString getImageUrlFromHint(const QVariant &argument) {
	int width, height, rowStride, bitsPerSample, channels;
	bool hasAlpha;
	QByteArray data;

	const QDBusArgument ARG = argument.value<QDBusArgument>();
	ARG.beginStructure();
	ARG >> width;
	ARG >> height;
	ARG >> rowStride;
	ARG >> hasAlpha;
	ARG >> bitsPerSample;
	ARG >> channels;
	ARG >> data;
	ARG.endStructure();

	auto hash = qHash(data);

	if (!isCached(hash)) {
		bool rgb = !hasAlpha && channels == 3 && bitsPerSample == 8;
		QImage::Format imageFormat =
			  rgb ? QImage::Format_RGB888 : QImage::Format_ARGB32;

		QImage img = QImage(reinterpret_cast<const uchar *>(data.constData()),
		                    width, height, imageFormat);

		if (!rgb)
			img = img.rgbSwapped();

		if (!cacheImage(img, hash)) {
			return {};
		}
	}
	return cachedImages[hash];
}

QString getImageUrlFromString(const QString &str) {
	static constexpr auto PIXEL_MAP_SIZE = 256;

	QUrl url(str);
	if (url.isValid() && QFile::exists(url.toLocalFile())) {
		return url.toLocalFile();
	} else {
		auto icon = XdgIcon::fromTheme(str);
		auto hash = static_cast<uintmax_t>(icon.cacheKey());

		if (!isCached(hash)) {
			auto pixelMap = icon.pixmap({PIXEL_MAP_SIZE, PIXEL_MAP_SIZE});
			if (!cacheImage(pixelMap, hash)) {
				return {};
			}
		}
		return cachedImages[hash];
	}
}

} // namespace

void BzardNotificationModifiers::IDGenerator::modify(
	  BzardNotification &notification) {
	NOTIFICATION_TO_REFS(notification);
	if (replacesId == 0)
		id = ++lastId;
}

void BzardNotificationModifiers::IconHandler::modify(
	  BzardNotification &notification) {
	NOTIFICATION_TO_REFS(notification);
	if (!hints["image_data"].isNull()) {
		iconUrl = getImageUrlFromHint(hints["image_data"]);
	} else if (!hints["image_path"].isNull()) {
		iconUrl = getImageUrlFromString(hints["image_path"].toString());
	} else if (!iconUrl.isEmpty()) {
		{
			static const QString HTTP{"http://"};
			static const QString HTTPS{"https://"};
			if (iconUrl.startsWith(HTTP, Qt::CaseInsensitive) ||
			    iconUrl.startsWith(HTTPS, Qt::CaseInsensitive))
				return;
		}
		iconUrl = getImageUrlFromString(iconUrl);
	} else if (!hints["icon_data"].isNull()) {
		iconUrl = getImageUrlFromHint(hints["icon_data"]);
	}

	toQmlAbsolutePath(iconUrl);
}

BzardNotificationModifiers::DefaultTimeout::DefaultTimeout()
	  : BzardConfigurable{"default_timeout"} {
	static constexpr auto REAL_DEFAULT{3500};
	defaultTimeout = static_cast<uint16_t>(
		  config.value("default_timeout", REAL_DEFAULT).toUInt());
	if (defaultTimeout == 0)
		defaultTimeout = REAL_DEFAULT;
}

void BzardNotificationModifiers::DefaultTimeout::modify(
	  BzardNotification &notification) {
	NOTIFICATION_TO_REFS(notification);
	if (expireTimeout < 0)
		expireTimeout =
			  static_cast<std::remove_reference_t<decltype(expireTimeout)>>(
					defaultTimeout);
}

BzardNotificationModifiers::TitleToIcon::TitleToIcon()
	  : BzardConfigurable{"title_to_icon"} {}

void BzardNotificationModifiers::TitleToIcon::modify(
	  BzardNotification &notification) {
	NOTIFICATION_TO_REFS(notification);
	if (!iconUrl.isEmpty())
		return;

	if (application.compare(title, Qt::CaseInsensitive) == 0) {
		iconUrl = application.toLower().replace(' ', '-');
	}
}

BzardNotificationModifiers::ReplaceMinusToDash::ReplaceMinusToDash()
	  : BzardConfigurable{"replace_minus_to_dash"} {
	fixTitle = config.value("title", true).toBool();
	fixBody = config.value("body", true).toBool();
}

void BzardNotificationModifiers::ReplaceMinusToDash::modify(
	  BzardNotification &notification) {
	NOTIFICATION_TO_REFS(notification);
	if (fixTitle)
		replaceMinusToDash(title);
	if (fixBody)
		replaceMinusToDash(body);
}

void BzardNotificationModifiers::ReplaceMinusToDash::replaceMinusToDash(
	  QString &str) {
	static QString minus{MINUS_PATTERN}, dash{REPLACE_TO};
	str.replace(minus, dash);
}

BzardNotificationModifiers::BodyToTitleWhenTitleIsAppName::
	  BodyToTitleWhenTitleIsAppName()
	  : BzardConfigurable{"body_to_title_when_title_is_app_name"} {}

void BzardNotificationModifiers::BodyToTitleWhenTitleIsAppName::modify(
	  BzardNotification &notification) {
	NOTIFICATION_TO_REFS(notification);
	if (application.compare(title, Qt::CaseInsensitive) == 0) {
		title = body;
		body = QString{};
	}
}

#undef NOTIFICATION_TO_REFS
