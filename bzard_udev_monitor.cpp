#include "bzard_udev_monitor.h"
#include <QDebug>

UdevMonitor::UdevMonitor(QObject *parent) : QObject(parent) {}

UdevMonitor::~UdevMonitor() {
	if (udevNotifier) {
		udevNotifier->setEnabled(false);
	}
	if (udevMonitor) {
		udev_monitor_unref(udevMonitor);
	}
	if (udevContext) {
		udev_unref(udevContext);
	}
}

bool UdevMonitor::initialize() {
	if (udevContext) {
		qWarning() << Q_FUNC_INFO << "Already initialized.";
		return true;
	}

	udevContext = udev_new();
	if (!udevContext) {
		const QString errorMessage =
			  QStringLiteral("Failed to create udev context");
		qCritical() << Q_FUNC_INFO << errorMessage;
		emit errorOccurred(errorMessage);
		return false;
	}

	udevMonitor = udev_monitor_new_from_netlink(udevContext, "udev");
	if (!udevMonitor) {
		const QString errorMessage =
			  QStringLiteral("Failed to create udev monitor");
		qCritical() << Q_FUNC_INFO << errorMessage;
		emit errorOccurred(errorMessage);
		udev_unref(udevContext);
		udevContext = nullptr;
		return false;
	}

	if (udev_monitor_enable_receiving(udevMonitor) < 0) {
		const QString errorMessage =
			  QStringLiteral("Failed to enable udev receiving");
		qCritical() << Q_FUNC_INFO << errorMessage;
		emit errorOccurred(errorMessage);
		udev_monitor_unref(udevMonitor);
		udev_unref(udevContext);
		udevMonitor = nullptr;
		udevContext = nullptr;
		return false;
	}

	const int udevFeed = udev_monitor_get_fd(udevMonitor);
	if (udevFeed < 0) {
		const QString errorMessage =
			  QStringLiteral("Failed to get udev monitor FD");
		qCritical() << Q_FUNC_INFO << errorMessage;
		emit errorOccurred(errorMessage);
		udev_monitor_unref(udevMonitor);
		udev_unref(udevContext);
		udevMonitor = nullptr;
		udevContext = nullptr;
		return false;
	}

	udevNotifier = new QSocketNotifier(udevFeed, QSocketNotifier::Read, this);
	connect(udevNotifier, &QSocketNotifier::activated, this,
	        &UdevMonitor::onUdevFdReadable);
	udevNotifier->setEnabled(true);
	qInfo() << Q_FUNC_INFO << "Udev monitoring initialized.";
	return true;
}

bool UdevMonitor::addSubsystemFilter(const QString &subsystem) {
	if (!udevMonitor) {
		const QString errorMessage =
			  QStringLiteral("Cannot add filter: udev monitor not initialized");
		qWarning() << Q_FUNC_INFO << errorMessage;
		emit errorOccurred(errorMessage);
		return false;
	}

	if (udev_monitor_filter_add_match_subsystem_devtype(
			  udevMonitor, subsystem.toUtf8().constData(), nullptr) < 0) {
		const QString errorMessage =
			  QString("Failed to add udev filter for subsystem %1.")
					.arg(subsystem);
		qCritical() << Q_FUNC_INFO << errorMessage;
		emit errorOccurred(errorMessage);
		return false;
	}
	qInfo() << Q_FUNC_INFO << "Added filter for subsystem:" << subsystem;
	return true;
}

void UdevMonitor::onUdevFdReadable() {
	if (!udevMonitor)
		return;
	udev_device *dev = udev_monitor_receive_device(udevMonitor);
	if (!dev) {
		return;
	}
	const char *actionCStr = udev_device_get_action(dev);
	const char *sysPathCStr = udev_device_get_syspath(dev);
	emit udevEvent(actionCStr ? QString::fromUtf8(actionCStr) : QString(),
	               sysPathCStr ? QString::fromUtf8(sysPathCStr) : QString());
	udev_device_unref(dev);
}
