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

#pragma once

#include <memory>

#include <QObject>
#include <QSettings>

#define BZARD_CONFIG_VAR(VAR__, NAME__, DEFAULT__)                             \
	static constexpr auto CONFIG_##VAR__ = NAME__;                             \
	static constexpr auto CONFIG_##VAR__##_DEFAULT = DEFAULT__;

#define BZARD_CONFIG_FACTOR(VAR__, NAME__, DEFAULT__)                          \
	static constexpr auto CONFIG_##VAR__ = NAME__;                             \
	static constexpr auto VAR__##_DEFAULT_FACTOR = DEFAULT__;

class BzardConfig {
  public:
	static QString applicationVersion();
	static QString applicationName();
	static QString configDir();

	explicit BzardConfig(const QString &CATEGORY_,
	                     const QString &FILE_NAME_ = "config");

	QVariant value(const QString &KEY,
	               const QVariant &DEAFAULT_VALUE = QVariant()) const;
	void setValue(const QString &KEY, const QVariant &VALUE);

  private:
	const QString CATEGORY;
	const QString FILE_NAME;
	std::unique_ptr<QSettings> settings;

	QString getConfigFileName() const;
	bool copyConfigFileFromExample(const QString &destination) const;
	bool copyThemesFromShare(const QString &destination) const;
};

struct BzardConfigurable {
	BzardConfigurable() = delete;
	const QString &name() const;
	bool isEnabled() const;

  protected:
	explicit BzardConfigurable(const QString &name);
	const QString name_;
	BzardConfig config;
};
