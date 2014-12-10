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

#ifndef HIGHSCORESMODEL_H
#define HIGHSCORESMODEL_H

#include <QAbstractTableModel>
#include <QTableView>
#include <QHeaderView>
#include <QStringList>
#include "ccgame.h"
#include "buttonscell.h"
#include "gametableview.h"
#include "gameslistmodel.h"


class HighScoresModel : public GamesListModel
{
    Q_OBJECT

    public:
        HighScoresModel(QObject* parent = 0);

        virtual void InsertRow(CCGame g);
        virtual int GetColIx(const int ix) const;
        virtual int GetMaxColCnt() const;
        bool IsValidHighScore(const int score) const;

    protected:
        virtual void DoSort(QList<CCGame> &list);

};

HighScoresModel* GetHighScoresModel();

#endif // HIGHSCORESMODEL_H
