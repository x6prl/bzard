#pragma once

#include "bzard_brightness_monitor.h"
#include "bzard_udev_monitor.h"
#include <QJSEngine>
#include <QMap>
#include <QObject>
#include <QQmlEngine>
#include <QStringList>

class QMLBrightnessMonitor : public QObject {
	Q_OBJECT
	Q_PROPERTY(QStringList availableDevices READ availableDevices CONSTANT)
	Q_PROPERTY(QVariantMap brightnessLevels READ brightnessLevels NOTIFY
	                 brightnessLevelsChanged)

  public:
	explicit QMLBrightnessMonitor(QObject *parent = nullptr);
	~QMLBrightnessMonitor() override = default;

	bool initialize();
	QStringList availableDevices() const;
	QVariantMap brightnessLevels() const;

  signals:
	void brightnessLevelsChanged();
	void errorOccurred(const QString &errorSource, const QString &errorMessage);

  private slots:
	void handleUdevEvent(const QString &action, const QString &eventSysPath);
	void onDeviceBrightnessChanged(int brightnessPercent);
	void onUdevError(const QString &errorString);

  private:
	using MonitorMap = QMap<QString, BzardBrightnessMonitor *>;

	void setupBrightnessMonitors();

	QStringList internalAvailableDevices;
	QVariantMap internalBrightnessLevels;
	MonitorMap activeMonitors;
	QMap<QString, QString> sysPathToDeviceName;
	std::unique_ptr<UdevMonitor> udevMonitorInstance;
	bool isInitialized = false;
};

QObject *qmlBrightnessMonitorProvider(QQmlEngine *engine,
                                      QJSEngine *scriptEngine);
