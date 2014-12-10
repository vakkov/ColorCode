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

#include "highscoresmodel.h"
#include "gametableview.h"

HighScoresModel* GetHighScoresModel()
{
    static HighScoresModel* m = new HighScoresModel();
    return m;
}

HighScoresModel::HighScoresModel(QObject* parent) : GamesListModel(parent)
{
    mId = MODEL_ID_HIGH;
    mEditIndex = QModelIndex();
    mMaxSize = 20;
}

bool HighScoresModel::IsValidHighScore(const int score) const
{
    if (mGamesList.size() < mMaxSize || mGamesList.last().mScore < score)
    {
        return true;
    }
    return false;
}

void HighScoresModel::InsertRow(CCGame g)
{
    if (mEditIndex.isValid())
    {
        emit CloseEditorSignal(mEditIndex);
    }
    mView->clearSelection();

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mGamesList.push_back(g);
    endInsertRows();

    QModelIndex mix = index(rowCount() - 1, GetColIx(COL_DELETE), QModelIndex());
    mView->openPersistentEditor(mix);

    DoSort(mGamesList);
    LimitGamesListSize();
    int ix = mGamesList.indexOf(g);
    emit layoutChanged();
    if (ix > -1)
    {
        mEditIndex = createIndex(ix, GetColIx(COL_USERNAME));
        EditField();
    }
}

int HighScoresModel::GetColIx(const int ix) const
{
    int ret = -1;
    if (ix == COL_RANKING)
    {
        ret = 0;
    }
    else if (ix == COL_SCORE)
    {
        ret = 1;
    }
    else if (ix == COL_USERNAME)
    {
        ret = 2;
    }
    else if (ix == COL_DELETE)
    {
        ret = 3;
    }
    return ret;
}

int HighScoresModel::GetMaxColCnt() const
{
    return 4;
}

void HighScoresModel::DoSort(QList<CCGame> &list)
{
    qSort(list.begin(), list.end(), GamesListModel::SortListScore);
}
