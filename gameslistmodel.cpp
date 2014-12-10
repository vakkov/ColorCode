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

#include "gameslistmodel.h"
#include "gametableview.h"

const int GamesListModel::MODEL_ID_DEFAULT = 0;
const int GamesListModel::MODEL_ID_PREV    = 1;
const int GamesListModel::MODEL_ID_SAVED   = 2;
const int GamesListModel::MODEL_ID_HIGH    = 3;

const int GamesListModel::COL_RANKING  = 0;
const int GamesListModel::COL_SCORE    = 1;
const int GamesListModel::COL_USERNAME = 2;
const int GamesListModel::COL_DELETE   = 3;
const int GamesListModel::COL_GAMENO   = 4;
const int GamesListModel::COL_DATE     = 5;
const int GamesListModel::COL_TIME     = 6;
const int GamesListModel::COL_CCNT     = 7;
const int GamesListModel::COL_PCNT     = 8;
const int GamesListModel::COL_DOUBLES  = 9;

bool GamesListModel::SortListScore(CCGame a, CCGame b)
{
    if (a.GetScore() > b.GetScore())
    {
        return true;
    }
    return false;
}

bool GamesListModel::SortListTime(CCGame a, CCGame b)
{
    if (a.mTime > b.mTime)
    {
        return true;
    }
    return false;
}

GamesListModel::GamesListModel(QObject* parent) : QAbstractTableModel(parent)
{
    mId = MODEL_ID_DEFAULT;
    mEditIndex = QModelIndex();
    mMaxSize = 20;
    mHoverRow = -1;
    mRowHFg = QApplication::palette().color(QPalette::Active, QPalette::ButtonText);
    mRowHBg = QApplication::palette().color(QPalette::Active, QPalette::Button);
}

int GamesListModel::GetId()
{
    return mId;
}

QString GamesListModel::GetListStr() const
{
    QStringList strl;
    for (int i = 0; i < mGamesList.size(); ++i)
    {
        strl << mGamesList.at(i).ToString();
    }
    return strl.join(";");
}

void GamesListModel::SetView(GameTableView* view)
{
    mView = view;
}

void GamesListModel::SetMaxSize(const int max)
{
    if (max != mMaxSize)
    {
        mMaxSize = max;
        LimitGamesListSize();
    }
}

void GamesListModel::SetRowHColors(QString fg, QString bg)
{
    mRowHFg.setNamedColor(fg);
    mRowHBg.setNamedColor(bg);
}

void GamesListModel::InsertRow(CCGame g)
{
    mView->clearSelection();

    bool found = false;
    for (int i = 0; i < mGamesList.size(); ++i)
    {
        if (mGamesList.at(i).mGameNo == g.mGameNo)
        {
            mGamesList.replace(i, g);
            found = true;
            break;
        }
    }

    if (!found)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        mGamesList.push_back(g);
        endInsertRows();
        QModelIndex mix = index(rowCount() - 1, GetColIx(COL_DELETE), QModelIndex());
        mView->openPersistentEditor(mix);
    }

    DoSort(mGamesList);
    LimitGamesListSize();
    emit layoutChanged();
}

void GamesListModel::LimitGamesListSize()
{
    if (!mGamesList.isEmpty() && mGamesList.size() > mMaxSize)
    {
        removeRows(mMaxSize, mGamesList.size() - mMaxSize);
    }
}

void GamesListModel::EditField()
{
    if (mView != NULL && mEditIndex.isValid())
    {
        mView->setCurrentIndex(mEditIndex);
        mView->selectRow(mEditIndex.row());
        mView->edit(mEditIndex);
    }
}

void GamesListModel::SetEditIndex(QModelIndex ix)
{
    mEditIndex = ix;
}

QModelIndex GamesListModel::GetEditIndex() const
{
    return mEditIndex;
}

QString GamesListModel::GetEditValue() const
{
    QString val = "";
    const CCGame* eg;
    if (mEditIndex.isValid())
    {
        eg = GetGameAt(mEditIndex.row());
        val = eg->GetUserName();
    }
    return val;
}

const CCGame* GamesListModel::GetGameAt(const int row) const
{
    if (row > -1 && mGamesList.size() > row)
    {
        return &mGamesList.at(row);
    }
    return NULL;
}

void GamesListModel::ReadList(const QString &str)
{
    mEditIndex = QModelIndex();
    if (rowCount() > 0)
    {
        removeRows(0, rowCount());
    }

    QStringList games = str.split(QString(";"));
    CCGame* g;
    QModelIndex mix;
    for (int i = 0; i < games.size(); ++i)
    {
        g = new CCGame(games.at(i));
        if (g->IsValidGame())
        {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            mGamesList.push_back(*g);
            endInsertRows();

            mix = index(rowCount() - 1, GetColIx(COL_DELETE), QModelIndex());
            mView->openPersistentEditor(mix);
        }
    }
    DoSort(mGamesList);
    LimitGamesListSize();
    emit layoutChanged();
}

void GamesListModel::HoverRowSlot(int row)
{
    if (mHoverRow == row)
    {
        return;
    }

    int tmprow;
    if (row == -1)
    {
        tmprow = mHoverRow;
    }
    else
    {
        tmprow = row;
    }

    mHoverRow = row;
    emit dataChanged(index(tmprow, 0), index(tmprow, columnCount() - 1));
}

bool GamesListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > rowCount() || count == 0)
    {
        return false;
    }

    if (mEditIndex.isValid())
    {
        if (mEditIndex.row() >= row && mEditIndex.row() < row + count)
        {
            mEditIndex = QModelIndex();
        }
        else if (mEditIndex.row() >= row + count)
        {
            mEditIndex = createIndex(mEditIndex.row() - count, GetColIx(COL_USERNAME));
        }
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i)
    {
        mGamesList.removeAt(row);
    }
    endRemoveRows();
    return true;
}

