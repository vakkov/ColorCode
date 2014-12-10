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

#include "gametablesdialog.h"
#include "ui_gametablesdialog.h"
#include "gametableview.h"
#include "highscoresmodel.h"
#include "prevgamesmodel.h"
#include "savedgamesmodel.h"
#include "buttondelegate.h"
#include "ccsolver.h"
#include "ccgame.h"
#include "settings.h"

const int GameTablesDialog::TABIX_INPUT      = 0;
const int GameTablesDialog::TABIX_PREVIOUS   = 1;
const int GameTablesDialog::TABIX_BOOKMARKS  = 2;
const int GameTablesDialog::TABIX_HIGHSCORES = 3;


GameTablesDialog::GameTablesDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), ui(new Ui::GameTablesDialog)
{
    ui->setupUi(this);
    Init();
}

GameTablesDialog::~GameTablesDialog()
{
    delete ui;
}

void GameTablesDialog::Init()
{
    setModal(true);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_AlwaysShowToolTips, true);

    mLastGameProperties = NULL;

    ui->mDescriptionTe->setFocusPolicy(Qt::NoFocus);
    ui->mDescriptionTe->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->mDescriptionTe->setText(tr("Insert an arbitrary game number greater than 0 in the input field above. <br><br>"
                                   "The game properties like color count, slot count etc. will be displayed in the corresponding area above.<br><br>"
                                   "Click the Play button or press key Enter, to play the game. If a game is currently running, it will be aborted."));

    ui->mGameNoLe->setValidator(new QIntValidator(1, INT_MAX, this));

    connect(ui->mGameNoLe, SIGNAL(textChanged(const QString)), this, SLOT(GameNoInpChangedSlot(const QString)));
    

    mPrevTableView = new GameTableView();
    mPrevTableView->SetGamesListModel(GetPrevGamesModel());
    mPrevTableView->setColumnWidth(GetPrevGamesModel()->GetColIx(GamesListModel::COL_RANKING), 20);
    mPrevTableView->setColumnWidth(GetPrevGamesModel()->GetColIx(GamesListModel::COL_DATE), 90);
    mPrevTableView->setColumnWidth(GetPrevGamesModel()->GetColIx(GamesListModel::COL_TIME), 90);
    mPrevTableView->setColumnWidth(GetPrevGamesModel()->GetColIx(GamesListModel::COL_DELETE), 40);

    mPrevTableView->setItemDelegateForColumn(GetPrevGamesModel()->GetColIx(GamesListModel::COL_RANKING), new ButtonDelegate());
    mPrevTableView->setItemDelegateForColumn(GetPrevGamesModel()->GetColIx(GamesListModel::COL_DATE), new ButtonDelegate());
    mPrevTableView->setItemDelegateForColumn(GetPrevGamesModel()->GetColIx(GamesListModel::COL_TIME), new ButtonDelegate());
    mPrevTableView->setItemDelegateForColumn(GetPrevGamesModel()->GetColIx(GamesListModel::COL_DELETE), new ButtonDelegate());

    GetPrevGamesModel()->SetView(mPrevTableView);

    ui->mPrevTab->layout()->addWidget(mPrevTableView);

    connect(mPrevTableView, SIGNAL(HoverRowSignal(int)), this, SLOT(HoverRowSlot(int)));
    connect(GetPrevGamesModel(), SIGNAL(CloseEditorSignal(QModelIndex)), mPrevTableView, SLOT(CloseEditorSlot(QModelIndex)));
    connect(GetPrevGamesModel(), SIGNAL(NewGameInputSignal(CCGame*)), this, SLOT(NewGameInputSlot(CCGame*)));


    mSavedTableView = new GameTableView();
    mSavedTableView->SetGamesListModel(GetSavedGamesModel());
    mSavedTableView->setColumnWidth(GetSavedGamesModel()->GetColIx(GamesListModel::COL_GAMENO), 70);
    mSavedTableView->setColumnWidth(GetSavedGamesModel()->GetColIx(GamesListModel::COL_CCNT), 50);
    mSavedTableView->setColumnWidth(GetSavedGamesModel()->GetColIx(GamesListModel::COL_PCNT), 50);
    mSavedTableView->setColumnWidth(GetSavedGamesModel()->GetColIx(GamesListModel::COL_DOUBLES), 30);
    mSavedTableView->setColumnWidth(GetSavedGamesModel()->GetColIx(GamesListModel::COL_DELETE), 40);

    mSavedTableView->setItemDelegateForColumn(GetSavedGamesModel()->GetColIx(GamesListModel::COL_GAMENO), new ButtonDelegate());
    mSavedTableView->setItemDelegateForColumn(GetSavedGamesModel()->GetColIx(GamesListModel::COL_CCNT), new ButtonDelegate());
    mSavedTableView->setItemDelegateForColumn(GetSavedGamesModel()->GetColIx(GamesListModel::COL_PCNT), new ButtonDelegate());
    mSavedTableView->setItemDelegateForColumn(GetSavedGamesModel()->GetColIx(GamesListModel::COL_DOUBLES), new ButtonDelegate());
    mSavedTableView->setItemDelegateForColumn(GetSavedGamesModel()->GetColIx(GamesListModel::COL_DELETE), new ButtonDelegate());

    GetSavedGamesModel()->SetView(mSavedTableView);

    ui->mSavedTab->layout()->addWidget(mSavedTableView);

    connect(mSavedTableView, SIGNAL(HoverRowSignal(int)), this, SLOT(HoverRowSlot(int)));
    connect(GetSavedGamesModel(), SIGNAL(CloseEditorSignal(QModelIndex)), mSavedTableView, SLOT(CloseEditorSlot(QModelIndex)));
    connect(GetSavedGamesModel(), SIGNAL(NewGameInputSignal(CCGame*)), this, SLOT(NewGameInputSlot(CCGame*)));


    mHighTableView = new GameTableView();
    mHighTableView->SetGamesListModel(GetHighScoresModel());
    mHighTableView->setColumnWidth(GetHighScoresModel()->GetColIx(GamesListModel::COL_RANKING), 20);
    mHighTableView->setColumnWidth(GetHighScoresModel()->GetColIx(GamesListModel::COL_SCORE), 60);
    mHighTableView->setColumnWidth(GetHighScoresModel()->GetColIx(GamesListModel::COL_USERNAME), 120);
    mHighTableView->setColumnWidth(GetHighScoresModel()->GetColIx(GamesListModel::COL_DELETE), 40);

    mHighTableView->setItemDelegateForColumn(GetHighScoresModel()->GetColIx(GamesListModel::COL_RANKING), new ButtonDelegate());
    mHighTableView->setItemDelegateForColumn(GetHighScoresModel()->GetColIx(GamesListModel::COL_SCORE), new ButtonDelegate());
    mHighTableView->setItemDelegateForColumn(GetHighScoresModel()->GetColIx(GamesListModel::COL_USERNAME), new ButtonDelegate());
    mHighTableView->setItemDelegateForColumn(GetHighScoresModel()->GetColIx(GamesListModel::COL_DELETE), new ButtonDelegate());

    GetHighScoresModel()->SetView(mHighTableView);

    ui->mHighTab->layout()->addWidget(mHighTableView);

    connect(mHighTableView, SIGNAL(HoverRowSignal(int)), this, SLOT(HoverRowSlot(int)));
    connect(GetHighScoresModel(), SIGNAL(CloseEditorSignal(QModelIndex)), mHighTableView, SLOT(CloseEditorSlot(QModelIndex)));
    connect(GetHighScoresModel(), SIGNAL(NewGameInputSignal(CCGame*)), this, SLOT(NewGameInputSlot(CCGame*)));
    connect(GetHighScoresModel(), SIGNAL(ValidNameCommitSignal()), this, SLOT(ValidNameCommitSlot()));


    connect(GetPrevGamesModel(), SIGNAL(SaveListGameSignal(CCGame*)), this, SLOT(InsertSavedGameSlot(CCGame*)));
    connect(GetHighScoresModel(), SIGNAL(SaveListGameSignal(CCGame*)), this, SLOT(InsertSavedGameSlot(CCGame*)));


    connect(ui->mTabsWidget, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)));

    connect(ui->mCloseBtn, SIGNAL(clicked()), this, SLOT(OkBtnSlot()));
    connect(ui->mClearBtn, SIGNAL(clicked()), this, SLOT(ClearListSlot()));
    connect(ui->mPlayBtn, SIGNAL(clicked()), this, SLOT(PlayInputSlot()));
}

