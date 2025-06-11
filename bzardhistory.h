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

#include <deque>
#include <memory>

#include <QAbstractListModel>
#include <QObject>

#include "bzardconfig.h"
#include "bzardnotificationreceiver.h"
class BzardHistoryModel;

class BzardHistoryNotification : public QObject
{
	Q_OBJECT
	Q_PROPERTY(uint id_ READ id_ CONSTANT)
	Q_PROPERTY(QString application READ application CONSTANT)
	Q_PROPERTY(QString title READ title CONSTANT)
	Q_PROPERTY(QString body READ body CONSTANT)
	Q_PROPERTY(QString iconUrl READ iconUrl CONSTANT)
      public:
	BzardHistoryNotification() = default;
	BzardHistoryNotification(const IQNotification &n,
			      QObject *parent = nullptr);

	uint id_() const;
	QString application() const;
	QString title() const;
	QString body() const;
	QString iconUrl() const;

      private:
	const uint id__{0};
	const QString application_;
	const QString title_;
	const QString body_;
	const QString iconUrl_;
};

class BzardHistory : public IQNotificationReceiver, public IQConfigurable
{
	friend class BzardHistoryModel;

	Q_OBJECT
	Q_PROPERTY(bool isEnabled READ isEnabled CONSTANT)
	Q_PROPERTY(QAbstractItemModel *model READ model CONSTANT)
      public:
	BzardHistory();
	QAbstractListModel *model() const;

      public slots:
	/*
	 * External slots
	 */
	void onCreateNotification(const IQNotification &notification) final;
	void onDropNotification(IQNotification::id_t id) final;

	/*
	 * QML slots
	 */
	void remove(uint index);

      signals:
	void rowInserted();

      private:
    using ptr_t = BzardHistory *;
	std::deque<std::unique_ptr<BzardHistoryNotification>> historyList;
	std::unique_ptr<BzardHistoryModel> model_;

	void removeHistoryNotification(uint index);
};

class BzardHistoryModel : public QAbstractListModel
{
	Q_OBJECT
      public:
	enum HistoryRoles {
		Id_Role = Qt::UserRole + 1,
		ApplicationRole,
		TitleRole,
		BodyRole,
		IconUrlRole
	};
	explicit BzardHistoryModel(BzardHistory::ptr_t history_);
	int rowCount(const QModelIndex &parent) const final;
	QVariant data(const QModelIndex &index, int role) const final;
	bool insertRows(int row, int count, const QModelIndex &parent) final;
	bool removeRows(int row, int count, const QModelIndex &parent) final;
	QHash<int, QByteArray> roleNames() const final;

      private slots:
	void onHistoryRowInserted();

      private:
	BzardHistory *iqHistory;
};
