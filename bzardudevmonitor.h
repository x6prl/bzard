#pragma once

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <libudev.h>

struct udev_device;

/**
 * @brief Monitors udev events using a socket notifier.
 *
 * Usage:
 * 1. Create an instance.
 * 2. Call initialize().
 * 3. Call addSubsystemFilter() (or other filter methods) BEFORE initialize()
 *    if specific filtering is needed. Filters added after might not apply.
 * 4. Connect to the udevEvent(action, syspath) signal.
 * 5. Connect to the errorOccurred(errorString) signal for initialization errors.
 */
class UdevMonitor : public QObject
{
    Q_OBJECT

public:
    explicit UdevMonitor(QObject *parent = nullptr);
    ~UdevMonitor() override;

    bool initialize();
    bool addSubsystemFilter(const QString& subsystem);

signals:
    void udevEvent(const QString& action, const QString& syspath);
    void errorOccurred(const QString& errorString);

private slots:
    void onUdevFdReadable();

private:
    udev *udevContext = nullptr;
    udev_monitor *udevMonitor = nullptr;
    QSocketNotifier *udevNotifier = nullptr;
};