Qt::ItemFlags GamesListModel::flags(const QModelIndex &index) const
{
    if (index.isValid() && index == mEditIndex)
    {
        return Qt::ItemIsEnabled|Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled;
}

bool GamesListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole && index.isValid() && index.row() < rowCount())
    {
        if (index.column() == GetColIx(COL_USERNAME))
        {
            if (index == mEditIndex)
            {
                CCGame g = mGamesList.at(index.row());
                g.SetUserName(value.toString());
                mGamesList.replace(index.row(), g);
                if (g.GetUserName() != "")
                {
                    emit ValidNameCommitSignal();
                }
            }
        }
        else if (index.column() == GetColIx(COL_DELETE))
        {
            bool ok;
            int aid = value.toInt(&ok);
            if (ok)
            {
                if (aid == ButtonsCell::ACTIONID_DELETE)
                {
                    removeRows(index.row(), 1);
                }
                else
                {
                    CCGame g = mGamesList.at(index.row());

                    if (aid == ButtonsCell::ACTIONID_SAVE)
                    {
                        CCGame* gc = new CCGame(g.ToString());
                        emit SaveListGameSignal(gc);
                    }
                    else if (aid == ButtonsCell::ACTIONID_PLAY)
                    {
                        CCGame* gc = new CCGame(g.ToString());
                        emit NewGameInputSignal(gc);
                    }
                    else if (aid == ButtonsCell::ACTIONID_COPY)
                    {
                        QClipboard* cb = QApplication::clipboard();
                        cb->setText(QString::number(g.mGameNo));
                    }
                }
            }
        }

        return true;
    }
    return false;
}

int GamesListModel::rowCount(const QModelIndex &) const
{
    return mGamesList.size();
}

int GamesListModel::columnCount(const QModelIndex &) const
{
    return GetMaxColCnt();
}

QVariant GamesListModel::data(const QModelIndex &index, int role) const
{
    if (!(index.row() < mGamesList.size()) || !index.isValid())
    {
        return QVariant();
    }
    else if (role == Qt::TextAlignmentRole)
    {
        if (index.column() == GetColIx(COL_USERNAME)
            || index.column() == GetColIx(COL_DATE)
            || index.column() == GetColIx(COL_TIME)
            || index.column() == GetColIx(COL_DOUBLES))
        {
            return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index == mEditIndex)
        {
            return QVariant(QApplication::palette().color(QPalette::Active, QPalette::Highlight));
        }
        else if (index.row() == mHoverRow)
        {
            return QVariant(mRowHBg);
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (index == mEditIndex)
        {
            return QVariant(QApplication::palette().color(QPalette::Active, QPalette::HighlightedText));
        }
        else if (index.row() == mHoverRow)
        {
            return QVariant(mRowHFg);
        }
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        CCGame g = mGamesList.at(index.row());
        if (index.column() == GetColIx(COL_RANKING))
        {
            return index.row() + 1;
        }
        else if (index.column() == GetColIx(COL_USERNAME))
        {
            return g.GetUserName();
        }
        else if (index.column() == GetColIx(COL_SCORE))
        {
            return g.GetScore();
        }
        else if (index.column() == GetColIx(COL_DATE))
        {
            return g.GetDatePartStr();
        }
        else if (index.column() == GetColIx(COL_TIME))
        {
            return g.GetTimePartStr();
        }
        else if (index.column() == GetColIx(COL_CCNT))
        {
            return g.mColorCnt;
        }
        else if (index.column() == GetColIx(COL_PCNT))
        {
            return g.mPegCnt;
        }
        else if (index.column() == GetColIx(COL_GAMENO))
        {
            return g.mGameNo;
        }
        else if (index.column() == GetColIx(COL_DOUBLES))
        {
            return (g.mDoubles == 1) ? tr("Yes") : tr("No");
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

QVariant GamesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return GetHHeaderLabel(section);
    }
    else if (role == Qt::DecorationRole && orientation == Qt::Horizontal)
    {
        if (section == GetColIx(COL_DOUBLES))
        {
            return QVariant(QIcon(":/img/same_color_header.png"));
        }
    }
    else if (role == Qt::ToolTipRole && orientation == Qt::Horizontal)
    {
        if (section == GetColIx(COL_DOUBLES))
        {
            return QVariant(QString(tr("Pegs of Same Color")));
        }
        else if (section == GetColIx(COL_DELETE))
        {
            return QVariant(QString(tr("Actions")));
        }
        else if (section == GetColIx(COL_RANKING))
        {
            return QVariant(QString(tr("Rank")));
        }
    }
    return QVariant();
}

QString GamesListModel::GetHHeaderLabel(const int col) const
{
    QString str;
    if (col == GetColIx(COL_RANKING))
    {
        str = "";
    }
    else if (col == GetColIx(COL_USERNAME))
    {
        str = tr("User");
    }
    else if (col == GetColIx(COL_SCORE))
    {
        str = tr("Score");
    }
    else if (col == GetColIx(COL_DATE))
    {
        str = tr("Date");
    }
    else if (col == GetColIx(COL_TIME))
    {
        str = tr("Time");
    }
    else if (col == GetColIx(COL_CCNT))
    {
        str = tr("Colors");
    }
    else if (col == GetColIx(COL_PCNT))
    {
        str = tr("Slots");
    }
    else if (col == GetColIx(COL_GAMENO))
    {
        str = tr("Game No.");
    }
    else
    {
        str = "";
    }
    return str;
}