void GameTablesDialog::ShowTab(int tabix)
{
    if (ui->mTabsWidget->currentIndex() != tabix)
    {
        ui->mTabsWidget->setCurrentIndex(tabix);
    }
    else
    {
        CurrentTabChanged(tabix);
    }
    show();
}

void GameTablesDialog::ShowInputSlot()
{
    ShowTab(TABIX_INPUT);
}

void GameTablesDialog::ShowPrevSlot()
{
    ShowTab(TABIX_PREVIOUS);
}

void GameTablesDialog::ShowSavedSlot()
{
    ShowTab(TABIX_BOOKMARKS);
}

void GameTablesDialog::ShowHighSlot()
{
    ShowTab(TABIX_HIGHSCORES);
}


void GameTablesDialog::HoverRowSlot(const int row)
{
    if (row > -1)
    {
        GamesListModel* glm = GetCurModel();
        if (glm != NULL)
        {
            SetGameProperties(glm->GetGameAt(row));
        }
        else
        {
            SetGameProperties(NULL);
        }
    }
    else
    {
        SetGameProperties(NULL);
    }
}

GamesListModel* GameTablesDialog::GetCurModel() const
{
    GamesListModel* glm;

    int ix = ui->mTabsWidget->currentIndex();
    if (ix == TABIX_PREVIOUS)
    {
        glm = qobject_cast<GamesListModel*>(GetPrevGamesModel());
    }
    else if (ix == TABIX_BOOKMARKS)
    {
        glm = qobject_cast<GamesListModel*>(GetSavedGamesModel());
    }
    else if (ix == TABIX_HIGHSCORES)
    {
        glm = qobject_cast<GamesListModel*>(GetHighScoresModel());
    }
    else
    {
        glm = NULL;
    }

    return glm;
}

