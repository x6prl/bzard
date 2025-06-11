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

#include "bzardhistory.h"

#include <QtQml/QtQml>

BzardHistory::BzardHistory()
    : IQConfigurable{"history"}, model_{std::make_unique<BzardHistoryModel>(this)}
{
	qmlRegisterType<BzardHistoryNotification>("bzard", 1, 0,
					       "HistoryNotification");
}

void BzardHistory::onCreateNotification(const IQNotification &notification)
{
	historyList.push_front(
	    std::make_unique<BzardHistoryNotification>(notification));
	emit rowInserted();
}

void BzardHistory::onDropNotification(IQNotification::id_t id) { Q_UNUSED(id) }

void BzardHistory::remove(uint index) { model_->removeRow(index); }

QAbstractListModel *BzardHistory::model() const { return model_.get(); }

void BzardHistory::removeHistoryNotification(uint index)
{
	auto it = historyList.begin();
	std::advance(it, index);
	historyList.erase(it);
}

BzardHistoryNotification::BzardHistoryNotification(const IQNotification &n,
					     QObject *parent)
    : QObject(parent), id__{n.id}, application_{n.application}, title_{n.title},
      body_{n.body}, iconUrl_{n.icon_url}
{
}

uint BzardHistoryNotification::id_() const { return id__; }

QString BzardHistoryNotification::application() const { return application_; }

QString BzardHistoryNotification::title() const { return title_; }

QString BzardHistoryNotification::body() const { return body_; }

QString BzardHistoryNotification::iconUrl() const { return iconUrl_; }

BzardHistoryModel::BzardHistoryModel(BzardHistory::ptr_t history_) : iqHistory{history_}
{
	connect(iqHistory, &BzardHistory::rowInserted, this,
		&BzardHistoryModel::onHistoryRowInserted);
}

int BzardHistoryModel::rowCount(const QModelIndex &parent) const
{
	if (!iqHistory)
		return 0;

	Q_UNUSED(parent);
	return iqHistory->historyList.size();
}

QVariant BzardHistoryModel::data(const QModelIndex &index, int role) const
{
	if (!iqHistory)
		return {};

	if (!index.isValid())
		return QVariant();

	auto &n = iqHistory->historyList[index.row()];
	switch (role) {
	case Id_Role:
		return n->id_();
		break;
	case ApplicationRole:
		return n->application();
		break;
	case TitleRole:
		return n->title();
		break;
	case BodyRole:
		return n->body();
		break;
	case IconUrlRole:
		return n->iconUrl();
		break;
	default:
		break;
	}
	return {};
}

bool BzardHistoryModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (row || count > 1)
		return false;

	beginInsertRows(parent, 0, 0);
	endInsertRows();
	return true;
}

bool BzardHistoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (!iqHistory)
		return false;

	auto &list = iqHistory->historyList;
	if (static_cast<size_t>(row) >= list.size() || row + count <= 0)
		return false;

	auto beginRow = qMax(0, row);
	auto endRow = qMin(row + count - 1, static_cast<int>(list.size() - 1));

	beginRemoveRows(parent, beginRow, endRow);

	while (beginRow <= endRow) {
		iqHistory->removeHistoryNotification(beginRow);
		++beginRow;
	}

	endRemoveRows();
	return true;
}

QHash<int, QByteArray> BzardHistoryModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[Id_Role] = "id_";
	roles[ApplicationRole] = "application";
	roles[TitleRole] = "title";
	roles[BodyRole] = "body";
	roles[IconUrlRole] = "iconUrl";
	return roles;
}

void BzardHistoryModel::onHistoryRowInserted() { insertRow(0); }
