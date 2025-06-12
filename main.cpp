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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtDBus/QDBusConnection>
#include <QtQml>

#include "notificationsadaptor.h"

#include "iqdbusservice.h"
#include "iqexpirationcontroller.h"
#include "iqhistory.h"
#include "iqnotificationmodifiers.h"
#include "iqnotifications.h"
#include "iqthemes.h"
#include "iqtopdown.h"
#include "iqtrayicon.h"

#ifdef IQ_X11
#include "X11-plugin/x11fullscreendetector.h"
#endif

/*
 * Should be called first
 */
static gsl::not_null<IQDBusService *> get_service();
static gsl::not_null<IQHistory *> get_history();
static QDBusConnection
connect_to_session_bus(gsl::not_null<IQDBusService *> service);
static QObject *iqnotifications_provider(QQmlEngine *engine,
					 QJSEngine *scriptEngine);
static QObject *iqthemes_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

gsl::not_null<IQDBusService *> get_service()
{
	using namespace IQNotificationModifiers; // NOLINT

	auto disposition = std::make_unique<IQTopDown>();
	auto dbus_service =
	    (new IQDBusService)
		->addModifier(make<IDGenerator>())
		->addModifier(make<TitleToIcon>())
		->addModifier(make<IconHandler>())
		->addModifier(make<BodyToTitleWhenTitleIsAppName>())
		->addModifier(make<DefaultTimeout>())
		->addModifier(make<ReplaceMinusToDash>());

	auto notifications = IQNotifications::get(std::move(disposition));
	if (notifications->isEnabled())
		dbus_service->connectReceiver(notifications);
	if (get_history()->isEnabled())
		dbus_service->connectReceiver(get_history().get());

	std::unique_ptr<IQFullscreenDetector> fullscreenDetector;
#ifdef IQ_X11
	fullscreenDetector = std::make_unique<X11FullscreenDetector>();
#endif // IQ_X11
	notifications->setFullscreenDetector(std::move(fullscreenDetector));
	return dbus_service;
}

gsl::not_null<IQHistory *> get_history()
{
	static IQHistory history;
	return {&history};
}

QObject *iqnotifications_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine);
	Q_UNUSED(scriptEngine);
	return IQNotifications::get();
}

QObject *iqthemes_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine);
	Q_UNUSED(scriptEngine);
	static IQThemes theme;
	return &theme;
}

QObject *iqhistory_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine);
	Q_UNUSED(scriptEngine);
	return get_history().get();
}

QDBusConnection connect_to_session_bus(gsl::not_null<IQDBusService *> service)
{
	auto connection = QDBusConnection::sessionBus();
	if (!connection.registerService("org.freedesktop.Notifications")) {
		throw std::runtime_error{"DBus Service already registered!"};
	}
	new NotificationsAdaptor(service);
	if (!connection.registerObject("/org/freedesktop/Notifications",
				       service)) {
		throw std::runtime_error{"Can't register DBus service object!"};
	}
	return connection;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);

	auto dbus_service = get_service();
	connect_to_session_bus(dbus_service);

	qmlRegisterSingletonType<IQThemes>("IQNotifier", 1, 0, "IQThemes",
					   iqthemes_provider);
	qmlRegisterType<IQExpirationController>("IQNotifier", 1, 0,
						"IQExpirationController");
	qmlRegisterType<IQTrayIcon>("IQNotifier", 1, 0, "IQTrayIcon");
	qmlRegisterSingletonType<IQNotifications>(
	    "IQNotifier", 1, 0, "IQNotifications", iqnotifications_provider);
	qmlRegisterSingletonType<IQHistory>("IQNotifier", 1, 0, "IQHistory",
					    iqhistory_provider);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
