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

#include "settings.h"
#include "ccsolver.h"
#include "colorcode.h"
#include "colorpeg.h"
#include "pegrow.h"
#include "rowhint.h"
#include "msg.h"
#include "background.h"
#include "about.h"
#include "prefdialog.h"
#include "solutionrow.h"
#include "graphicsbtn.h"
#include "pegfactory.h"
#include "gamenodisplay.h"
#include "timedisplay.h"
#include "timedisplaybg.h"
#include "highscore.h"
#include "gametablesdialog.h"

using namespace std;

ColorCode* ColorCode::INSTANCE = NULL;

const QString ColorCode::VERSION = "0.7.2";

volatile bool ColorCode::mNoAct       = false;
volatile bool ColorCode::mIgnoreGuess = false;

const int ColorCode::STATE_NONE    = 0;
const int ColorCode::STATE_RUNNING = 1;
const int ColorCode::STATE_PAUSED  = 2;
const int ColorCode::STATE_WON     = 3;
const int ColorCode::STATE_LOST    = 4;
const int ColorCode::STATE_GAVE_UP = 5;
const int ColorCode::STATE_ERROR   = 6;

const int ColorCode::MODE_HVM = 0;
const int ColorCode::MODE_MVH = 1;

const int ColorCode::MAX_COLOR_CNT = 10;
const int ColorCode::MIN_COLOR_CNT = 2;
const int ColorCode::SOLUTION_ROW_IX = -1;

const int ColorCode::LAYER_BG    = 1;
const int ColorCode::LAYER_ROWS  = 2;
const int ColorCode::LAYER_HINTS = 3;
const int ColorCode::LAYER_SOL   = 4;
const int ColorCode::LAYER_MSG   = 5;
const int ColorCode::LAYER_PEGS  = 6;
const int ColorCode::LAYER_BTNS  = 7;
const int ColorCode::LAYER_DRAG  = 8;

const int ColorCode::LEVEL_SETTINGS[5][3] = {
                                                { 2, 2, 1},
                                                { 4, 3, 0},
                                                { 6, 4, 1},
                                                { 8, 4, 1},
                                                {10, 5, 1}
                                            };


int ColorCode::mMaxRowCnt = 10;
int ColorCode::mRowY0     = 130;
int ColorCode::mColorCnt  = 0;
int ColorCode::mPegCnt    = 0;
int ColorCode::mDoubles   = 1;
int ColorCode::mXOffs     = 0;
int ColorCode::mMaxZ      = 0;
int ColorCode::mLevel     = -1;
int ColorCode::mGameMode  = ColorCode::MODE_HVM;

QString ColorCode::GAME_TITLE = "";

ColorCode::ColorCode()
{
    SetState(STATE_NONE);

    GAME_TITLE = tr("ColorCode");

    setWindowTitle(GAME_TITLE + " ");
    setWindowIcon(QIcon(QPixmap(":/img/cc16.png")));
    setIconSize(QSize(16, 16));

    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_AlwaysShowToolTips, true);

    INSTANCE = this;

    QCoreApplication::setOrganizationName("dirks");
    QCoreApplication::setOrganizationDomain("laebisch.com");
    QCoreApplication::setApplicationName("colorcode");

    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    
    Init();
}

ColorCode::~ColorCode()
{
    mIgnoreGuess = true;
    mSettings->WriteSettings();
    WaitForSolver();
}

void ColorCode::Init()
{
    mGameTablesDialog = new GameTablesDialog(this);
    mGameInput = NULL;
    mFinishedGameInput = NULL;
    mPrefDialog = NULL;
    mSettings = GetSettings();
    connect(mSettings, SIGNAL(ReadSettingsGamesDone()), mGameTablesDialog, SLOT(ReadSettingsGamesSlot()));
    connect(this, SIGNAL(NewHighScoreSignal(CCGame*)), mGameTablesDialog, SLOT(InsertHighScoreSlot(CCGame*)));
    connect(mGameTablesDialog, SIGNAL(NewGameInputSignal(CCGame*)), this, SLOT(NewGameInputSlot(CCGame*)));

    mSettings->Init();

    mHighScore = new HighScore();

    mCurRow         = NULL;
    mColorCnt       = 0;
    mGameCnt        = 0;
    mGameId         = 0;
    mGuessCnt       = 0;
    mHintsCnt       = 0;
    mSolverStrength = CCSolver::STRENGTH_HIGH;

    mOrigSize = NULL;

    mMenuBar = menuBar();

    // as long as menuBar's size is invalid populated initially ... done for 4.6
#ifdef Q_WS_X11
#if QT_VERSION < 0x040600
    mScene = new QGraphicsScene(0, 0, 320, 580);
#else
    mScene = new QGraphicsScene(0, 0, 320, 560);
#endif
#else
    mScene = new QGraphicsScene(0, 0, 320, 560);
#endif

    mScene->setBackgroundBrush(QBrush(QColor("#b0b1b3")));

    mView = new QGraphicsView;
    mView->setScene(mScene);
    mView->setGeometry(0, 0, 320, 560);
    mView->setDragMode(QGraphicsView::NoDrag);
    mView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    mView->setContextMenuPolicy(Qt::NoContextMenu);
    mView->setAlignment(Qt::AlignCenter);
    mView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mView->setFrameStyle(QFrame::NoFrame);

    setCentralWidget(mView);

    mBg = new BackGround();
    mScene->addItem(mBg);
    mBg->setPos(0, 0);
    mBg->setZValue(LAYER_BG);

    mMsg = new Msg;
    mScene->addItem(mMsg);
    mMsg->setPos(20, 0);
    mMsg->setZValue(LAYER_MSG);

    mGameNoDisplay = new GameNoDisplay;
    mScene->addItem(mGameNoDisplay);
    mGameNoDisplay->setPos(10, 534);
    mGameNoDisplay->setZValue(LAYER_MSG);

    mDoneBtn = new GraphicsBtn;
    mDoneBtn->SetLabel(tr("Done"));
    mScene->addItem(mDoneBtn);
    mDoneBtn->setPos(84, 118);
    mDoneBtn->setZValue(LAYER_BTNS);
    mDoneBtn->ShowBtn(false);
    connect(mDoneBtn, SIGNAL(BtnPressSignal(GraphicsBtn *)), this, SLOT(DoneBtnPressSlot(GraphicsBtn *)));

    mOkBtn = new GraphicsBtn;
    mOkBtn->SetWidth(38);
    mOkBtn->SetLabel("ok");
    mScene->addItem(mOkBtn);
    mOkBtn->setPos(5, 329);
    mOkBtn->setZValue(LAYER_BTNS);
    mOkBtn->ShowBtn(false);
    connect(mOkBtn, SIGNAL(BtnPressSignal(GraphicsBtn *)), this, SLOT(DoneBtnPressSlot(GraphicsBtn *)));

    mTimerBtn = new GraphicsBtn;
    mTimerBtn->SetWidth(261);
    mTimerBtn->SetLabel("");
    mScene->addItem(mTimerBtn);
    mTimerBtn->setTransform(QTransform::fromScale(0.44, 0.44), true);			
//mTimerBtn->scale(0.44, 0.44);
    mTimerBtn->setPos(102, 110);
    mTimerBtn->setZValue(LAYER_HINTS);
    mTimerBtn->InvertStates();
    mTimerBtn->ShowBtn(true);
    connect(mTimerBtn, SIGNAL(BtnPressSignal(GraphicsBtn *)), this, SLOT(PauseGameSlot(GraphicsBtn *)));

    mSolutionRowBg = new GraphicsBtn;
    mSolutionRowBg->SetEnabled(false);
    mSolutionRowBg->SetWidth(219);
    mSolutionRowBg->SetLabel("");
    mScene->addItem(mSolutionRowBg);
    mSolutionRowBg->setTransform(QTransform::fromScale(1.14, 1.14), true);
    //mSolutionRowBg->scale(1.14, 1.14);
    mSolutionRowBg->setPos(35, 66);
    mSolutionRowBg->setZValue(LAYER_ROWS);
    mSolutionRowBg->SetStatesOpacity(1.0, 0.1);
    mSolutionRowBg->InvertStates();
    mSolutionRowBg->ShowBtn(true);

    mPegFactory = new PegFactory();
    connect(mPegFactory, SIGNAL(SnapPegSignal(ColorPeg*)), this, SLOT(SnapPegSlot(ColorPeg*)));

    mSolver = new CCSolver(this);
    connect(mSolver, SIGNAL(finished()), this, SLOT(SetSolverGuessSlot()));
    mHintsDelayTimer = new QTimer();
    mHintsDelayTimer->setSingleShot(true);
    mHintsDelayTimer->setInterval(500);
    connect(mHintsDelayTimer, SIGNAL(timeout()), this, SLOT(SetAutoHintsSlot()));

    mGameTimerBg = new TimeDisplayBg();
    mScene->addItem(mGameTimerBg);
    mGameTimerBg->setPos(50, 108);
    mGameTimerBg->setZValue(LAYER_SOL);

    mGameTimer = new TimeDisplay();
    mScene->addItem(mGameTimer);
    mGameTimer->setPos(49, 109);
    mGameTimer->setZValue(LAYER_MSG);

    InitActions();
    InitMenus();
    InitToolBars();

    statusBar()->setStyleSheet("QStatusBar::item { border: 0px solid black }; ");
    mStatusLabel = new QLabel();
    statusBar()->addPermanentWidget(mStatusLabel, 0);

    InitSolution();
    InitRows();

    ApplySettingsSlot();
}

