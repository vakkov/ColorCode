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

#include "prevgamesmodel.h"
#include "gametableview.h"

PrevGamesModel* GetPrevGamesModel()
{
    static PrevGamesModel* m = new PrevGamesModel();
    return m;
}

PrevGamesModel::PrevGamesModel(QObject* parent) : GamesListModel(parent)
{
    mId = MODEL_ID_PREV;
    mEditIndex = QModelIndex();
    mMaxSize = 20;
}

int PrevGamesModel::GetColIx(const int ix) const
{
    int ret = -1;
    if (ix == COL_RANKING)
    {
        ret = 0;
    }
    else if (ix == COL_DATE)
    {
        ret = 1;
    }
    else if (ix == COL_TIME)
    {
        ret = 2;
    }
    else if (ix == COL_DELETE)
    {
        ret = 3;
    }
    return ret;
}

int PrevGamesModel::GetMaxColCnt() const
{
    return 4;
}

void PrevGamesModel::DoSort(QList<CCGame> &list)
{
    qSort(list.begin(), list.end(), GamesListModel::SortListTime);
}
