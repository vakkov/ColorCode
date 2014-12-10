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

#include "savedgamesmodel.h"
#include "gametableview.h"

SavedGamesModel* GetSavedGamesModel()
{
    static SavedGamesModel* m = new SavedGamesModel();
    return m;
}

SavedGamesModel::SavedGamesModel(QObject* parent) : GamesListModel(parent)
{
    mId = MODEL_ID_SAVED;
    mEditIndex = QModelIndex();
    mMaxSize = 20;
}

int SavedGamesModel::GetColIx(const int ix) const
{
    int ret = -1;
    if (ix == COL_GAMENO)
    {
        ret = 0;
    }
    else if (ix == COL_CCNT)
    {
        ret = 1;
    }
    else if (ix == COL_PCNT)
    {
        ret = 2;
    }
    else if (ix == COL_DOUBLES)
    {
        ret = 3;
    }
    else if (ix == COL_DELETE)
    {
        ret = 4;
    }
    return ret;
}

int SavedGamesModel::GetMaxColCnt() const
{
    return 5;
}

void SavedGamesModel::DoSort(QList<CCGame> &list)
{
    qSort(list.begin(), list.end(), GamesListModel::SortListTime);
}