void ColorCode::InitSolution()
{
    mSolutionRow = new SolutionRow();
    mSolutionRow->SetIx(SOLUTION_ROW_IX);
    mSolutionRow->setPos(50, 68);
    mSolutionRow->setZValue(LAYER_SOL);
    mScene->addItem(mSolutionRow);
    connect(mSolutionRow, SIGNAL(RemovePegSignal(ColorPeg*)), mPegFactory, SLOT(RemovePegSlot(ColorPeg*)));
    connect(mSolutionRow, SIGNAL(RowSolutionSignal(int)), this, SLOT(RowSolutionSlot(int)));
}

void ColorCode::InitRows()
{
    PegRow* row;
    RowHint* hint;

    int xpos = 60;
    int ypos = mRowY0;
    int i;

    for (i = 0; i < mMaxRowCnt; ++i)
    {
        row = new PegRow();
        row->SetIx(i);
        row->setPos(QPoint(xpos, ypos + (mMaxRowCnt - (i + 1)) * 40));
        row->setZValue(LAYER_ROWS);
        row->SetActive(false);
        mScene->addItem(row);
        mPegRows[i] = row;
        connect(row, SIGNAL(RemovePegSignal(ColorPeg*)), mPegFactory, SLOT(RemovePegSlot(ColorPeg*)));
        connect(row, SIGNAL(RowSolutionSignal(int)), this, SLOT(RowSolutionSlot(int)));

        hint = new RowHint;
        hint->SetIx(i);
        hint->setPos(QPoint(4, ypos + (mMaxRowCnt - (i + 1)) * 40));
        hint->setZValue(LAYER_HINTS);
        hint->SetActive(false);
        mScene->addItem(hint);
        mHintBtns[i] = hint;
        connect(hint, SIGNAL(HintPressedSignal(int)), this, SLOT(HintPressedSlot(int)));
    }
}

void ColorCode::InitActions()
{
    mActNewGame = new QAction(tr("&New Game"), this);
    mActNewGame->setIcon(QIcon(":/img/document-new.png"));
    mActNewGame->setShortcut(tr("Ctrl+N"));
    connect(mActNewGame, SIGNAL(triggered()), this, SLOT(NewGameSlot()));

    mActSelectGame = new QAction(tr("&Start particular Game"), this);
    mActSelectGame->setIcon(QIcon(":/img/document-new.png"));
    mActSelectGame->setShortcut(tr("Ctrl+Alt+N"));
    connect(mActSelectGame, SIGNAL(triggered()), mGameTablesDialog, SLOT(ShowInputSlot()));

    mActCopyGameNo = new QAction(tr("&Copy Game Number"), this);
    mActCopyGameNo->setIcon(QIcon(":/img/edit-copy.png"));
    mActCopyGameNo->setShortcut(tr("Ctrl+C"));
    connect(mActCopyGameNo, SIGNAL(triggered()), this, SLOT(CopyGameNoSlot()));

    mActRestartGame = new QAction(tr("&Restart Game"), this);
    mActRestartGame->setIcon(QIcon(":/img/view-refresh.png"));
    mActRestartGame->setShortcut(tr("Ctrl+Shift+R"));
    connect(mActRestartGame, SIGNAL(triggered()), this, SLOT(RestartGameSlot()));

    mActGiveIn = new QAction(tr("&Throw In The Towel"), this);
    mActGiveIn->setIcon(QIcon(":/img/face-sad.png"));
    mActGiveIn->setShortcut(tr("Ctrl+G"));
    connect(mActGiveIn, SIGNAL(triggered()), this, SLOT(GiveInSlot()));

    mActPauseGame = new QAction(tr("&Have a Break"), this);
    mActPauseGame->setIcon(QIcon(":/img/pause.png"));
    mActPauseGame->setShortcut(tr("Ctrl+P"));
    connect(mActPauseGame, SIGNAL(triggered()), this, SLOT(PauseGameSlot()));

    mActSaveGame = new QAction(tr("&Bookmark Current Game"), this);
    mActSaveGame->setIcon(QIcon(":/img/bookmark_add.png"));
    mActSaveGame->setShortcut(tr("Ctrl+B"));
    connect(mActSaveGame, SIGNAL(triggered()), this, SLOT(SavePrevGameSlot()));

    mActShowHighScores = new QAction(tr("&Highscores"), this);
    mActShowHighScores->setIcon(QIcon(":/img/games-highscores.png"));
    mActShowHighScores->setShortcut(tr("Ctrl+Alt+H"));
    connect(mActShowHighScores, SIGNAL(triggered()), mGameTablesDialog, SLOT(ShowHighSlot()));

    mActShowPrevGames = new QAction(tr("&Recent Games"), this);
    mActShowPrevGames->setIcon(QIcon(":/img/history.png"));
    mActShowPrevGames->setShortcut(tr("Ctrl+Alt+R"));
    connect(mActShowPrevGames, SIGNAL(triggered()), mGameTablesDialog, SLOT(ShowPrevSlot()));

    mActShowSavedGames = new QAction(tr("&Bookmarks"), this);
    mActShowSavedGames->setIcon(QIcon(":/img/bookmark.png"));
    mActShowSavedGames->setShortcut(tr("Ctrl+Alt+B"));
    connect(mActShowSavedGames, SIGNAL(triggered()), mGameTablesDialog, SLOT(ShowSavedSlot()));

    mActExit = new QAction(tr("E&xit"), this);
    mActExit->setIcon(QIcon(":/img/application-exit.png"));
    mActExit->setShortcut(tr("Ctrl+Q"));
    connect(mActExit, SIGNAL(triggered()), this, SLOT(close()));

    mActShowToolbar = new QAction(tr("Show Toolbar"), this);
    mActShowToolbar->setCheckable(true);
    mActShowToolbar->setChecked(true);
    mActShowToolbar->setShortcut(tr("Ctrl+Shift+T"));
    connect(mActShowToolbar, SIGNAL(triggered()), this, SLOT(ShowToolbarSlot()));

    mActShowMenubar = new QAction(tr("Show Menubar"), this);
    mActShowMenubar->setCheckable(true);
    mActShowMenubar->setChecked(true);
    mActShowMenubar->setShortcut(tr("Ctrl+M"));
    connect(mActShowMenubar, SIGNAL(triggered()), this, SLOT(ShowMenubarSlot()));

    mActShowStatusbar = new QAction(tr("Show Statusbar"), this);
    mActShowStatusbar->setCheckable(true);
    mActShowStatusbar->setChecked(true);
    mActShowStatusbar->setShortcut(tr("Ctrl+Shift+B"));
    connect(mActShowStatusbar, SIGNAL(triggered()), this, SLOT(ShowStatusbarSlot()));

    mActShowTimer = new QAction(tr("Show Timer"), this);
    mActShowTimer->setCheckable(true);
    mActShowTimer->setChecked(true);
    mActShowTimer->setShortcut(tr("Ctrl+T"));
    connect(mActShowTimer, SIGNAL(triggered()), this, SLOT(ShowTimerSlot()));

    mActShowGameNo = new QAction(tr("Display Game Number"), this);
    mActShowGameNo->setCheckable(true);
    mActShowGameNo->setChecked(true);
    mActShowGameNo->setShortcut(tr("Ctrl+Shift+N"));
    connect(mActShowGameNo, SIGNAL(triggered()), this, SLOT(ShowGameNoSlot()));

    mActResetColorsOrder = new QAction(tr("Reset Color Order"), this);
    mActResetColorsOrder->setShortcut(tr("Ctrl+Shift+C"));
    connect(mActResetColorsOrder, SIGNAL(triggered()), mPegFactory, SLOT(ResetColorsOrderSlot()));

    mActShowIndicators = new QAction(tr("Show Indicators"), this);
    mActShowIndicators->setCheckable(true);
    mActShowIndicators->setChecked(false);
    mActShowIndicators->setShortcut(tr("Ctrl+Shift+L"));
    connect(mActShowIndicators, SIGNAL(triggered()), this, SLOT(SetIndicatorsSlot()));

    mActDoubles = new QAction(tr("Allow Pegs of the Same Color"), this);
    mActDoubles->setCheckable(true);
    mActDoubles->setChecked(true);
    mActDoubles->setShortcut(tr("Ctrl+Shift+S"));
    connect(mActDoubles, SIGNAL(triggered(bool)), this, SLOT(DoublesChangedSlot(bool)));

    mActDoublesIcon = new QAction(tr("Allow Pegs of Same Color"), this);
    mActDoublesIcon->setCheckable(true);
    mActDoublesIcon->setChecked(true);
    mActDoublesIcon->setToolTip(tr("Disallow Pegs of Same Color"));
    mActDoublesIcon->setIcon(QIcon(":/img/same_color_1.png"));
    connect(mActDoublesIcon, SIGNAL(triggered(bool)), this, SLOT(DoublesChangedSlot(bool)));

    mActAutoClose = new QAction(tr("Close Rows when the last Peg is placed"), this);
    mActAutoClose->setCheckable(true);
    mActAutoClose->setChecked(false);
    mActAutoClose->setShortcut(tr("Ctrl+L"));
    connect(mActAutoClose, SIGNAL(triggered()), this, SLOT(AutoCloseSlot()));

    mActAutoHints = new QAction(tr("Set Hints automatically"), this);
    mActAutoHints->setCheckable(true);
    mActAutoHints->setChecked(false);
    mActAutoHints->setShortcut(tr("Ctrl+Shift+H"));
    connect(mActAutoHints, SIGNAL(triggered()), this, SLOT(AutoHintsSlot()));

    mLaunchHelpAction = new QAction(tr("Online &Help"), this);
    mLaunchHelpAction->setIcon(QIcon(":/img/help.png"));
    mLaunchHelpAction->setShortcut(tr("F1"));
    connect(mLaunchHelpAction, SIGNAL(triggered()), this, SLOT(OnlineHelpSlot()));

    mActAbout = new QAction(tr("About &ColorCode"), this);
    mActAbout->setIcon(QIcon(":/img/help-about.png"));
    mActAbout->setShortcut(tr("Ctrl+A"));
    connect(mActAbout, SIGNAL(triggered()), this, SLOT(AboutSlot()));

    mActAboutQt = new QAction(tr("About &Qt"), this);
    mActAboutQt->setIcon(QIcon(":/img/qt.png"));
    mActAboutQt->setShortcut(tr("Ctrl+I"));
    connect(mActAboutQt, SIGNAL(triggered()), this, SLOT(AboutQtSlot()));

    mActRandRow = new QAction(tr("Fill Row by Random"), this);
    mActRandRow->setIcon(QIcon(":/img/system-switch-user.png"));
    mActRandRow->setShortcut(tr("Ctrl+R"));
    connect(mActRandRow, SIGNAL(triggered()), this, SLOT(RandRowSlot()));

    mActPrevRow = new QAction(tr("Duplicate Previous Row"), this);
    mActPrevRow->setIcon(QIcon(":/img/edit-copy.png"));
    mActPrevRow->setShortcut(tr("Ctrl+D"));
    connect(mActPrevRow, SIGNAL(triggered()), this, SLOT(PrevRowSlot()));

    mActClearRow = new QAction(tr("Clear Row"), this);
    mActClearRow->setIcon(QIcon(":/img/edit-clear.png"));
    mActClearRow->setShortcut(tr("Ctrl+Alt+C"));
    connect(mActClearRow, SIGNAL(triggered()), this, SLOT(ClearRowSlot()));

    mActLevelEasy = new QAction(tr("Beginner (2 Colors, 2 Slots, Doubles)"), this);
    mActLevelEasy->setData(0);
    mActLevelEasy->setCheckable(true);
    mActLevelEasy->setChecked(false);
    connect(mActLevelEasy, SIGNAL(triggered()), this, SLOT(SetLevelSlot()));

    mActLevelClassic = new QAction(tr("Easy (4 Colors, 3 Slots, No Doubles)"), this);
    mActLevelClassic->setData(1);
    mActLevelClassic->setCheckable(true);
    mActLevelClassic->setChecked(false);
    connect(mActLevelClassic, SIGNAL(triggered()), this, SLOT(SetLevelSlot()));

    mActLevelMedium = new QAction(tr("Classic (6 Colors, 4 Slots, Doubles)"), this);
    mActLevelMedium->setData(2);
    mActLevelMedium->setCheckable(true);
    mActLevelMedium->setChecked(true);
    connect(mActLevelMedium, SIGNAL(triggered()), this, SLOT(SetLevelSlot()));

    mActLevelChallenging = new QAction(tr("Challenging (8 Colors, 4 Slots, Doubles)"), this);
    mActLevelChallenging->setData(3);
    mActLevelChallenging->setCheckable(true);
    mActLevelChallenging->setChecked(false);
    connect(mActLevelChallenging, SIGNAL(triggered()), this, SLOT(SetLevelSlot()));

    mActLevelHard = new QAction(tr("Hard (10 Colors, 5 Slots, Doubles)"), this);
    mActLevelHard->setData(4);
    mActLevelHard->setCheckable(true);
    mActLevelHard->setChecked(false);
    connect(mActLevelHard, SIGNAL(triggered()), this, SLOT(SetLevelSlot()));

    mActSetGuess = new QAction(tr("Computer's Guess"), this);
    mActSetGuess->setIcon(QIcon(":/img/business_user.png"));
    mActSetGuess->setShortcut(tr("Ctrl+H"));
    connect(mActSetGuess, SIGNAL(triggered()), this, SLOT(SetGuessSlot()));

    mActSetHints = new QAction(tr("Rate it for me"), this);
    mActSetHints->setIcon(QIcon(":/img/icon_female16.png"));
    mActSetHints->setShortcut(tr("Ctrl+H"));
    connect(mActSetHints, SIGNAL(triggered()), this, SLOT(SetHintsSlot()));

    mActModeHvM = new QAction(tr("Human vs Computer"), this);
    mActModeHvM->setData(MODE_HVM);
    mActModeHvM->setCheckable(true);
    mActModeHvM->setChecked(true);
    connect(mActModeHvM, SIGNAL(triggered()), this, SLOT(SetGameModeSlot()));

    mActModeMvH = new QAction(tr("Computer vs Human"), this);
    mActModeMvH->setData(MODE_MVH);
    mActModeMvH->setCheckable(true);
    mActModeMvH->setChecked(false);
    connect(mActModeMvH, SIGNAL(triggered()), this, SLOT(SetGameModeSlot()));

    mActPreferences = new QAction(tr("Preferences"), this);
    mActPreferences->setIcon(QIcon(":/img/configure.png"));
    mActPreferences->setShortcut(tr("Ctrl+Shift+P"));
    connect(mActPreferences, SIGNAL(triggered()), this, SLOT(OpenPreferencesSlot()));
}

