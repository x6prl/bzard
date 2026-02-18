#include "bzard_qml_brightness_monitor.h"
#include <QDebug>

QMLBrightnessMonitor::QMLBrightnessMonitor(QObject *parent) : QObject(parent) {
	qInfo() << Q_FUNC_INFO << "Singleton instance created.";
}

bool QMLBrightnessMonitor::initialize() {
	if (isInitialized) {
		return true;
	}
	qInfo() << Q_FUNC_INFO << "Initializing...";

	udevMonitorInstance = std::make_unique<UdevMonitor>();
	if (!udevMonitorInstance->initialize()) {
		emit errorOccurred("UdevMonitor", "Initialization failed.");
		udevMonitorInstance.reset();
		return false;
	}
	if (!udevMonitorInstance->addSubsystemFilter(QStringLiteral("backlight"))) {
		emit errorOccurred("UdevMonitor",
		                   "Failed to add backlight subsystem filter.");
		udevMonitorInstance.reset();
		return false;
	}
	connect(udevMonitorInstance.get(), &UdevMonitor::udevEvent, this,
	        &QMLBrightnessMonitor::handleUdevEvent);
	connect(udevMonitorInstance.get(), &UdevMonitor::errorOccurred, this,
	        &QMLBrightnessMonitor::onUdevError);

	setupBrightnessMonitors();

	isInitialized = true;
	qInfo() << Q_FUNC_INFO << "Initialization complete.";
	return true;
}

void QMLBrightnessMonitor::setupBrightnessMonitors() {
	qInfo() << Q_FUNC_INFO << "Setting up brightness monitors...";
	qDeleteAll(activeMonitors);
	activeMonitors.clear();
	sysPathToDeviceName.clear();
	internalBrightnessLevels.clear();

	internalAvailableDevices = BzardBrightnessMonitor::availableDevices();
	qInfo() << "Found devices:" << internalAvailableDevices;

	bool ok = false;
	for (const QString &deviceName : internalAvailableDevices) {
		std::optional<BzardBrightnessMonitor::DevicePaths> pathsOpt =
			  BzardBrightnessMonitor::findDevicePaths(deviceName);
		if (!pathsOpt) {
			emit errorOccurred(deviceName, "Could not find/validate paths.");
			continue;
		}

		try {
			BzardBrightnessMonitor::DevicePaths paths = pathsOpt.value();
			BzardBrightnessMonitor *monitor =
				  new BzardBrightnessMonitor(paths, this);
			connect(monitor, &BzardBrightnessMonitor::brightnessChanged, this,
			        &QMLBrightnessMonitor::onDeviceBrightnessChanged);

			QString sysPath = monitor->sysPath();
			sysPathToDeviceName[sysPath] = deviceName;
			activeMonitors.insert(sysPath, monitor);

			ok = true;

		} catch (const std::exception &e) {
			emit errorOccurred(
				  deviceName,
				  QString("Exception during creation: %1").arg(e.what()));
		}
	}

	if (!ok) {
		qWarning() << Q_FUNC_INFO
				   << "Failed to set up any brightness monitors.";
	}
	emit brightnessLevelsChanged();
}

QStringList QMLBrightnessMonitor::availableDevices() const {
	return internalAvailableDevices;
}
QVariantMap QMLBrightnessMonitor::brightnessLevels() const {
	return internalBrightnessLevels;
}

void QMLBrightnessMonitor::handleUdevEvent(const QString &action,
                                           const QString &eventSysPath) {
	if (action != QLatin1String("change") || eventSysPath.isEmpty()) {
		return;
	}
	for (auto iteration = activeMonitors.constBegin();
	     iteration != activeMonitors.constEnd(); ++iteration) {
		const QString &monitorSysPath = iteration.key();
		BzardBrightnessMonitor *monitorPtr = iteration.value();
		if (eventSysPath.startsWith(monitorSysPath) && monitorPtr) {
			monitorPtr->checkForUpdate();
			break;
		}
	}
}

void QMLBrightnessMonitor::onDeviceBrightnessChanged(int brightnessPercent) {
	BzardBrightnessMonitor *senderMonitor =
		  qobject_cast<BzardBrightnessMonitor *>(sender());
	if (!senderMonitor) {
		return;
	}
	QString senderSysPath = senderMonitor->sysPath();
	QString deviceName = sysPathToDeviceName.value(
		  senderSysPath, QStringLiteral("UnknownDevice"));
	bool changed = false;
	if (!internalBrightnessLevels.contains(deviceName) ||
	    internalBrightnessLevels.value(deviceName).toInt() !=
	          brightnessPercent) {
		internalBrightnessLevels[deviceName] = brightnessPercent;
		changed = true;
	}
	if (changed) {
		emit brightnessLevelsChanged();
	}
}

void QMLBrightnessMonitor::onUdevError(const QString &errorString) {
	qWarning() << "[UdevMonitor Error]" << errorString;
	emit errorOccurred(QStringLiteral("UdevMonitor"), errorString);
}

static QPointer<QMLBrightnessMonitor> gBrightnessMonitorInstance = nullptr;
QObject *qmlBrightnessMonitorProvider(QQmlEngine *engine,
                                      QJSEngine *scriptEngine) {
	Q_UNUSED(engine);
	Q_UNUSED(scriptEngine);
	if (!gBrightnessMonitorInstance) {
		gBrightnessMonitorInstance = new QMLBrightnessMonitor();
		if (!gBrightnessMonitorInstance->initialize()) {
			qCritical("QMLBrightnessMonitor failed to initialize!");
			return nullptr;
		}
	}
	return gBrightnessMonitorInstance;
}
