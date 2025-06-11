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

#define IQ_CONF_VAR(VAR__, NAME__, DEFAULT__)                                  \
	static constexpr auto CONFIG_##VAR__ = NAME__;                         \
	static constexpr auto CONFIG_##VAR__##_DEFAULT = DEFAULT__;

#define IQ_CONF_FACTOR(VAR__, NAME__, DEFAULT__)                               \
	static constexpr auto CONFIG_##VAR__ = NAME__;                         \
	static constexpr auto VAR__##_DEFAULT_FACTOR = DEFAULT__;

class IQConfig
{
      public:
	static QString applicationVersion();
	static QString applicationName();
	static QString configDir();

	explicit IQConfig(const QString &category_,
			  const QString &fileName_ = "config");

	QVariant value(const QString &key,
		       const QVariant &defaultValue = QVariant()) const;
	void setValue(const QString &key, const QVariant &value);

      private:
	const QString category;
	const QString fileName;
	std::unique_ptr<QSettings> settings;

	QString getConfigFileName() const;
	bool copyConfigFileFromExample(const QString &destination) const;
	bool copyThemesFromShare(const QString &destination) const;
};

struct IQConfigurable {
	IQConfigurable() = delete;
	const QString &name() const;
	bool isEnabled() const;

      protected:
	explicit IQConfigurable(const QString &name);
	const QString name_;
	IQConfig config;
};