void ColorCode::InitMenus()
{
    mMenuGame = mMenuBar->addMenu(tr("&Game"));
    mMenuGame->addAction(mActNewGame);
    mMenuGame->addAction(mActSelectGame);
    mMenuGame->addAction(mActRestartGame);
    mMenuGame->addSeparator();
    mMenuGame->addAction(mActPauseGame);
    mMenuGame->addAction(mActGiveIn);
    mMenuGame->addSeparator();
    mMenuGame->addAction(mActCopyGameNo);
    mMenuGame->addAction(mActSaveGame);
    mMenuGame->addSeparator();
    mMenuGame->addAction(mActExit);

    mMenuRow = mMenuBar->addMenu(tr("&Row"));
    mMenuRow->addAction(mActRandRow);
    mMenuRow->addAction(mActPrevRow);
    mMenuRow->addAction(mActClearRow);
    mMenuRow->addSeparator();
    mMenuRow->addAction(mActSetGuess);
    mMenuRow->addAction(mActSetHints);
    connect(mMenuRow, SIGNAL(aboutToShow()), this, SLOT(UpdateRowMenuSlot()));


    mMenuGames = mMenuBar->addMenu(tr("Game &Lists"));

    mMenuGames->addAction(mActShowHighScores);
    mMenuGames->addAction(mActShowSavedGames);
    mMenuGames->addAction(mActShowPrevGames);


    mMenuSettings = mMenuBar->addMenu(tr("&Settings"));

    mMenuModes = mMenuSettings->addMenu(tr("Game Mode"));
    mActGroupModes = new QActionGroup(mMenuModes);
    mActGroupModes->setExclusive(true);
    mActGroupModes->addAction(mActModeHvM);
    mActGroupModes->addAction(mActModeMvH);
    QList<QAction *> modeacts = mActGroupModes->actions();
    mMenuModes->addActions(modeacts);
    mMenuSettings->addSeparator();

    mMenuSettings->addAction(mActShowMenubar);
    mMenuSettings->addAction(mActShowToolbar);
    mMenuSettings->addAction(mActShowStatusbar);
    mMenuSettings->addSeparator();
    mMenuSettings->addAction(mActShowTimer);
    mMenuSettings->addAction(mActShowGameNo);
    mMenuSettings->addSeparator();
    mMenuSettings->addAction(mActResetColorsOrder);
    mMenuSettings->addAction(mActShowIndicators);
    mMenuSettings->addSeparator();

    mMenuLevels = mMenuSettings->addMenu(tr("Level Presets"));
    mActGroupLevels = new QActionGroup(mMenuLevels);
    mActGroupLevels->addAction(mActLevelEasy);
    mActGroupLevels->addAction(mActLevelClassic);
    mActGroupLevels->addAction(mActLevelMedium);
    mActGroupLevels->addAction(mActLevelChallenging);
    mActGroupLevels->addAction(mActLevelHard);
    QList<QAction *> levelacts = mActGroupLevels->actions();
    mMenuLevels->addActions(levelacts);
    mMenuSettings->addSeparator();

    mMenuSettings->addAction(mActDoubles);
    mMenuSettings->addSeparator();

    mMenuSettings->addAction(mActAutoClose);
    mMenuSettings->addAction(mActAutoHints);

    mMenuSettings->addSeparator();
    mMenuSettings->addAction(mActPreferences);


    mMenuHelp = mMenuBar->addMenu(tr("&Help"));
    mMenuHelp->addAction(mLaunchHelpAction);
    mMenuHelp->addSeparator();
    mMenuHelp->addAction(mActAbout);
    mMenuHelp->addAction(mActAboutQt);

    mMenuRowContext = new QMenu();
    mMenuRowContext->addAction(mActRandRow);
    mMenuRowContext->addAction(mActPrevRow);
    mMenuRowContext->addAction(mActClearRow);
    mMenuRowContext->addSeparator();
    mMenuRowContext->addAction(mActSetGuess);
    mMenuRowContext->addAction(mActSetHints);
    mMenuRowContext->addSeparator();
    mMenuRowContext->addAction(mActNewGame);
    mMenuRowContext->addAction(mActRestartGame);
    mMenuRowContext->addAction(mActGiveIn);
    mMenuRowContext->addSeparator();
    mMenuRowContext->addAction(mActExit);

    addActions(mMenuBar->actions());
}