GameTableView* GameTablesDialog::GetCurView() const
{
    GameTableView* gtv;

    int ix = ui->mTabsWidget->currentIndex();
    if (ix == TABIX_PREVIOUS)
    {
        gtv = qobject_cast<GameTableView*>(mPrevTableView);
    }
    else if (ix == TABIX_BOOKMARKS)
    {
        gtv = qobject_cast<GameTableView*>(mSavedTableView);
    }
    else if (ix == TABIX_HIGHSCORES)
    {
        gtv = qobject_cast<GameTableView*>(mHighTableView);
    }
    else
    {
        gtv = NULL;
    }

    return gtv;
}

void GameTablesDialog::SetGameProperties(const CCGame* g)
{
    if (g != NULL && g->IsValidGame())
    {
        if (mLastGameProperties == NULL || mLastGameProperties != g)
        {
            mLastGameProperties = const_cast<CCGame*>(g);

            int ix = ui->mTabsWidget->currentIndex();
            switch (ix)
            {
                case TABIX_PREVIOUS:
                case TABIX_BOOKMARKS:
                    ui->mUserValLbl->setText("");
                    ui->mDurationValLbl->setText("");
                    ui->mDateValLbl->setText(g->GetDateTimeStr());
                break;
                case TABIX_HIGHSCORES:
                    ui->mUserValLbl->setText(g->GetUserName());
                    ui->mDurationValLbl->setText(FormatDuration(g->mDuration));
                    ui->mDateValLbl->setText(g->GetDateTimeStr());
                break;
                case TABIX_INPUT:
                    ui->mDateValLbl->setText("");
                break;
            }

            ui->mColorCntValLbl->setText(QString::number(g->mColorCnt));
            ui->mPegCntValLbl->setText(QString::number(g->mPegCnt));
            QString str = (g->mDoubles == 1) ? tr("Yes") : tr("No");
            ui->mDoublesValLbl->setText(str);
            ui->mGameNoValLbl->setText(QString::number(g->mGameNo));
        }
    }
    else
    {
        mLastGameProperties = NULL;

        ui->mUserValLbl->setText("");
        ui->mColorCntValLbl->setText("");
        ui->mPegCntValLbl->setText("");
        ui->mDoublesValLbl->setText("");
        ui->mGameNoValLbl->setText("");
        ui->mDateValLbl->setText("");
        ui->mDurationValLbl->setText("");
    }
}

