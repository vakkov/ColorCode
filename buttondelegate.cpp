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

#include <QtWidgets>

#include "highscoresmodel.h"
#include "buttondelegate.h"
#include "settings.h"

ButtonDelegate::ButtonDelegate(QWidget* parent) : QStyledItemDelegate(parent)
{

}

GamesListModel* ButtonDelegate::GetGamesListModel(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return NULL;
    }

    GamesListModel* glm = qobject_cast<GamesListModel*>(const_cast<QAbstractItemModel*>(index.model()));
    return glm;
}

QSize ButtonDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* ButtonDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    GamesListModel* glm = GetGamesListModel(index);

    if (glm != NULL && index.column() == glm->GetColIx(GamesListModel::COL_DELETE))
    {
        ButtonsCell* editor;

        if (glm->GetId() != GamesListModel::MODEL_ID_SAVED)
        {
            editor = new ButtonsCell(parent);
        }
        else
        {
            editor = new ButtonsCell(parent, ButtonsCell::ACTIONID_SAVE);
        }

        editor->setMouseTracking(true);
        editor->setAttribute(Qt::WA_Hover, true);
        editor->setAttribute(Qt::WA_AlwaysShowToolTips, true);

        connect(editor, SIGNAL(CellButtonClickedSignal()), this, SLOT(ButtonClickedSlot()));
        return editor;
    }
    else if (glm != NULL && index.column() == glm->GetColIx(GamesListModel::COL_USERNAME))
    {
        QLineEdit* editor = new QLineEdit(parent);
        editor->setMaxLength(Settings::MAX_LENGTH_USERNAME);
#ifdef Q_WS_X11
        editor->setFrame(false);
#endif
        editor->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        return editor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void ButtonDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    GamesListModel* glm = GetGamesListModel(index);
    if (glm != NULL && index.column() == glm->GetColIx(GamesListModel::COL_DELETE))
    {
        ;
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ButtonDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const
{
    if (!index.isValid() || editor == NULL)
    {
        return;
    }

    GamesListModel* glm = GetGamesListModel(index);

    if (glm != NULL && index.column() == glm->GetColIx(GamesListModel::COL_DELETE))
    {
        ButtonsCell* editor = qobject_cast<ButtonsCell*>(sender());
        if (editor != NULL)
        {
            glm->setData(index, editor->GetActionId(), Qt::EditRole);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ButtonDelegate::ButtonClickedSlot()
{
    ButtonsCell* editor = qobject_cast<ButtonsCell*>(sender());
    emit commitData(editor);
}
