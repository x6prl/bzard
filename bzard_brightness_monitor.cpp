#include "bzard_brightness_monitor.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <cmath>
#include <expected>
#include <optional>
#include <string>

QStringList BzardBrightnessMonitor::availableDevices() {
	const QDir BACKLIGHT_DIR(QStringLiteral("/sys/class/backlight"));
	if (!BACKLIGHT_DIR.exists()) {
		qWarning() << Q_FUNC_INFO
				   << "Directory /sys/class/backlight does not exist.";
		return QStringList();
	}
	return BACKLIGHT_DIR.entryList(QDir::Dirs | QDir::NoDotAndDotDot,
	                               QDir::Name);
}

std::optional<BzardBrightnessMonitor::DevicePaths>
BzardBrightnessMonitor::findDevicePaths(const QString &deviceName) {
	if (deviceName.isEmpty()) {
		qWarning() << Q_FUNC_INFO << "Device name cannot be empty.";
		return std::nullopt;
	}
	const QDir BACKLIGHT_DIR(QStringLiteral("/sys/class/backlight"));
	DevicePaths paths;
	paths.basePath = BACKLIGHT_DIR.filePath(deviceName);
	const QFileInfo BASE_INFO(paths.basePath);
	if (!BASE_INFO.exists() || !BASE_INFO.isDir()) {
		qWarning() << Q_FUNC_INFO
				   << "Device directory not found or is not a directory:"
				   << paths.basePath;
		return std::nullopt;
	}
	paths.brightnessFilePath = BASE_INFO.filePath() + QDir::separator() +
	                           QStringLiteral("brightness");
	paths.maxBrightnessFilePath = BASE_INFO.filePath() + QDir::separator() +
	                              QStringLiteral("max_brightness");
	paths.canonicalSysPath = BASE_INFO.canonicalFilePath();
	bool files_ok = QFile::exists(paths.brightnessFilePath) &&
	                QFile::exists(paths.maxBrightnessFilePath) &&
	                !paths.canonicalSysPath.isEmpty();
	if (!files_ok) {
		qWarning() << Q_FUNC_INFO
				   << "Required files or syspath not found/valid for device"
				   << deviceName << "at" << paths.basePath;
		return std::nullopt;
	}
	return paths;
}

BzardBrightnessMonitor::BzardBrightnessMonitor(const DevicePaths &paths,
                                               QObject *parent)
	  : QObject(parent), basePath(paths.basePath),
		brightnessFilePath(paths.brightnessFilePath),
		maxBrightnessFilePath(paths.maxBrightnessFilePath),
		cannonicalSysPath(paths.canonicalSysPath) {
	if (basePath.isEmpty() || brightnessFilePath.isEmpty() ||
	    maxBrightnessFilePath.isEmpty() || cannonicalSysPath.isEmpty()) {
		qCritical() << Q_FUNC_INFO << "FATAL: Constructed with invalid paths!";
	}
	checkForUpdate();
}

QString BzardBrightnessMonitor::sysPath() const { return cannonicalSysPath; }

int64_t BzardBrightnessMonitor::readLongFromFile(const QString &filePath) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw QString("Could not open file %1: %2")
			  .arg(filePath, file.errorString())
			  .toStdString();
	}
	bool ok = false;
	const QString content = file.readAll().trimmed();
	qlonglong value = content.toLongLong(&ok);
	if (!ok) {
		throw QString("Failed to parse value '%1' from file: %2")
			  .arg(content, filePath)
			  .toStdString();
	}
	return value;
}

void BzardBrightnessMonitor::checkForUpdate() {
	std::expected<qlonglong, std::string> currentRes =
		  readLongFromFile(brightnessFilePath);
	std::expected<qlonglong, std::string> maxRes =
		  readLongFromFile(maxBrightnessFilePath);

	if (!currentRes) {
		qWarning() << Q_FUNC_INFO << QString::fromStdString(currentRes.error());
		return;
	}
	if (!maxRes) {
		qWarning() << Q_FUNC_INFO << QString::fromStdString(maxRes.error());
		return;
	}

	const auto current = currentRes.value();
	const auto max = maxRes.value();

	if (max <= 0) {
		qWarning() << Q_FUNC_INFO << "Invalid max_brightness value (" << max
				   << ") for" << basePath;
		return;
	}

	const int currentPercent = std::max(
		  0, std::min(100, static_cast<int>(std::round(
								 static_cast<double>(current) * 100.0 / max))));

	if (currentPercent != lastBrightnessPercent) {
		lastBrightnessPercent = currentPercent;
		emit brightnessChanged(lastBrightnessPercent);
	}
}
