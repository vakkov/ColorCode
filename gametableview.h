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

#ifndef GAMETABLEVIEW_H
#define GAMETABLEVIEW_H

#include <QTableView>

class GamesListModel;

class GameTableView : public QTableView
{
    Q_OBJECT

    public:
        GamesListModel* mGamesListModel;
        GameTableView(QWidget * parent = 0);
        void SetGamesListModel(GamesListModel* glm);

    public slots:
        void CloseEditorSlot(const QModelIndex &index);

    signals:
        void HoverRowSignal(const int row);

    protected:
        void mouseMoveEvent(QMouseEvent* e);
        void wheelEvent(QWheelEvent* e);
        void leaveEvent(QEvent* e);

    protected slots:
        void ScrollSliderChangedSlot(int v = 0);

    private:
        int mHoverRow;
        void SetHoverRowByY(const int y);
};

#endif // GAMETABLEVIEW_H