QString GameTablesDialog::FormatDuration(const uint dur) const
{
    QTime t(0, 0, 0, 0);
    t = t.addMSecs(int(dur));
    QString tstr = QString("%1 : %2 : %3 : %4").arg( t.toString("hh"), t.toString("mm"), t.toString("ss"), t.toString("zzz"));
    return tstr;
}

void GameTablesDialog::ReadSettingsGamesSlot()
{
    SetSettingsSlot();

    GetPrevGamesModel()->ReadList(GetSettings()->mPrevGamesStr);
    GetSavedGamesModel()->ReadList(GetSettings()->mSavedGamesStr);
    GetHighScoresModel()->ReadList(GetSettings()->mHighGamesStr);
    clearFocus();
}

void GameTablesDialog::SetSettingsSlot()
{
    GetPrevGamesModel()->SetMaxSize(GetSettings()->mGamesListMaxCnt);
    GetSavedGamesModel()->SetMaxSize(GetSettings()->mGamesListMaxCnt);
    GetHighScoresModel()->SetMaxSize(GetSettings()->mGamesListMaxCnt);
    GetPrevGamesModel()->SetRowHColors(GetSettings()->GetActiveRowHFg(), GetSettings()->GetActiveRowHBg());
    GetSavedGamesModel()->SetRowHColors(GetSettings()->GetActiveRowHFg(), GetSettings()->GetActiveRowHBg());
    GetHighScoresModel()->SetRowHColors(GetSettings()->GetActiveRowHFg(), GetSettings()->GetActiveRowHBg());
}

void GameTablesDialog::InsertHighScoreSlot(CCGame* g)
{
    if (GetSettings()->mHighScoreHandling == Settings::HIGHSCORE_NO)
    {
        return;
    }

    if (GetHighScoresModel()->IsValidHighScore(g->mScore))
    {
        CCGame game = *g;
        game.SetUserName(GetSettings()->mHighScoreUserName);
        if (GetSettings()->mHighScoreHandling != Settings::HIGHSCORE_AUTO || GetSettings()->mHighScoreUserName == "")
        {
            ShowTab(GameTablesDialog::TABIX_HIGHSCORES);
        }
        GetHighScoresModel()->InsertRow(game);
    }
}

void GameTablesDialog::InsertPrevGameSlot(CCGame* g)
{
    CCGame game = *g;
    game.Anonymize();
    GetPrevGamesModel()->InsertRow(game);
}

void GameTablesDialog::InsertSavedGameSlot(CCGame* g)
{
    CCGame game = *g;
    game.Anonymize();
    game.mTime = QDateTime::currentDateTime().toTime_t();
    GetSavedGamesModel()->InsertRow(game);
}

void GameTablesDialog::OkBtnSlot()
{
    close();
}

