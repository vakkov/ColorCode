/* ColorCode, a free MasterMind clone with built in solver
 * Copyright (C) 2009  Dirk Laebisch
 * http://www.laebisch.com/
 *
 * ColorCode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ColorCode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ColorCode. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMESLISTMODEL_H
#define GAMESLISTMODEL_H

#include <QAbstractTableModel>
#include <QTableView>
#include <QHeaderView>
#include <QStringList>
#include "ccgame.h"
#include "buttonscell.h"
#include "gametableview.h"

class GamesListModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        GamesListModel(QObject* parent = 0);

        static const int MODEL_ID_DEFAULT;
        static const int MODEL_ID_PREV;
        static const int MODEL_ID_SAVED;
        static const int MODEL_ID_HIGH;

        static const int COL_RANKING;
        static const int COL_SCORE;
        static const int COL_USERNAME;
        static const int COL_DELETE;
        static const int COL_GAMENO;
        static const int COL_DATE;
        static const int COL_TIME;
        static const int COL_CCNT;
        static const int COL_PCNT;
        static const int COL_DOUBLES;

        static bool SortListScore(CCGame a, CCGame b);
        static bool SortListTime(CCGame a, CCGame b);

        virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

        int GetId();
        QString GetListStr() const;
        void SetView(GameTableView* view);
        void SetMaxSize(const int max);
        void SetRowHColors(QString fg, QString bg);
        virtual void InsertRow(CCGame g);
        void EditField();
        void ReadList(const QString &str);
        void SetEditIndex(QModelIndex ix);
        QModelIndex GetEditIndex() const;
        QString GetEditValue() const;
        const CCGame* GetGameAt(const int row) const;

        virtual int GetColIx(const int ix) const = 0;
        virtual int GetMaxColCnt() const = 0;

    public slots:
        void HoverRowSlot(int row);

    signals:
        void CloseEditorSignal(const QModelIndex &index);
        void SaveListGameSignal(CCGame* g);
        void NewGameInputSignal(CCGame* g);
        void ValidNameCommitSignal();

    protected:
        QString GetHHeaderLabel(const int col) const;
        virtual void DoSort(QList<CCGame> &list) = 0;
        void LimitGamesListSize();

        int mId;
        int mMaxSize;
        int mHoverRow;
        GameTableView* mView;
        QList<CCGame> mGamesList;
        QModelIndex mEditIndex;
        QColor mRowHFg;
        QColor mRowHBg;

};

#endif // GAMESLISTMODEL_H