void ColorCode::InitToolBars()
{
    mGameToolbar = addToolBar(tr("Game"));
    mGameToolbar->setAllowedAreas(Qt::NoToolBarArea);
    mGameToolbar->setFloatable(false);
    mGameToolbar->setIconSize(QSize(16, 16));
    mGameToolbar->setMovable(false);
    mGameToolbar->addAction(mActNewGame);
    mGameToolbar->addAction(mActRestartGame);
    mGameToolbar->addAction(mActGiveIn);
    mGameToolbar->addSeparator();
    mGameToolbar->addAction(mActSetGuess);
    mGameToolbar->addAction(mActSetHints);

    mColorCntCmb = new QComboBox();
    mColorCntCmb->setLayoutDirection(Qt::LeftToRight);
    mColorCntCmb->setFixedWidth(84);
    mColorCntCmb->addItem("2 " + tr("Colors"), 2);
    mColorCntCmb->addItem("3 " + tr("Colors"), 3);
    mColorCntCmb->addItem("4 " + tr("Colors"), 4);
    mColorCntCmb->addItem("5 " + tr("Colors"), 5);
    mColorCntCmb->addItem("6 " + tr("Colors"), 6);
    mColorCntCmb->addItem("7 " + tr("Colors"), 7);
    mColorCntCmb->addItem("8 " + tr("Colors"), 8);
    mColorCntCmb->addItem("9 " + tr("Colors"), 9);
    mColorCntCmb->addItem("10 " + tr("Colors"), 10);
    mColorCntCmb->setCurrentIndex(6);
    connect(mColorCntCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(ColorCntChangedSlot()));

    mPegCntCmb = new QComboBox();
    mPegCntCmb->setLayoutDirection(Qt::LeftToRight);
    mPegCntCmb->setFixedWidth(76);
    mPegCntCmb->addItem("2 " + tr("Slots"), 2);
    mPegCntCmb->addItem("3 " + tr("Slots"), 3);
    mPegCntCmb->addItem("4 " + tr("Slots"), 4);
    mPegCntCmb->addItem("5 " + tr("Slots"), 5);
    mPegCntCmb->setCurrentIndex(2);
    connect(mPegCntCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(PegCntChangedSlot()));

    mLevelToolbar = addToolBar(tr("Level"));
    mLevelToolbar->setAllowedAreas(Qt::NoToolBarArea);
    mLevelToolbar->setFloatable(false);
    mLevelToolbar->setIconSize(QSize(16, 16));
    mLevelToolbar->setMovable(false);
    mLevelToolbar->setLayoutDirection(Qt::RightToLeft);
    mLevelToolbar->addWidget(mColorCntCmb);
    QWidget* spacer = new QWidget();
    spacer->setMinimumWidth(4);
    mLevelToolbar->addWidget(spacer);
    mLevelToolbar->addWidget(mPegCntCmb);
    QWidget* spacer2 = new QWidget();
    spacer2->setMinimumWidth(4);
    mLevelToolbar->addWidget(spacer2);
    mLevelToolbar->addAction(mActDoublesIcon);
}

void ColorCode::ApplySettingsSlot()
{
    bool restart = NeedsRestart();
    mNoAct = true;

    mActShowToolbar->setChecked(mSettings->mShowToolBar);
    ShowToolbarSlot();
    mActShowMenubar->setChecked(mSettings->mShowMenuBar);
    ShowMenubarSlot();
    mActShowStatusbar->setChecked(mSettings->mShowStatusBar);
    ShowStatusbarSlot();
    mActShowTimer->setChecked(mSettings->mShowTimer);
    ShowTimer();
    mActShowGameNo->setChecked(mSettings->mShowGameNo);
    UpdateGameNoDisplay();
    mActShowIndicators->setChecked(mSettings->mShowIndicators);
    SetIndicatorsSlot();

    mActAutoClose->setChecked(mSettings->mAutoClose);
    mActAutoHints->setChecked(mSettings->mAutoHints);

    SetDoublesSlot(mSettings->mDoubles);
    int i;
    i = mColorCntCmb->findData(mSettings->mColorCnt);
    if (i != -1 && mColorCntCmb->currentIndex() != i)
    {
        mColorCntCmb->setCurrentIndex(i);
    }
    i = mPegCntCmb->findData(mSettings->mPegCnt);
    if (i != -1 && mPegCntCmb->currentIndex() != i)
    {
        mPegCntCmb->setCurrentIndex(i);
    }
    CheckLevel();

    if (mSettings->mGameMode == MODE_HVM)
    {
        mActModeHvM->setChecked(true);
        UpdateGameNoDisplay();
    }
    else
    {
        mActModeMvH->setChecked(true);
    }

    mHintsDelayTimer->setInterval(mSettings->mHintsDelay);

    mNoAct = false;

    if (restart)
    {
        TryNewGame();
    }
}

