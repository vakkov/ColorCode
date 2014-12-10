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

GameTableView::GameTableView(QWidget *parent) : QTableView(parent)
{
    setFocusPolicy(Qt::NoFocus);

    mGamesListModel = NULL;
    mHoverRow = -1;

    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_AlwaysShowToolTips, true);

    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(18);
    verticalHeader()->hide();
    horizontalHeader()->setFixedHeight(18);

    setGridStyle(Qt::NoPen);
    setShowGrid(false);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setAlternatingRowColors(true);

    setFrameStyle(QFrame::NoFrame);
    setFont(QApplication::font());
    horizontalHeader()->setHighlightSections(false);

    verticalScrollBar()->setTracking(true);
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(ScrollSliderChangedSlot(int)));
    connect(verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(ScrollSliderChangedSlot()));

    viewport()->setMouseTracking(true);
    viewport()->setAttribute(Qt::WA_Hover, true);
    viewport()->setAttribute(Qt::WA_AlwaysShowToolTips, true);

    viewport()->setFocus();
}

void GameTableView::SetGamesListModel(GamesListModel* glm)
{
    mGamesListModel = glm;
    setModel(glm);
    connect(this, SIGNAL(HoverRowSignal(int)), mGamesListModel, SLOT(HoverRowSlot(int)));
}

void GameTableView::CloseEditorSlot(const QModelIndex &index)
{
    if (mGamesListModel != NULL && index.isValid() && index.column() == mGamesListModel->GetColIx(HighScoresModel::COL_USERNAME))
    {
        closeEditor(indexWidget(index), QAbstractItemDelegate::SubmitModelCache);
    }
}

void GameTableView::ScrollSliderChangedSlot(int)
{
    SetHoverRowByY((mapFromGlobal(QCursor::pos()).y() - horizontalHeader()->height()));
}

void GameTableView::SetHoverRowByY(const int y)
{
    int row = rowAt(y);
    if (row == mHoverRow)
    {
        return;
    }

    mHoverRow = row;

    emit HoverRowSignal(mHoverRow);
}

void GameTableView::mouseMoveEvent(QMouseEvent* e)
{
    QTableView::mouseMoveEvent(e);
    SetHoverRowByY(e->y());
}

void GameTableView::wheelEvent(QWheelEvent* e)
{
    QTableView::wheelEvent(e);
    SetHoverRowByY(e->y());
}

void GameTableView::leaveEvent(QEvent*)
{
    if (mHoverRow == -1)
    {
        return;
    }
    mHoverRow = -1;
    emit HoverRowSignal(mHoverRow);
}

