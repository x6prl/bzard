#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QStringList>
#include <QMap>
// #include <memory> // Больше не нужен для QMap

#include "bzardudevmonitor.h"
#include "bzardbrightnessmonitor.h"

class QMLBrightnessMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList availableDevices READ availableDevices CONSTANT)
    Q_PROPERTY(QVariantMap brightnessLevels READ brightnessLevels NOTIFY brightnessLevelsChanged)

public:
    explicit QMLBrightnessMonitor(QObject *parent = nullptr);
    // Деструктор по умолчанию ОК, Qt удалит дочерние объекты
    ~QMLBrightnessMonitor() override = default;

    bool initialize();
    QStringList availableDevices() const;
    QVariantMap brightnessLevels() const;

signals:
    void brightnessLevelsChanged();
    void errorOccurred(const QString& errorSource, const QString& errorMessage);

private slots:
    void handleUdevEvent(const QString& action, const QString& eventSyspath);
    void onDeviceBrightnessChanged(int brightnessPercent);
    void onUdevError(const QString& errorString);

private:
    // --- Тип карты изменен на raw pointer ---
    // Мы делаем BzardBrightnessMonitor дочерними объектами этого класса,
    // поэтому Qt автоматически удалит их при удалении QMLBrightnessMonitor.
    using MonitorMap = QMap<QString, BzardBrightnessMonitor*>;

    void setupBrightnessMonitors();

    QStringList internalAvailableDevices;
    QVariantMap internalBrightnessLevels;
    MonitorMap  activeMonitors; // Теперь хранит raw pointers
    QMap<QString, QString> syspathToDeviceName;

    // unique_ptr все еще подходит для UdevMonitor, так как он один
    std::unique_ptr<UdevMonitor> udevMonitorInstance;
    bool isInitialized = false;
};

QObject* qml_brightness_monitor_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
