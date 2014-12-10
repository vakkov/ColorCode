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

#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QStyledItemDelegate>
#include "buttonscell.h"
#include "highscoresmodel.h"
#include "gametableview.h"
#include "gameslistmodel.h"

class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    public:
        ButtonDelegate(QWidget* parent = 0);

        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget* editor, const QModelIndex &index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const;

    private:
        GamesListModel* GetGamesListModel(const QModelIndex &index) const;

    private slots:
        void ButtonClickedSlot();
};

#endif // BUTTONDELEGATE_H
