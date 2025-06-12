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

#include "iqconfig.h"

#include <qt5xdg/XdgDirs>

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace
{
// TODO: refactor; taken from here: https://gist.github.com/ssendeavour/7324701
static bool copyRecursively(const QString &srcFilePath,
			    const QString &tgtFilePath)
{
	QFileInfo srcFileInfo(srcFilePath);
	if (srcFileInfo.isDir()) {
		QDir targetDir(tgtFilePath);
		targetDir.cdUp();
		if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
			return false;
		QDir sourceDir(srcFilePath);
		QStringList fileNames = sourceDir.entryList(
		    QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot |
		    QDir::Hidden | QDir::System);
		for (const QString &fileName : fileNames) {
			const QString newSrcFilePath =
			    srcFilePath + QLatin1Char('/') + fileName;
			const QString newTgtFilePath =
			    tgtFilePath + QLatin1Char('/') + fileName;
			if (!copyRecursively(newSrcFilePath, newTgtFilePath))
				return false;
		}
	} else {
		if (!QFile::copy(srcFilePath, tgtFilePath)) // NOLINT
			return false;
	}
	return true;
}
} // anonymouse namespace

IQConfig::IQConfig(const QString &category_, const QString &fileName_)
    : category{category_.isEmpty() ? "" : category_ + '/'}, fileName{fileName_},
      settings{std::make_unique<QSettings>(getConfigFileName(),
					   QSettings::IniFormat)}
{
}

QVariant IQConfig::value(const QString &key, const QVariant &defaultValue) const
{
	return settings->value(category + key, defaultValue);
}

void IQConfig::setValue(const QString &key, const QVariant &value)
{
	settings->setValue(category + key, value);
}

#define IQ_MACRO_STRING(S) IQ_MACRO_STRING__(S)
#define IQ_MACRO_STRING__(S) #S
QString IQConfig::applicationName()
{
	return QStringLiteral(IQ_MACRO_STRING(IQ_APP_NAME));
}

QString IQConfig::configDir()
{
	static auto configDir = XdgDirs::configHome() + '/' + applicationName();
	return configDir;
}

QString IQConfig::applicationVersion()
{
	return QStringLiteral(IQ_MACRO_STRING(IQ_VERSION));
}
#undef IQ_MACRO_STRING__
#undef IQ_MACRO_STRING

QString IQConfig::getConfigFileName() const
{
	auto config = configDir() + '/' + fileName;
	QFileInfo config_file{config};

	if (config_file.exists()) {
		if (!config_file.isFile())
			throw std::runtime_error{config.toStdString() +
						 " is not a valid config file"};
	} else {
		QDir dir;
		dir.mkpath(configDir());
		if (config.contains("/themes/default/theme"))
			copyThemesFromShare(
			    QString{config}.replace("/default/theme", ""));
		else
			copyConfigFileFromExample(config);
	}
	return config;
}

bool IQConfig::copyConfigFileFromExample(const QString &destination) const
{
	auto config_example_path =
	    "/usr/share/" + applicationName() + '/' + fileName + ".example";
	QFile config_example_file{config_example_path};
	if (!config_example_file.exists())
		return false;
	return config_example_file.copy(destination);
}

bool IQConfig::copyThemesFromShare(const QString &destination) const
{
	auto shareThemesPath = "/usr/share/" + applicationName() + "/themes";
	return copyRecursively(shareThemesPath, destination);
}

IQConfigurable::IQConfigurable(const QString &name) : name_{name}, config{name_}
{
}

const QString &IQConfigurable::name() const { return name_; }

bool IQConfigurable::isEnabled() const
{
	return config.value("enabled", false).toBool();
}
