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

#ifndef GAMETABLESDIALOG_H
#define GAMETABLESDIALOG_H

#include <QtWidgets>
#include <QDialog>

class GamesListModel;
class GameTableView;
class CCGame;

namespace Ui
{
    class GameTablesDialog;
}

class GameTablesDialog : public QDialog
{
    Q_OBJECT

public:
    static const int TABIX_INPUT;
    static const int TABIX_PREVIOUS;
    static const int TABIX_BOOKMARKS;
    static const int TABIX_HIGHSCORES;

    GameTablesDialog(QWidget* parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~GameTablesDialog();
    void ShowTab(int tabix);

    GameTableView* mPrevTableView;
    GameTableView* mSavedTableView;
    GameTableView* mHighTableView;

public slots:
    void ShowInputSlot();
    void ShowPrevSlot();
    void ShowSavedSlot();
    void ShowHighSlot();
    void HoverRowSlot(const int row);
    void ReadSettingsGamesSlot();
    void SetSettingsSlot();
    void InsertHighScoreSlot(CCGame* g);
    void InsertPrevGameSlot(CCGame* g);
    void InsertSavedGameSlot(CCGame* g);
    void NewGameInputSlot(CCGame* g);
    void OkBtnSlot();
    void ClearListSlot();

    void CurrentTabChanged(int ix);
    virtual void reject();

signals:
    void NewGameInputSignal(CCGame* g);

protected:
    void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent* e);

private:
    void Init();
    void SetRowHeight();
    GamesListModel* GetCurModel() const;
    GameTableView* GetCurView() const;
    void SetGameProperties(const CCGame* g);
    QString FormatDuration(const uint dur) const;

    CCGame* mLastGameProperties;

    Ui::GameTablesDialog *ui;

private slots:
    void GameNoInpChangedSlot(const QString &txt);
    void PlayInputSlot();
    void ValidNameCommitSlot();

};

#endif // GAMETABLESDIALOG_H
