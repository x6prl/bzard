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

#include "bzard_history.h"

#include <QtQml/QtQml>

BzardHistory::BzardHistory()
	  : BzardConfigurable{"history"},
		model_{std::make_unique<BzardHistoryModel>(this)} {
	qmlRegisterType<BzardHistoryNotification>("bzard", 1, 0,
	                                          "HistoryNotification");
}

void BzardHistory::onCreateNotification(const BzardNotification &NOTIFICATION) {
	historyList.push_front(
		  std::make_unique<BzardHistoryNotification>(NOTIFICATION));
	emit rowInserted();
}

void BzardHistory::onDropNotification(BzardNotification::IdT id) {
	Q_UNUSED(id)
}

void BzardHistory::remove(uint index) { model_->removeRow(index); }

QAbstractListModel *BzardHistory::model() const { return model_.get(); }

void BzardHistory::removeHistoryNotification(uint index) {
	auto it = historyList.begin();
	std::advance(it, index);
	historyList.erase(it);
}

BzardHistoryNotification::BzardHistoryNotification(
	  const BzardNotification &NOTIFICATION, QObject *parent)
	  : QObject(parent), ID__{NOTIFICATION.id},
		APPLICATION{NOTIFICATION.application}, TITLE_{NOTIFICATION.title},
		BODY_{NOTIFICATION.body}, ICON_URL_{NOTIFICATION.iconUrl} {}

uint BzardHistoryNotification::id_() const { return ID__; }

QString BzardHistoryNotification::application() const { return APPLICATION; }

QString BzardHistoryNotification::title() const { return TITLE_; }

QString BzardHistoryNotification::body() const { return BODY_; }

QString BzardHistoryNotification::iconUrl() const { return ICON_URL_; }

BzardHistoryModel::BzardHistoryModel(BzardHistory::PtrT history)
	  : bzardHistory{history} {
	connect(bzardHistory, &BzardHistory::rowInserted, this,
	        &BzardHistoryModel::onHistoryRowInserted);
}

int BzardHistoryModel::rowCount(const QModelIndex &parent) const {
	if (!bzardHistory)
		return 0;

	return bzardHistory->historyList.size();
	Q_UNUSED(parent);
}

QVariant BzardHistoryModel::data(const QModelIndex &index, int role) const {
	if (!bzardHistory)
		return {};

	if (!index.isValid())
		return QVariant();

	auto &index_ = bzardHistory->historyList[index.row()];
	switch (role) {
	case HR_ID_ROLE:
		return index_->id_();
		break;
	case HR_APPLICATION_ROLE:
		return index_->application();
		break;
	case HR_TITLE_ROLE:
		return index_->title();
		break;
	case HR_BODY_ROLE:
		return index_->body();
		break;
	case HR_ICON_URL_ROLE:
		return index_->iconUrl();
		break;
	default:
		break;
	}
	return {};
}

bool BzardHistoryModel::insertRows(int row, int count,
                                   const QModelIndex &parent) {
	if (row || count > 1)
		return false;

	beginInsertRows(parent, 0, 0);
	endInsertRows();
	return true;
}

bool BzardHistoryModel::removeRows(int row, int count,
                                   const QModelIndex &parent) {
	if (!bzardHistory)
		return false;

	auto &list = bzardHistory->historyList;
	if (static_cast<size_t>(row) >= list.size() || row + count <= 0)
		return false;

	auto beginRow = qMax(0, row);
	auto endRow = qMin(row + count - 1, static_cast<int>(list.size() - 1));

	beginRemoveRows(parent, beginRow, endRow);

	while (beginRow <= endRow) {
		bzardHistory->removeHistoryNotification(beginRow);
		++beginRow;
	}

	endRemoveRows();
	return true;
}

QHash<int, QByteArray> BzardHistoryModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[HR_ID_ROLE] = "id_";
	roles[HR_APPLICATION_ROLE] = "application";
	roles[HR_TITLE_ROLE] = "title";
	roles[HR_BODY_ROLE] = "body";
	roles[HR_ICON_URL_ROLE] = "iconUrl";
	return roles;
}

void BzardHistoryModel::onHistoryRowInserted() { insertRow(0); }