void GameTablesDialog::ClearListSlot()
{
    GamesListModel* glm = GetCurModel();
    if (glm == NULL)
    {
        return;
    }

    int rcnt = glm->rowCount();
    if (rcnt <= 0)
    {
        return;
    }

    int r = QMessageBox::warning( this,
                                   tr("Message"),
                                   tr("This will permanently remove all entries from the List.\nDo You want to proceed?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);

    if (r == QMessageBox::Yes)
    {
        rcnt = glm->rowCount();
        if (rcnt > 0)
        {
            glm->removeRows(0, rcnt);
        }
    }
}

void GameTablesDialog::PlayInputSlot()
{
    if (ui->mTabsWidget->currentIndex() == TABIX_INPUT
        && ui->mGameNoLe->text() != "")
    {
        uint gameno = ui->mGameNoLe->text().toUInt();
        if (gameno > 0)
        {
            const int* props = ColorCode::INSTANCE->GetCCSolver()->GetGameByNo(gameno);
            CCGame* g = new CCGame("", 0, 0, gameno, 0, 0, 0, ColorCode::MODE_HVM, props[1], props[2], props[3]);
            NewGameInputSlot(g);
        }
    }
}

void GameTablesDialog::GameNoInpChangedSlot(const QString &txt)
{
    if (txt != "" && txt.toUInt() > 0)
    {
        const int* props = ColorCode::INSTANCE->GetCCSolver()->GetGameByNo(txt.toUInt());
        CCGame* g = new CCGame("", 0, 0, txt.toUInt(), 0, 0, 0, ColorCode::MODE_HVM, props[1], props[2], props[3]);
        SetGameProperties(g);
    }
    else
    {
        SetGameProperties(NULL);
    }
}

void GameTablesDialog::NewGameInputSlot(CCGame* g)
{
    emit NewGameInputSignal(g);
    close();
}

void GameTablesDialog::ValidNameCommitSlot()
{
    if (ui->mTabsWidget->currentIndex() == TABIX_HIGHSCORES)
    {
        ui->mCloseBtn->setFocus();
    }
}

void GameTablesDialog::SetRowHeight()
{
    for (int i = 0; i < GetHighScoresModel()->rowCount(); ++i)
    {
        mHighTableView->setRowHeight(i, 18);
    }
}

void GameTablesDialog::CurrentTabChanged(int ix)
{
    mLastGameProperties = NULL;
    SetGameProperties(NULL);

    QString str = "";
    QIcon ico;
    switch (ix)
    {
        case TABIX_INPUT:
            ui->mPlayBtn->setVisible(true);
            ui->mClearBtn->setVisible(false);
            ui->mGameNoLe->setFocus();
            ui->mUserLbl->setEnabled(false);
            ui->mDurationLbl->setEnabled(false);
            ui->mDateLbl->setEnabled(false);
            GameNoInpChangedSlot(ui->mGameNoLe->text());
            str = tr("Game Number Input");
            ico = QIcon(":/img/document-new.png");
        break;
        case TABIX_PREVIOUS:
            ui->mPlayBtn->setVisible(false);
            ui->mClearBtn->setVisible(true);
            ui->mUserLbl->setEnabled(false);
            ui->mDurationLbl->setEnabled(false);
            ui->mDateLbl->setEnabled(true);
            str = tr("Recent Games");
            ico = QIcon(":/img/history.png");
        break;
        case TABIX_BOOKMARKS:
            ui->mPlayBtn->setVisible(false);
            ui->mClearBtn->setVisible(true);
            ui->mUserLbl->setEnabled(false);
            ui->mDurationLbl->setEnabled(false);
            ui->mDateLbl->setEnabled(true);
            str = tr("Bookmarked Games");
            ico = QIcon(":/img/bookmark.png");
        break;
        case TABIX_HIGHSCORES:
            ui->mPlayBtn->setVisible(false);
            ui->mClearBtn->setVisible(true);
            ui->mUserLbl->setEnabled(true);
            ui->mDurationLbl->setEnabled(true);
            ui->mDateLbl->setEnabled(true);
            str = tr("High Score List");
            ico = QIcon(":/img/games-highscores.png");
        break;
        default:
            str = tr("Game Lists");
        break;
    }
    setWindowTitle(str);
    setWindowIcon(ico);
}

void GameTablesDialog::closeEvent(QCloseEvent* e)
{
    GetHighScoresModel()->EditField();
    if (GetHighScoresModel()->GetEditIndex().isValid() && GetHighScoresModel()->GetEditValue() == "")
    {
        e->ignore();

        int r = QMessageBox::warning( this,
        tr("Message"),
        tr("Please provide a name!\nOtherwise your score won't be saved."),
        QMessageBox::Ok | QMessageBox::Discard,
        QMessageBox::Ok);

        if (r == QMessageBox::Discard)
        {
            GetHighScoresModel()->removeRows(GetHighScoresModel()->GetEditIndex().row(), 1);
            e->accept();
        }
        else
        {
            GetHighScoresModel()->EditField();
        }
    }
    else
    {
        GetHighScoresModel()->SetEditIndex(QModelIndex());
        e->accept();
    }
}

void GameTablesDialog::reject()
{
    close();
}

void GameTablesDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}
