#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <optional>
#include <expected> // Для std::expected
#include <string>   // Для std::string в ошибке

class UdevMonitor;

/**
 * @brief Reads and reports brightness percentage for a specific backlight device.
 *
 * Relies on sysfs files and an external trigger (like UdevMonitor) to check for updates.
 * Configuration (device paths) is immutable after construction. Reading errors
 * are returned by reading functions but logged internally by this class.
 * Percentage changes are emitted via the brightnessChanged signal.
 *
 * Usage:
 * 1. Call BzardBrightnessMonitor::findDevicePaths(deviceName) to get validated paths.
 * 2. If paths are valid, construct the BzardBrightnessMonitor instance with them.
 * 3. Call checkForUpdate() when an external event suggests a possible change.
 * 4. Connect to the brightnessChanged(int) signal.
 */
class BzardBrightnessMonitor : public QObject
{
    Q_OBJECT

public:
    struct DevicePaths {
        QString basePath;
        QString brightnessFilePath;
        QString maxBrightnessFilePath;
        QString canonicalSyspath;
    };

    // Define an error type for std::expected
    // Using std::string for simplicity, could be an enum class
    using ReadError = std::string;
    using ReadResult = std::expected<qlonglong, ReadError>;


    static std::optional<DevicePaths> findDevicePaths(const QString& deviceName);
    static QStringList availableDevices();


    explicit BzardBrightnessMonitor(const DevicePaths& paths, QObject *parent = nullptr);


    QString syspath() const;
    void checkForUpdate();


signals:
    void brightnessChanged(int brightnessPercent);


private:
    /**
     * @brief Reads a qlonglong value from the specified sysfs file path.
     * @param filePath The full path to the file.
     * @return ReadResult containing the value on success or a ReadError string on failure.
     */
    static ReadResult readLongFromFile(const QString& filePath);


    const QString basePath;
    const QString brightnessFilePath;
    const QString maxBrightnessFilePath;
    const QString canonicalSyspath;

    int lastBrightnessPercent = -1;
};
