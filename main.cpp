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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtDBus/QDBusConnection>
#include <QtQml>

#include "notificationsadaptor.h"

#include "bzard_dbus_service.h"
#include "bzard_expiration_controller.h"
#include "bzard_history.h"
#include "bzard_notification_modifiers.h"
#include "bzard_notifications.h"
#include "bzard_themes.h"
#include "bzard_top_down.h"
#include "bzard_tray_icon.h"

#ifdef BZARD_X11
#include "X11-plugin/x11fullscreendetector.h"
#endif

static BzardDBusService *get_service();
static BzardHistory *get_history();
static QDBusConnection connect_to_session_bus(BzardDBusService *service);
static QObject *bzardnotifications_provider(QQmlEngine *engine,
                                            QJSEngine *scriptEngine);
static QObject *bzardthemes_provider(QQmlEngine *engine,
                                     QJSEngine *scriptEngine);

BzardDBusService *get_service() {
	using namespace BzardNotificationModifiers;

	auto disposition = std::make_unique<BzardTopDown>();
	auto dbus_service =
		  (new BzardDBusService)
				->addModifier(make<IDGenerator>())
				->addModifier(make<TitleToIcon>())
				->addModifier(make<IconHandler>())
				->addModifier(make<BodyToTitleWhenTitleIsAppName>())
				->addModifier(make<DefaultTimeout>())
				->addModifier(make<ReplaceMinusToDash>());

	auto notifications = BzardNotifications::get(std::move(disposition));
	if (notifications->isEnabled())
		dbus_service->connectReceiver(notifications);
	if (get_history()->isEnabled())
		dbus_service->connectReceiver(get_history());

	std::unique_ptr<BzardFullscreenDetector> fullscreenDetector;
#ifdef BZARD_X11
	fullscreenDetector = std::make_unique<X11FullscreenDetector>();
#endif
	notifications->setFullscreenDetector(std::move(fullscreenDetector));
	return dbus_service;
}

BzardHistory *get_history() {
	static BzardHistory history;
	return &history;
}

QObject *bzardnotifications_provider(QQmlEngine *engine,
                                     QJSEngine *scriptEngine) {
	Q_UNUSED(engine);
	Q_UNUSED(scriptEngine);
	return BzardNotifications::get();
}

QObject *bzardthemes_provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
	Q_UNUSED(engine);
	Q_UNUSED(scriptEngine);
	return &BzardThemes::instance();
}

QObject *bzardhistory_provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
	Q_UNUSED(engine);
	Q_UNUSED(scriptEngine);
	return get_history();
}

QDBusConnection connect_to_session_bus(BzardDBusService *service) {
	auto connection = QDBusConnection::sessionBus();
	new NotificationsAdaptor(service);

	if (!connection.registerService("org.freedesktop.Notifications")) {
		throw std::runtime_error{"DBus Service already registered!"};
	}

	if (!connection.registerObject("/org/freedesktop/Notifications", service)) {
		throw std::runtime_error{"Can't register DBus service object!"};
	}
	return connection;
}

int main(int argc, char *argv[]) {
	if (qgetenv("XDG_SESSION_TYPE") == QByteArray("wayland"))
		qputenv("QT_WAYLAND_SHELL_INTEGRATION", QByteArray("layer-shell"));

	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);

	auto dbus_service = get_service();
	connect_to_session_bus(dbus_service);

	qmlRegisterSingletonType<BzardThemes>("bzard", 1, 0, "BzardThemes",
	                                      bzardthemes_provider);
	qmlRegisterType<BzardExpirationController>("bzard", 1, 0,
	                                           "BzardExpirationController");
	qmlRegisterType<BzardTrayIcon>("bzard", 1, 0, "BzardTrayIcon");
	qmlRegisterSingletonType<BzardNotifications>(
		  "bzard", 1, 0, "BzardNotifications", bzardnotifications_provider);
	qmlRegisterSingletonType<BzardHistory>("bzard", 1, 0, "BzardHistory",
	                                       bzardhistory_provider);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