void ColorCode::TryNewGame()
{
    int r = QMessageBox::Yes;
    if (GamesRunning())
    {
        r = QMessageBox::question( this,
                                   tr("Message"),
                                   tr("The changed settings will only apply to new games!\nDo you want to give in the current and start a new Game?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
    }

    if (r == QMessageBox::Yes)
    {
        mGameInput = NULL;
        NewGame();
    }
}

bool ColorCode::NeedsRestart() const
{
    bool need = false;

    if ( mSettings->mDoubles != mActDoubles->isChecked()
        || mSettings->mColorCnt != mColorCnt
        || mSettings->mPegCnt != mPegCnt
        || mSettings->mGameMode != mGameMode
        || (mGameMode == MODE_MVH && mSolverStrength != mSettings->mSolverStrength) )
    {
        need = true;
    }

    return need;
}

void ColorCode::contextMenuEvent(QContextMenuEvent* e)
{

    QList<QGraphicsItem *> list = mView->items(mView->mapFromGlobal(e->globalPos()));
    int i = 0;
    bool isrow = false;
    if (mGameState == STATE_RUNNING && mCurRow != NULL)
    {
        for (i = 0; i < list.size(); ++i)
        {
            if (list.at(i) == mCurRow || list.at(i) == mHintBtns[mCurRow->GetIx()])
            {
                 isrow = true;
                 break;
            }
        }
    }

    if (isrow)
    {
        UpdateRowMenuSlot();
        mMenuRowContext->exec(e->globalPos());
    }
    else
    {
        mMenuGame->exec(e->globalPos());
    }
}

void ColorCode::resizeEvent (QResizeEvent* e)
{
    Q_UNUSED(e);
    Scale();
}

void ColorCode::keyPressEvent(QKeyEvent *e)
{
    if (mCurRow == NULL || mGameState != STATE_RUNNING)
    {
        return;
    }

    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (mGameMode == MODE_HVM)
            {
                int ix = mCurRow->GetIx();
                if (mHintBtns[ix]->mActive)
                {
                    std::vector<int> s = mCurRow->GetSolution();
                    if (s.size() == (unsigned) mPegCnt)
                    {
                        mHintBtns[ix]->SetActive(false);
                        HintPressedSlot(ix);
                    }
                }
            }
            else if (mGameMode == MODE_MVH)
            {
                if (mDoneBtn->isVisible() || mOkBtn->isVisible())
                {
                    DoneBtnPressSlot(mOkBtn);
                }
            }
        break;
    }
}

void ColorCode::UpdateActions()
{
    if (mGameState == STATE_PAUSED)
    {
        mActSelectGame->setEnabled(true);
        mActSaveGame->setEnabled(true);
        mActSetGuess->setEnabled(false);
        mActRestartGame->setEnabled(false);
        mActGiveIn->setEnabled(false);
        return;
    }

    bool running = (mGameState == STATE_RUNNING && mCurRow != NULL);
    if (mGameMode == MODE_HVM)
    {
        mActSelectGame->setVisible(true);
        mActSelectGame->setEnabled(true);
        mActSaveGame->setVisible(true);
        mActSaveGame->setEnabled(true);
        mActPauseGame->setVisible(true);
        mActShowTimer->setVisible(true);
        mActShowTimer->setEnabled(true);
        mActShowGameNo->setVisible(true);
        mActShowGameNo->setEnabled(true);
        mActCopyGameNo->setVisible(true);
        mActCopyGameNo->setEnabled(true);
        if (running)
        {
            mActSetGuess->setEnabled(true);
            mActRestartGame->setEnabled(true);
            mActGiveIn->setEnabled(true);
            mActPauseGame->setEnabled(true);
        }
        else
        {
            mActSetGuess->setEnabled(false);
            mActRestartGame->setEnabled(false);
            mActGiveIn->setEnabled(false);
            mActPauseGame->setEnabled(false);
        }
    }
    else if (mGameMode == MODE_MVH)
    {
        mActSelectGame->setVisible(false);
        mActSelectGame->setEnabled(false);
        mActSaveGame->setVisible(false);
        mActSaveGame->setEnabled(false);
        mActPauseGame->setVisible(false);
        mActPauseGame->setEnabled(false);
        mActShowTimer->setVisible(false);
        mActShowTimer->setEnabled(false);
        mActShowGameNo->setVisible(false);
        mActShowGameNo->setEnabled(false);
        mActCopyGameNo->setVisible(false);
        mActCopyGameNo->setEnabled(false);
        if (running)
        {
            mActRestartGame->setEnabled(true);
            mActGiveIn->setEnabled(true);
            if (mCurRow == mSolutionRow)
            {
                mActSetHints->setEnabled(false);
            }
            else
            {
                mActSetHints->setEnabled(!mSettings->mAutoHints && !mSolver->isRunning());
            }
            UpdateRowMenuSlot();
        }
        else
        {
            mActSetHints->setEnabled(false);
            mActRestartGame->setEnabled(false);
            mActGiveIn->setEnabled(false);
        }
    }
}

void ColorCode::UpdateRowMenuSlot()
{
    if (mGameMode == MODE_HVM)
    {
        if (mGameState != STATE_RUNNING || mCurRow == NULL)
        {
            mActRandRow->setEnabled(false);
            mActPrevRow->setEnabled(false);
            mActClearRow->setEnabled(false);
            mActSetGuess->setEnabled(false);
            return;
        }
        else
        {
            mActRandRow->setEnabled(true);
        }

        if (mCurRow->GetIx() < 1)
        {
            mActPrevRow->setEnabled(false);
        }
        else
        {
            mActPrevRow->setEnabled(true);
        }

        if (mCurRow->GetPegCnt() == 0)
        {
            mActClearRow->setEnabled(false);
        }
        else
        {
            mActClearRow->setEnabled(true);
        }
    }
    else if (mGameMode == MODE_MVH)
    {
        if (mGameState == STATE_RUNNING)
        {
            if (mCurRow == mSolutionRow)
            {
                mActSetHints->setEnabled(false);
                mActRandRow->setEnabled(true);
                if (mSolutionRow->GetPegCnt() == 0)
                {
                    mActClearRow->setEnabled(false);
                }
                else
                {
                    mActClearRow->setEnabled(true);
                }
            }
            else
            {
                mActSetHints->setEnabled(!mSettings->mAutoHints && !mSolver->isRunning());
                mActRandRow->setEnabled(false);
                mActClearRow->setEnabled(false);
            }
        }
        else
        {
            mActRandRow->setEnabled(false);
            mActClearRow->setEnabled(false);
            mActSetHints->setEnabled(false);
        }
    }
}

void ColorCode::PauseGameSlot(GraphicsBtn*)
{
    if (mCurRow == NULL)
    {
        return;
    }

    bool paused = (mGameState == STATE_PAUSED);
    if (mGameState == STATE_RUNNING)
    {
        mGameTimer->PauseT();
        mActPauseGame->setText(tr("&Resume Game"));
        mActPauseGame->setIcon(QIcon(":/img/resume.png"));
        SetState(STATE_PAUSED);
        UpdateGameNoDisplay();
        ResolveGame();
    }
    else if (paused)
    {
        mGameTimer->ResumeT();
        mActPauseGame->setText(tr("&Have a Break"));
        mActPauseGame->setIcon(QIcon(":/img/pause.png"));
        SetState(STATE_RUNNING);
        UpdateGameNoDisplay();
        RowSolutionSlot(mCurRow->GetIx());
    }
    mPegFactory->SetPaused();
    for (int i = 0; i < mMaxRowCnt; ++i)
    {
        mHintBtns[i]->SetPaused(paused);
    }
    UpdateActions();
}

void ColorCode::RestartGameSlot()
{
    if (mSolver->isRunning())
    {
        WaitForSolver();
    }

    if (mGameMode == MODE_HVM)
    {
        ResetRows();
        SetState(STATE_RUNNING);
        SetCurRow(NULL);
        mSolver->RestartGame();
        NextRow();
    }
    else if (mGameMode == MODE_MVH)
    {
        mHintsDelayTimer->stop();
        mOkBtn->ShowBtn(false);
        ResetRows();

        SetCurRow(NULL);
        mSolver->NewGame(mColorCnt, mPegCnt, mDoubles, mSolverStrength, mMaxRowCnt);
        mSolutionRow->OpenRow();
        GetSolution();
        SetState(STATE_RUNNING);
        RowSolutionSlot(mSolutionRow->GetIx());
    }
}

void ColorCode::NewGameSlot()
{
    if (mGameState == STATE_RUNNING && mCurRow != NULL)
    {
        int r = QMessageBox::warning( this,
                                      tr("New Game"),
                                      tr("Do you want to give in\nand start a new Game?"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (r == QMessageBox::Yes)
        {
            mGameInput = NULL;
            NewGame();
        }
    }
    else
    {
        mGameInput = NULL;
        NewGame();
    }
}

void ColorCode::GiveInSlot()
{
    int r = QMessageBox::Yes;
    if (mGameState == STATE_RUNNING && mCurRow != NULL)
    {
        r = QMessageBox::warning( this,
                                  tr("Give In"),
                                  tr("Do you really want to give in?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    }

    if (r == QMessageBox::Yes)
    {
        SetState(STATE_GAVE_UP);

        if (mSolver->isRunning())
        {
            WaitForSolver();
        }

        if (mCurRow != NULL)
        {
            mCurRow->CloseRow();
            mHintBtns[mCurRow->GetIx()]->SetActive(false);
        }

        ResolveGame();
    }
}

void ColorCode::CopyGameNoSlot()
{
    uint no = 0;
    if (mGameInput != NULL)
    {
        no = mGameInput->mGameNo;
    }
    else if (mFinishedGameInput != NULL)
    {
        no = mFinishedGameInput->mGameNo;
    }

    if (no > 0)
    {
        QClipboard* cb = QApplication::clipboard();
        cb->setText(QString::number(no));
    }
}

void ColorCode::OpenPreferencesSlot()
{
    if (mPrefDialog == NULL)
    {
        CreatePrefDialog();
    }
    if (mPrefDialog == NULL)
    {
        return;
    }

    mSettings->SaveLastSettings();
    mPrefDialog->SetSettings();
    int r = mPrefDialog->exec();

    if (r == QDialog::Accepted)
    {
        ApplySettingsSlot();
    }
    else
    {
        mSettings->RestoreLastSettings();
    }
}

void ColorCode::OnlineHelpSlot()
{
    QDesktopServices::openUrl(QUrl("http://colorcode.laebisch.com/documentation", QUrl::TolerantMode));
}

void ColorCode::AboutSlot()
{
    About ab(this);
    ab.exec();
}

void ColorCode::AboutQtSlot()
{
    QMessageBox::aboutQt( this,
                        tr("About Qt"));
}

void ColorCode::ShowToolbarSlot()
{
    mSettings->mShowToolBar = mActShowToolbar->isChecked();
    if (!mActShowToolbar->isChecked())
    {
        mGameToolbar->hide();
        mLevelToolbar->hide();
    }
    else
    {
        mGameToolbar->show();
        mLevelToolbar->show();
    }
    Scale();
}

void ColorCode::ShowMenubarSlot()
{
    mSettings->mShowMenuBar = mActShowMenubar->isChecked();
    if (!mActShowMenubar->isChecked())
    {
        mMenuBar->hide();
    }
    else
    {
        mMenuBar->show();
    }
    Scale();
}

void ColorCode::ShowStatusbarSlot()
{
    mSettings->mShowStatusBar = mActShowStatusbar->isChecked();
    if (!mActShowStatusbar->isChecked())
    {
        statusBar()->hide();
    }
    else
    {
        statusBar()->show();
    }
    Scale();
}

void ColorCode::ShowTimerSlot()
{
    mSettings->mShowTimer = mActShowTimer->isChecked();
    ShowTimer();
}

void ColorCode::ShowTimer()
{
    if (mGameMode == MODE_HVM)
    {
        mGameTimer->SetTenth(mSettings->mShowTenth);
        mGameTimer->setVisible(mSettings->mShowTimer);
        mGameTimer->update();
        mGameTimerBg->setVisible(mSettings->mShowTimer);
        mTimerBtn->ShowBtn(mSettings->mShowTimer);
    }
    else
    {
        mGameTimer->setVisible(false);
        mGameTimerBg->setVisible(false);
        mTimerBtn->ShowBtn(false);
    }
}

void ColorCode::ShowGameNoSlot()
{
    mSettings->mShowGameNo = mActShowGameNo->isChecked();
    UpdateGameNoDisplay();
}

void ColorCode::UpdateGameNoDisplay()
{
    if (mGameMode == MODE_HVM)
    {
        QString str = tr("Human vs. Computer");
        CCGame* g = GetCurrentGameInput();
        if (mSettings->mShowGameNo && g != NULL && mGameState != STATE_PAUSED)
        {
            str += QString(" :: ") + QString(tr("Game Number")) + QString(": ") + QString::number(g->mGameNo);
        }
        mGameNoDisplay->ShowStr(str);
    }
    else if (mGameMode == MODE_MVH)
    {
        mGameNoDisplay->ShowStr(tr("Computer vs. Human"));
    }
}

CCGame* ColorCode::GetCurrentGameInput() const
{
    if (mGameInput != NULL && mGameInput->IsValidGame())
    {
        return mGameInput;
    }
    else if (mFinishedGameInput != NULL && mFinishedGameInput->IsValidGame())
    {
        return mFinishedGameInput;
    }
    return NULL;
}

void ColorCode::CloseHighScoreDialogSlot()
{
    ;
}

void ColorCode::SetIndicatorsSlot()
{
    mSettings->mShowIndicators = mActShowIndicators->isChecked();
    mPegFactory->SetIndicators();
}

void ColorCode::AutoCloseSlot()
{
    mSettings->mAutoClose = mActAutoClose->isChecked();
}

void ColorCode::AutoHintsSlot()
{
    mSettings->mAutoHints = mActAutoHints->isChecked();

    if (mGameMode == MODE_MVH && mGameState == STATE_RUNNING && mCurRow != NULL && mCurRow != mSolutionRow)
    {
        mActSetHints->setEnabled(!mSettings->mAutoHints);
        if (mSettings->mAutoHints)
        {
            SetHintsSlot();
            SetAutoHintsSlot();
        }
    }
}

void ColorCode::ColorCntChangedSlot()
{
    SetColorCnt();

    if (mNoAct)
    {
        return;
    }

    int r = QMessageBox::Yes;
    if (GamesRunning())
    {
        r = QMessageBox::question( this,
                                   tr("Message"),
                                   tr("The changed settings will only apply to new games!\nDo you want to give in the current and start a new Game?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
    }

    if (r == QMessageBox::Yes)
    {
        mGameInput = NULL;
        NewGame();
    }
}

void ColorCode::PegCntChangedSlot()
{
    SetPegCnt();

    if (mNoAct)
    {
        return;
    }

    int pcnt = GetPegCntInput();
    if (pcnt == mPegCnt)
    {
        return;
    }

    int r = QMessageBox::Yes;
    if (GamesRunning())
    {
        r = QMessageBox::question( this,
                                   tr("Message"),
                                   tr("The changed settings will only apply to new games!\nDo you want to give in the current and start a new Game?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
    }

    if (r == QMessageBox::Yes)
    {
        mGameInput = NULL;
        NewGame();
    }
}

void ColorCode::DoublesChangedSlot(bool checked)
{
    if (mNoAct)
    {
        return;
    }

    SetDoublesSlot(checked);

    if (mDoubles == mSettings->mDoubles)
    {
        return;
    }

    int r = QMessageBox::Yes;
    if (GamesRunning())
    {
        r = QMessageBox::question( this,
                                   tr("Message"),
                                   tr("The changed settings will only apply to new games!\nDo you want to give in the current and start a new Game?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
    }

    if (r == QMessageBox::Yes)
    {
        mGameInput = NULL;
        NewGame();
    }
}

void ColorCode::SetLevelSlot()
{
    mNoAct = true;
    int ix = mActGroupLevels->checkedAction()->data().toInt();

    if (ix < 0 || ix > 4)
    {
        return;
    }

    int i;
    i = mColorCntCmb->findData(LEVEL_SETTINGS[ix][0]);
    if (i != -1 && mColorCntCmb->currentIndex() != i)
    {
        mColorCntCmb->setCurrentIndex(i);
        SetColorCnt();
    }
    i = mPegCntCmb->findData(LEVEL_SETTINGS[ix][1]);
    if (i != -1 && mPegCntCmb->currentIndex() != i)
    {
        mPegCntCmb->setCurrentIndex(i);
        SetPegCnt();
    }

    SetDoublesSlot((LEVEL_SETTINGS[ix][2] == 1));

    int r = QMessageBox::Yes;
    if (GamesRunning())
    {
        r = QMessageBox::question( this,
                                   tr("Message"),
                                   tr("The changed settings will only apply to new games!\nDo you want to give in the current and start a new Game?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
    }

    mNoAct = false;

    if (r == QMessageBox::Yes)
    {
        mGameInput = NULL;
        NewGame();
    }    
}

void ColorCode::SetGameModeSlot()
{
    SetGameMode();

    int r = QMessageBox::Yes;
    if (GamesRunning())
    {
        r = QMessageBox::question( this,
                                   tr("Message"),
                                   tr("The changed settings will only apply to new games!\nDo you want to give in the current and start a new Game?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
    }

    if (r == QMessageBox::Yes)
    {
        mGameInput = NULL;
        NewGame();
    }
}


void ColorCode::SetPegCnt()
{
    mSettings->mPegCnt = GetPegCntInput();
}

void ColorCode::SetColorCnt()
{
    mSettings->mColorCnt = GetColorCntInput();
}

void ColorCode::SetGameMode()
{
    mSettings->mGameMode = GetGameModeInput();
}

void ColorCode::ShowMsgSlot(QString msg)
{
    mMsg->ShowMsg(msg);
}

void ColorCode::SnapPegSlot(ColorPeg* cp)
{
    int i;
    bool snapped = false;

    if (mGameMode == MODE_HVM)
    {
        QList<QGraphicsItem *> list = mScene->items(QPointF(cp->pos().x() + 18, cp->pos().y() + 18));

        if (mSolver->isRunning())
        {
            WaitForSolver();
        }

        for (i = 0; i < list.size(); ++i)
        {
            if (mCurRow != NULL && list.at(i) == mCurRow)
            {
                 snapped = mCurRow->SnapCP(cp);
                 break;
            }
        }
    }
    else if (mGameMode == MODE_MVH)
    {
        snapped = mSolutionRow->SnapCP(cp);
    }

    if (!snapped)
    {
        mPegFactory->RemovePegSlot(cp);
    }
}

void ColorCode::RowSolutionSlot(int ix)
{
    if (mGameMode == MODE_HVM)
    {
        if (ix == SOLUTION_ROW_IX) { return; }

        std::vector<int> s = mPegRows[ix]->GetSolution();
        if (s.size() == (unsigned) mPegCnt)
        {
            if (mActAutoClose->isChecked())
            {
                HintPressedSlot(ix);
            }
            else
            {
                mHintBtns[ix]->SetActive(true);
                ShowMsgSlot(tr("Press the Hint Field or Key Enter if You're done."));
            }
        }
        else
        {
            mHintBtns[ix]->SetActive(false);
            ShowMsgSlot(tr("Place Your pegs ..."));
        }
    }
    else if (mGameMode == MODE_MVH)
    {
        if (ix == SOLUTION_ROW_IX)
        {
            std::vector<int> s = mSolutionRow->GetSolution();
            if (s.size() == (unsigned) mPegCnt)
            {
                bool valid = true;
                if (!mActDoubles->isChecked())
                {                    
                    int i;
                    int check[mColorCnt];
                    for (i = 0; i < mColorCnt; ++i)
                    {
                        check[i] = 0;
                    }

                    for (i = 0; (unsigned)i < s.size(); ++i)
                    {
                        if (s[i] >= mColorCnt)
                        {
                            valid = false;
                            break;
                        }

                        if (check[s[i]] != 0)
                        {
                            valid = false;
                            break;
                        }

                        check[s[i]] = 1;
                    }
                }

                if (valid)
                {
                    mDoneBtn->ShowBtn(true);
                    ShowMsgSlot(tr("Press the button below or Key Enter if You're done."));
                }
                else
                {
                    ShowMsgSlot(tr("The chosen settings do not allow pegs of the same color!"));
                }
            }
            else
            {
                mDoneBtn->ShowBtn(false);
                ShowMsgSlot(tr("Place Your secret ColorCode ..."));
            }
        }
    }
}

void ColorCode::HintPressedSlot(int)
{
    mCurRow->CloseRow();
    ResolveRow();
    NextRow();
    ResolveGame();
}

void ColorCode::RandRowSlot()
{
    if (mCurRow == NULL || !mCurRow->IsActive() || mGameState != STATE_RUNNING)
    {
        return;
    }

    if (mGameMode == MODE_HVM || (mGameMode == MODE_MVH && mCurRow == mSolutionRow))
    {
        mCurRow->ClearRow();

        int i, rndm;
        int check[mColorCnt];
        ColorPeg* peg;
        for (i = 0; i < mColorCnt; ++i)
        {
            check[i] = 0;
        }

        for (i = 0; i < mPegCnt; ++i)
        {
            rndm = qrand() % mColorCnt;
            if (mDoubles == 0 && check[rndm] != 0)
            {
                --i;
                continue;
            }

            check[rndm] = 1;

            peg = mPegFactory->CreatePeg(rndm);
            mCurRow->ForceSnap(peg, i);
        }
    }
}

void ColorCode::SetSolverGuessSlot()
{
    if (mIgnoreGuess)
    {
        mIgnoreGuess = false;
        return;
    }

    if (mGameState != STATE_RUNNING || mCurRow == NULL)
    {
        return;
    }

    int* row = mSolver->GuessOut();
    if (row == NULL)
    {
        SetState(STATE_ERROR);
    }
    else
    {
        ColorPeg* peg;
        int i;
        for (i = 0; i < mPegCnt; ++i)
        {
            peg = mPegFactory->CreatePeg(row[i]);
            mCurRow->ForceSnap(peg, i);
        }

        if (mGameMode == MODE_HVM)
        {
            return;
        }
    }

    if (mGameState == STATE_RUNNING)
    {
        mCurRow->CloseRow();
        mHintBtns[mCurRow->GetIx()]->SetActive(true);

        if (mActAutoHints->isChecked())
        {
            if (mOkBtn->isVisible())
            {
                mOkBtn->ShowBtn(false);
            }
            ShowMsgSlot(tr("guessed and rated ..."));
            SetHintsSlot();
            mHintBtns[mCurRow->GetIx()]->SetActive(false);

            std::vector<int> rowhints = mHintBtns[mCurRow->GetIx()]->GetHints();
            int b = 0;
            for (unsigned i = 0; i < rowhints.size(); ++i)
            {
                if (rowhints.at(i) == 2)
                {
                    ++b;
                }
            }

            if (b == mPegCnt)
            {
                SetState(STATE_WON);
                ResolveGame();
            }
            else
            {
                mHintsDelayTimer->start();
            }
        }
        else
        {
            ShowMsgSlot(tr("Please rate the guess. Press OK or Key Enter if You're done."));
            mOkBtn->setPos(5, mCurRow->pos().y() - 39);
            mOkBtn->ShowBtn(true);
        }
    }
    else
    {
        mCurRow->SetActive(false);
        ResolveGame();
    }
    UpdateActions();
}

void ColorCode::SetGuessSlot()
{
    if (mCurRow == NULL || mCurRow == mSolutionRow || mGameState != STATE_RUNNING || mSolver->isRunning())
    {
        return;
    }

    mCurRow->ClearRow();
    ShowMsgSlot(tr("Let me think ..."));
    mSolver->StartGuess();
    UpdateActions();
}

void ColorCode::SetHintsSlot()
{
    if (mCurRow == NULL || mGameState != STATE_RUNNING)
    {
        return;
    }

    if (mGameMode == MODE_MVH)
    {
        if (mCurRow == mSolutionRow || !mHintBtns[mCurRow->GetIx()]->IsActive())
        {
            return;
        }

        std::vector<int> hints = RateSol2Guess(mSolution, mCurRow->GetSolution());
        mHintBtns[mCurRow->GetIx()]->DrawHints(hints);
    }
}

void ColorCode::PrevRowSlot()
{
    if (mCurRow == NULL || mGameState != STATE_RUNNING)
    {
        return;
    }

    if (mCurRow->GetIx() < 1)
    {
        return;
    }

    mCurRow->ClearRow();

    ColorPeg* peg;
    std::vector<int> prev = mPegRows[mCurRow->GetIx() - 1]->GetSolution();
    int i;
    for (i = 0; i < mPegCnt; ++i)
    {
        peg = mPegFactory->CreatePeg(prev.at(i));
        mCurRow->ForceSnap(peg, i);
    }
}

void ColorCode::ClearRowSlot()
{
    if (mCurRow == NULL || mGameState != STATE_RUNNING)
    {
        return;
    }

    mCurRow->ClearRow();
}

void ColorCode::DoneBtnPressSlot(GraphicsBtn*)
{
    mDoneBtn->ShowBtn(false);
    mOkBtn->ShowBtn(false);

    if (mCurRow == mSolutionRow)
    {
        SetSolution();
        NextRow();
    }
    else
    {
        ResolveHints();
    }
}

void ColorCode::SetAutoHintsSlot()
{
    DoneBtnPressSlot(mOkBtn);
}

void ColorCode::TestSlot()
{
    ;
}

void ColorCode::ApplyPreferencesSlot()
{
    ;
}

void ColorCode::CreatePrefDialog()
{
    mPrefDialog = new PrefDialog(this);
    mPrefDialog->setModal(true);
    mPrefDialog->InitSettings();
    connect(mPrefDialog, SIGNAL(accepted()), this, SLOT(ApplyPreferencesSlot()));
    connect(mPrefDialog, SIGNAL(accepted()), mGameTablesDialog, SLOT(SetSettingsSlot()));
    connect(mPrefDialog, SIGNAL(ResetColorOrderSignal()), mPegFactory, SLOT(ResetColorsOrderSlot()));
}

void ColorCode::CheckDoublesSetting()
{
    if (mColorCnt < mPegCnt)
    {
        if (mActDoubles->isEnabled())
        {
            mActDoubles->setEnabled(false);
        }
        if (mActDoublesIcon->isEnabled())
        {
            mActDoublesIcon->setEnabled(false);
        }
        if (!mActDoubles->isChecked())
        {
            mActDoubles->setChecked(true);
        }
        if (!mActDoublesIcon->isChecked())
        {
            mActDoublesIcon->setChecked(true);
        }
    }
    else
    {
        if (!mActDoubles->isEnabled())
        {
            mActDoubles->setEnabled(true);
        }
        if (!mActDoublesIcon->isEnabled())
        {
            mActDoublesIcon->setEnabled(true);
        }
    }

    if (mActDoublesIcon->isChecked())
    {
        mActDoublesIcon->setIcon(QIcon(":/img/same_color_1.png"));
        mActDoublesIcon->setToolTip(tr("Disallow Pegs of the Same Color"));
    }
    else
    {
        mActDoublesIcon->setIcon(QIcon(":/img/same_color_0.png"));
        mActDoublesIcon->setToolTip(tr("Allow Pegs of the Same Color"));
    }
}

void ColorCode::SetDoublesSlot(bool checked)
{
    mSettings->mDoubles = checked;

    mActDoubles->setChecked(checked);
    mActDoublesIcon->setChecked(checked);

    if (checked)
    {
        mActDoublesIcon->setIcon(QIcon(":/img/same_color_1.png"));
        mActDoublesIcon->setToolTip(tr("Disallow Pegs of the Same Color"));
    }
    else
    {
        mActDoublesIcon->setIcon(QIcon(":/img/same_color_0.png"));
        mActDoublesIcon->setToolTip(tr("Allow Pegs of the Same Color"));
    }
}

void ColorCode::CheckLevel()
{
    int ix = -1;
    for (int i = 0; i < 5; ++i)
    {
        if ( LEVEL_SETTINGS[i][0] == mColorCnt
             && LEVEL_SETTINGS[i][1] == mPegCnt
             && LEVEL_SETTINGS[i][2] == mDoubles )
        {
            ix = i;
            break;
        }
    }

    if (ix > -1)
    {
        QList<QAction *> levelacts = mActGroupLevels->actions();
        levelacts.at(ix)->setChecked(true);
    }
    else
    {
        QAction* act = mActGroupLevels->checkedAction();
        if (act != NULL)
        {
            act->setChecked(false);
        }
    }
}

void ColorCode::ResetGame()
{
    mDoneBtn->ShowBtn(false);
    mOkBtn->ShowBtn(false);

    ApplyPegCnt();

    mSolutionRow->Reset(mPegCnt, mGameMode);
    ResetRows();

    mGameId = qrand();
    mGuessCnt = 0;
    ++mGameCnt;

    mPegFactory->ApplyColorCnt();
    
    CheckDoublesSetting();

    mDoubles = GetDoublesInput();
    CheckLevel();

    ApplySolverStrength();
}

void ColorCode::ResetRows()
{
    for (int i = 0; i < mMaxRowCnt; ++i)
    {
        mPegRows[i]->Reset(mPegCnt, mGameMode);
        mHintBtns[i]->Reset(mPegCnt, mGameMode);
    }
}

void ColorCode::NewGameInputSlot(CCGame* g)
{
    if (g != NULL && g->IsValidGame())
    {
        mGameInput = g;
        mGameInput->Anonymize();

        mNoAct = true;
        if (mGameInput->mGameMode == MODE_HVM)
        {
            mActModeHvM->setChecked(true);
        }
        else
        {
            mActModeMvH->setChecked(true);
        }

        SetDoublesSlot((bool) mGameInput->mDoubles);

        int i;
        i = mColorCntCmb->findData(mGameInput->mColorCnt);
        if (i != -1 && mColorCntCmb->currentIndex() != i)
        {
            mColorCntCmb->setCurrentIndex(i);
        }
        i = mPegCntCmb->findData(mGameInput->mPegCnt);
        if (i != -1 && mPegCntCmb->currentIndex() != i)
        {
            mPegCntCmb->setCurrentIndex(i);
        }
        CheckLevel();

        mNoAct = false;
    }

    NewGame();
}

CCGame* ColorCode::GetRandGameInput()
{
    int gameno = mSolver->GetRandGameNo(mColorCnt, mPegCnt, mDoubles);
    CCGame* g = mSettings->GetCurSettingsGame();
    g->mGameNo = gameno;

    return g;
}

void ColorCode::NewGame()
{
    if (mGameState == STATE_PAUSED)
    {
        PauseGameSlot();
    }

    mHintsDelayTimer->stop();

    if (mSolver->isRunning())
    {
        WaitForSolver();
    }

    ApplyGameMode();
    ResetGame();
    QString doubles = (mDoubles == 1) ? tr("Yes") : tr("No");
    QString colors = QString::number(mColorCnt, 10);
    QString pegs = QString::number(mPegCnt, 10);
    mStatusLabel->setText(tr("Pegs of Same Color") + ": <b>" + doubles + "</b> :: " + tr("Slots") + ": <b>" + pegs + "</b> :: " + tr("Colors") + ": <b>" + colors + "</b> ");
    SetState(STATE_RUNNING);

    SetCurRow(NULL);

    if (mGameMode == MODE_HVM)
    {
        mFinishedGameInput = NULL;
        mSolver->NewGame(mColorCnt, mPegCnt, mDoubles, CCSolver::STRENGTH_HIGH, mMaxRowCnt);
        mHighScore->NewGame(mSolver->CalcPosCnt(mColorCnt, mPegCnt, mDoubles));
        if (mGameInput == NULL)
        {
            mGameInput = mSettings->GetCurSettingsGame();
        }

        if (mGameInput->mColorCnt != mColorCnt)
        {
            mGameInput->mColorCnt = mColorCnt;
        }
        if (mGameInput->mPegCnt != mPegCnt)
        {
            mGameInput->mPegCnt = mPegCnt;
        }
        if (mGameInput->mDoubles != mDoubles)
        {
            mGameInput->mDoubles = mDoubles;
        }
        if (mGameInput->mGameMode != mGameMode)
        {
            mGameInput->mGameMode = mGameMode;
        }

        mGameInput->mDuration = 0;
        mGameInput->mTime = QDateTime::currentDateTime().toTime_t();

        if (mGameInput->mGameNo == 0)
        {
            mGameInput->mGameNo = mSolver->GetRandGameNo(mColorCnt, mPegCnt, mDoubles);
        }

        mGameTablesDialog->InsertPrevGameSlot(mGameInput);

        SetSolution();

        mGameTimer->StartT();

        NextRow();
    }
    else if (mGameMode == MODE_MVH)
    {
        mSolver->NewGame(mColorCnt, mPegCnt, mDoubles, mSolverStrength, mMaxRowCnt);
        GetSolution();
    }

    UpdateGameNoDisplay();
}

void ColorCode::NextRow()
{
    if (mGameState != STATE_RUNNING)
    {
        return;
    }

    if (mCurRow == NULL)
    {
        SetCurRow(mPegRows[0]);
    }
    else if (mCurRow->GetIx() < mMaxRowCnt - 1)
    {
        SetCurRow(mPegRows[mCurRow->GetIx() + 1]);
    }
    else
    {
        SetCurRow(NULL);
        SetState(STATE_LOST);
    }

    if (mCurRow != NULL)
    {
        ++mGuessCnt;
        mCurRow->SetActive(true);
        if (mGameMode == MODE_HVM)
        {
            ++mHighScore->mRowsCnt;
            ShowMsgSlot(tr("Place Your pegs ..."));
        }
        else if (mGameMode == MODE_MVH)
        {
            SetGuessSlot();
        }
    }
}

void ColorCode::SetCurRow(PegRow* const &row)
{
    mCurRow = row;
    UpdateActions();
}

void ColorCode::ResolveRow()
{
    std::vector<int> rowsol = mCurRow->GetSolution();
    mSolver->GuessIn(&rowsol);

    std::vector<int> hints = RateSol2Guess(mSolution, rowsol);

    if (hints.size() == (unsigned) mPegCnt)
    {
        int bl = 0;
        for (int i = 0; i < mPegCnt; ++i)
        {
            if (hints.at(i) == 2)
            {
                ++bl;
            }
        }

        if (bl == mPegCnt)
        {
            SetState(STATE_WON);
        }
    }

    mSolver->ResIn(&hints);
    mHintBtns[mCurRow->GetIx()]->DrawHints(hints);
}

std::vector<int> ColorCode::RateSol2Guess(const std::vector<int> sol, const std::vector<int> guess)
{
    std::vector<int> hints;
    std::vector<int> left1;
    std::vector<int> left0;

    int i, p0, p1;
    for (i = 0; i < mPegCnt; ++i)
    {
        p0 = guess.at(i);
        p1 = sol.at(i);
        if (p0 == p1)
        {
            hints.push_back(2);
        }
        else
        {
            left0.push_back(p0);
            left1.push_back(p1);
        }
    }

    if (hints.size() < (unsigned) mPegCnt)
    {
        int len0 = left0.size();
        for (i = 0; i < len0; ++i)
        {
            p0 = left0.at(i);
            for (unsigned j = 0; j < left1.size(); ++j)
            {
                p1 = left1.at(j);
                if (p0 == p1)
                {
                    hints.push_back(1);
                    left1.erase(left1.begin() + j);
                    break;
                }
            }
        }
    }

    return hints;
}

void ColorCode::ResolveHints()
{
    if (mCurRow == NULL)
    {
        return;
    }

    mHintBtns[mCurRow->GetIx()]->SetActive(false);

    std::vector<int> rowsol = mCurRow->GetSolution();
    mSolver->GuessIn(&rowsol);
    std::vector<int> rowhints = mHintBtns[mCurRow->GetIx()]->GetHints();
    int b = 0;
    int w = 0;
    for (unsigned i = 0; i < rowhints.size(); ++i)
    {
        if (rowhints.at(i) == 2)
        {
            ++b;
        }
        else if (rowhints.at(i) == 1)
        {
            ++w;
        }
    }

    if (b == mPegCnt)
    {
        SetState(STATE_WON);
        ResolveGame();
    }
    else if (b == mPegCnt - 1 && w == 1)
    {
        ;
    }
    else
    {
        mSolver->ResIn(&rowhints);
        NextRow();
        ResolveGame();
    }
}

void ColorCode::ResolveGame()
{
    if (mGameMode == MODE_HVM)
    {
        switch (mGameState)
        {
            case STATE_WON:
            {
                ShowMsgSlot(tr("Congratulation! You have won!"));
                mHighScore->mTime = mGameTimer->GetGameTime();
                mGameInput->mScore = mHighScore->GetScore();
                mGameInput->mDuration = mGameTimer->GetGameTime();
                CCGame* g = new CCGame(mGameInput->ToString());
                emit NewHighScoreSignal(g);
            }
            break;
            case STATE_LOST:
                ShowMsgSlot(tr("Sorry! You lost!"));
            break;
            case STATE_GAVE_UP:
                ShowMsgSlot(tr("Sure, You're too weak for me!"));
            break;
            case STATE_ERROR:
                ShowMsgSlot(tr("The impossible happened, sorry."));
            break;
            case STATE_PAUSED:
                ShowMsgSlot(tr("Paused"));
                return;
            break;
            case STATE_RUNNING:
            default:
                return;
            break;
        }

        mFinishedGameInput = new CCGame(mGameInput->ToString());
        mGameInput = NULL;
        mGameTimer->StopT();
        ShowSolution();
    }
    else if (mGameMode == MODE_MVH)
    {
        switch (mGameState)
        {
            case STATE_WON:
                ShowMsgSlot(tr("Yeah! I guessed it, man!"));
            break;
            case STATE_LOST:
                ShowMsgSlot(tr("Embarrassing! I lost a game!"));
            break;
            case STATE_GAVE_UP:
                ShowMsgSlot(tr("Don't you like to see me winning? ;-)"));
            break;
            case STATE_ERROR:
                ShowMsgSlot(tr("Nope! Thats impossible! Did you gave me false hints?"));
            break;
            case STATE_RUNNING:
            default:
                return;
            break;
        }

        mDoneBtn->ShowBtn(false);
        mOkBtn->ShowBtn(false);
    }

    SetCurRow(NULL);
}

void ColorCode::ApplyGameMode()
{
    mGameMode = GetGameModeInput();

    if (mGameMode == MODE_HVM)
    {
        mActSetHints->setVisible(false);

        mActSetGuess->setVisible(true);
        mActPrevRow->setVisible(true);
    }
    else if (mGameMode == MODE_MVH)
    {
        mActSetHints->setVisible(true);

        mActSetGuess->setVisible(false);
        mActPrevRow->setVisible(false);
    }
    ShowTimer();
}

int ColorCode::GetGameState() const
{
    return mGameState;
}

int ColorCode::GetGameModeInput() const
{
    int ix = mActGroupModes->checkedAction()->data().toInt();
    if (ix != MODE_HVM && ix != MODE_MVH)
    {
        ix = MODE_HVM;
    }
    return ix;
}

int ColorCode::GetDoublesInput() const
{
    return (int) mActDoubles->isChecked();
}

int ColorCode::GetColorCntInput() const
{
    int ccnt = mColorCntCmb->itemData(mColorCntCmb->currentIndex(), Qt::UserRole).toInt();
    ccnt = max(2, min(10, ccnt));
    return ccnt;
}

int ColorCode::GetPegCntInput() const
{
    int pegcnt = mPegCntCmb->itemData(mPegCntCmb->currentIndex(), Qt::UserRole).toInt();
    pegcnt = max(2, min(5, pegcnt));
    return pegcnt;
}

void ColorCode::ApplyPegCnt()
{
    mPegCnt = GetPegCntInput();
    mXOffs = 160 - mPegCnt * 20;
}

void ColorCode::ApplySolverStrength()
{
    mSolverStrength = mSettings->mSolverStrength;
}

void ColorCode::SetSolution()
{
    mSolution.clear();

    if (mGameMode == MODE_HVM)
    {
        mSolutionRow->ClearRow();
        mSolution = mSolver->GetSolutionByNo(mGameInput->mGameNo);
    }
    else if (mGameMode == MODE_MVH)
    {
        std::vector<int> s = mSolutionRow->GetSolution();
        if (s.size() == (unsigned) mPegCnt)
        {
            mSolutionRow->CloseRow();
            mDoneBtn->ShowBtn(false);

            for (int i = 0; i < mPegCnt; ++i)
            {
                mSolution.push_back(s.at(i));
            }
        }
    }
}

void ColorCode::GetSolution()
{
    ShowMsgSlot(tr("Place Your secret ColorCode ..."));
    mSolutionRow->SetActive(true);
    SetCurRow(mSolutionRow);
}

void ColorCode::ShowSolution()
{
    mSolutionRow->SetActive(true);
    ColorPeg* peg;
    for (int i = 0; i < mPegCnt; ++i)
    {
        peg = mPegFactory->CreatePeg(mSolution.at(i));
        peg->SetBtn(false);
        mSolutionRow->ForceSnap(peg, i);
    }
    mSolutionRow->CloseRow();
}

void ColorCode::SetState(const int s)
{
    if (mGameState != s)
    {
        mGameState = s;
    }
}

bool ColorCode::GamesRunning()
{
    if (mGameMode == MODE_HVM)
    {
        return ((mGameState == STATE_RUNNING || mGameState == STATE_PAUSED) && mGuessCnt > 1);
    }
    else if (mGameMode == MODE_MVH)
    {
        return (mGameState == STATE_RUNNING && mCurRow != mSolutionRow);
    }
    return false;
}

void ColorCode::Scale()
{
    if (mGameCnt == 0)
    {
        return;
    }

    qreal w = geometry().width();
    qreal h = geometry().height();

    if (mActShowStatusbar->isChecked())
    {
        h -= statusBar()->height();
    }
    if (mActShowMenubar->isChecked())
    {
        h -= mMenuBar->height();
    }
    if (mActShowToolbar->isChecked())
    {
        h -= mGameToolbar->height();
    }

    if (w < 50 || h < 50)
    {
        return;
    }

    if (mOrigSize == NULL)
    {
        mOrigSize = new QSize(320, 560);
        mScene->setSceneRect(QRectF(0, 0, 320, 560));
    }
    else
    {
        qreal sc = min(w / mOrigSize->width(), h / mOrigSize->height());
        mView->resetTransform();
        mView->scale(sc, sc);
    }
}

CCSolver* ColorCode::GetCCSolver() const
{
    return mSolver;
}

void ColorCode::WaitForSolver()
{
    mIgnoreGuess = true;
    mSolver->Interrupt();
    mSolver->wait();
}

bool ColorCode::ForceRepaint()
{
    mView->viewport()->repaint();
    QApplication::processEvents();
    return true;
}

void ColorCode::SavePrevGameSlot()
{
    CCGame* g = GetCurrentGameInput();
    if (g != NULL)
    {
        mGameTablesDialog->InsertSavedGameSlot(g);
    }
}
