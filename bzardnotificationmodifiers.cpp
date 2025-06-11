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

#include "bzardnotificationmodifiers.h"

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
#define N_TO_REFS(N__)                                                         \
	id_t &id = N__.id;                                                     \
	Q_UNUSED(id);                                                          \
	QString &application = N__.application;                                \
	Q_UNUSED(application);                                                 \
	QString &body = N__.body;                                              \
	Q_UNUSED(body);                                                        \
	QString &title = N__.title;                                            \
	Q_UNUSED(title);                                                       \
	QString &icon_url = N__.icon_url;                                      \
	Q_UNUSED(icon_url);                                                    \
	QStringList &actions = N__.actions;                                    \
	Q_UNUSED(actions);                                                     \
	QVariantMap &hints = N__.hints;                                        \
	Q_UNUSED(hints);                                                       \
	IQNotification::ExpireTimeout &expire_timeout = N__.expire_timeout;    \
	Q_UNUSED(expire_timeout);                                              \
	id_t &replaces_id = N__.replaces_id;                                   \
	Q_UNUSED(replaces_id);

namespace
{

template <class K, class V> using map_t = std::map<K, V>;
static map_t<uintmax_t, QString> cached_images;

bool isCached(uintmax_t hash)
{
	return cached_images.find(hash) != cached_images.end();
}

QString imageFilenameFromHash(uintmax_t hash)
{
	auto ret = XdgDirs::cacheHome() + "/iq-cached_" +
		   QString::number(hash) + ".png";
	return ret;
}

/*
 * No ret due to we want to reuse var
 */
void toQmlAbsolutePath(QString &path)
{
	if (path.isEmpty())
		return;
	if (path[0] == '/')
		path.insert(0, "file://");
}

template <class T> bool cacheImage(const T &img, uintmax_t hash)
{
	auto fname = imageFilenameFromHash(hash);
	if (img.save(fname))
		cached_images[hash] = std::move(fname);
	else
		return false;
	return true;
}

QString getImageUrlFromHint(const QVariant &argument)
{
	int width, height, rowstride, bitsPerSample, channels;
	bool hasAlpha;
	QByteArray data;

	const QDBusArgument arg = argument.value<QDBusArgument>();
	arg.beginStructure();
	arg >> width;
	arg >> height;
	arg >> rowstride;
	arg >> hasAlpha;
	arg >> bitsPerSample;
	arg >> channels;
	arg >> data;
	arg.endStructure();

	auto hash = qHash(data);

	if (!isCached(hash)) {
		bool rgb = !hasAlpha && channels == 3 && bitsPerSample == 8;
		QImage::Format imageFormat =
		    rgb ? QImage::Format_RGB888 : QImage::Format_ARGB32;

		QImage img =
		    QImage(reinterpret_cast<const uchar *>(data.constData()),
			   width, height, imageFormat);

		if (!rgb)
			img = img.rgbSwapped();

		if (!cacheImage(img, hash)) {
			return {};
		}
	}
	return cached_images[hash];
}

QString getImageUrlFromString(const QString &str)
{
	static constexpr auto PIXMAP_SIZE = 256;

	QUrl url(str);
	if (url.isValid() && QFile::exists(url.toLocalFile())) {
		return url.toLocalFile();
	} else {
		// TODO: OMFG????????
		auto icon = XdgIcon::fromTheme(str);
		auto hash = static_cast<uintmax_t>(icon.cacheKey());

		if (!isCached(hash)) {
			auto pixmap = icon.pixmap({PIXMAP_SIZE, PIXMAP_SIZE});
			if (!cacheImage(pixmap, hash)) {
				return {};
			}
		}
		return cached_images[hash];
	}
}

} // anonymouse namespace

void BzardNotificationModifiers::IDGenerator::modify(IQNotification &notification)
{
	N_TO_REFS(notification);
	if (replaces_id == 0)
		id = ++last_id;
}

/*
 * Based on lxqt notification daemon
 */
void BzardNotificationModifiers::IconHandler::modify(IQNotification &notification)
{
	N_TO_REFS(notification);
	if (!hints["image_data"].isNull()) {
		icon_url = getImageUrlFromHint(hints["image_data"]);
	} else if (!hints["image_path"].isNull()) {
		icon_url =
		    getImageUrlFromString(hints["image_path"].toString());
	} else if (!icon_url.isEmpty()) {
		/*
		 *  Check, is it web URL
		 */
		{
			static const QString http{"http://"};
			static const QString https{"https://"};
			if (icon_url.startsWith(http, Qt::CaseInsensitive) ||
			    icon_url.startsWith(https, Qt::CaseInsensitive))
				return;
		}
		icon_url = getImageUrlFromString(icon_url);
	} else if (!hints["icon_data"].isNull()) {
		icon_url = getImageUrlFromHint(hints["icon_data"]);
	}

	toQmlAbsolutePath(icon_url);
}

BzardNotificationModifiers::DefaultTimeout::DefaultTimeout()
    : IQConfigurable{"default_timeout"}
{
	static constexpr auto real_default{3500};
	defaultTimeout = static_cast<uint16_t>(
	    config.value("default_timeout", real_default).toUInt());
	if (defaultTimeout == 0)
		defaultTimeout = real_default;
}

void BzardNotificationModifiers::DefaultTimeout::modify(
    IQNotification &notification)
{
	N_TO_REFS(notification);
	if (expire_timeout < 0)
		expire_timeout = static_cast<
		    std::remove_reference_t<decltype(expire_timeout)>>(
		    defaultTimeout);
}

BzardNotificationModifiers::TitleToIcon::TitleToIcon()
    : IQConfigurable{"title_to_icon"}
{
}

void BzardNotificationModifiers::TitleToIcon::modify(IQNotification &notification)
{
	N_TO_REFS(notification);
	// Do nothing if icon presented
	if (!icon_url.isEmpty())
		return;

	if (application.compare(title, Qt::CaseInsensitive) == 0) {
		icon_url = application.toLower().replace(' ', '-');
	}
}

BzardNotificationModifiers::ReplaceMinusToDash::ReplaceMinusToDash()
    : IQConfigurable{"replace_minus_to_dash"}
{
	fixTitle = config.value("title", true).toBool();
	fixBody = config.value("body", true).toBool();
}

void BzardNotificationModifiers::ReplaceMinusToDash::modify(
    IQNotification &notification)
{
	N_TO_REFS(notification);
	if (fixTitle)
		replaceMinusToDash(title);
	if (fixBody)
		replaceMinusToDash(body);
}

void BzardNotificationModifiers::ReplaceMinusToDash::replaceMinusToDash(
    QString &str)
{
	static QString minus{minusPattern}, dash{replaceTo};
	str.replace(minus, dash);
}

BzardNotificationModifiers::BodyToTitleWhenTitleIsAppName::
    BodyToTitleWhenTitleIsAppName()
    : IQConfigurable{"body_to_title_when_title_is_app_name"}
{
}

void BzardNotificationModifiers::BodyToTitleWhenTitleIsAppName::modify(
    IQNotification &notification)
{
	N_TO_REFS(notification);
	if (application.compare(title, Qt::CaseInsensitive) == 0) {
		title = body;
		body = QString{};
	}
}

#undef N_TO_REFS
