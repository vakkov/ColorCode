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

#ifndef COLORCODE_H
#define COLORCODE_H

#include <iostream>
#include <QMainWindow>

struct PegType
{
    int ix;
    char let;
    QRadialGradient* grad;
    QColor pencolor;
};

class QAction;
class QActionGroup;
class QComboBox;
class QLabel;
class QGraphicsItem;
class QGraphicsTextItem;
class QGraphicsScene;
class QGraphicsView;
class PrefDialog;
class Settings;
class CCSolver;
class ColorPeg;
class PegRow;
class RowHint;
class Msg;
class BackGround;
class SolutionRow;
class GraphicsBtn;
class PegFactory;
class CCGame;
class GameNoDisplay;
class TimeDisplay;
class TimeDisplayBg;
class HighScore;
class GameTablesDialog;

class ColorCode : public QMainWindow
{
    Q_OBJECT

    public:
        ColorCode();
        ~ColorCode();

        static ColorCode* INSTANCE;

        static const QString VERSION;
        static QString GAME_TITLE;

        static const int STATE_NONE;
        static const int STATE_RUNNING;
        static const int STATE_PAUSED;
        static const int STATE_WON;
        static const int STATE_LOST;
        static const int STATE_GAVE_UP;
        static const int STATE_ERROR;

        static const int LAYER_BG;
        static const int LAYER_ROWS;
        static const int LAYER_HINTS;
        static const int LAYER_SOL;
        static const int LAYER_MSG;
        static const int LAYER_PEGS;
        static const int LAYER_BTNS;
        static const int LAYER_DRAG;

        static const int MODE_HVM;
        static const int MODE_MVH;

        static const int MAX_COLOR_CNT;
        static const int MIN_COLOR_CNT;
        static const int SOLUTION_ROW_IX;

        static const int LEVEL_SETTINGS[5][3];

        static int mMaxRowCnt;
        static int mRowY0;
        static int mColorCnt;

        Settings* mSettings;
        GameTablesDialog* mGameTablesDialog;
        QGraphicsScene* mScene;
        QGraphicsView* mView;

        int GetGameState() const;
        int GetGameModeInput() const;
        int GetDoublesInput() const;
        int GetColorCntInput() const;
        int GetPegCntInput() const;
        CCSolver* GetCCSolver() const;

    public slots:
        void SnapPegSlot(ColorPeg *cp);
        void RowSolutionSlot(int ix);
        void HintPressedSlot(int ix);
        void ShowMsgSlot(QString msg);

        void DoneBtnPressSlot(GraphicsBtn* btn = NULL);
        void SetSolverGuessSlot();

        void ApplyPreferencesSlot();

    signals:
        void NewHighScoreSignal(CCGame* g);

    protected :
        void resizeEvent(QResizeEvent* e);
        void contextMenuEvent(QContextMenuEvent* e);
        void keyPressEvent(QKeyEvent *e);

    private slots:
        void InitRows();
        void NewGameSlot();
        void NewGameInputSlot(CCGame* g);
        void SavePrevGameSlot();
        void NewGame();
        void RestartGameSlot();
        void GiveInSlot();
        void PauseGameSlot(GraphicsBtn* = NULL);
        void ResetRows();
        void OnlineHelpSlot();
        void AboutSlot();
        void AboutQtSlot();
        void OpenPreferencesSlot();
        void ApplySettingsSlot();
        void ShowToolbarSlot();
        void ShowMenubarSlot();
        void ShowStatusbarSlot();
        void ShowTimerSlot();
        void ShowGameNoSlot();
        void CloseHighScoreDialogSlot();
        void SetIndicatorsSlot();
        void DoublesChangedSlot(bool checked);
        void SetDoublesSlot(bool checked);
        void AutoCloseSlot();
        void AutoHintsSlot();
        void SetLevelSlot();
        void SetGameModeSlot();
        void CopyGameNoSlot();

        void RandRowSlot();
        void PrevRowSlot();
        void ClearRowSlot();

        void ColorCntChangedSlot();
        void PegCntChangedSlot();

        void UpdateRowMenuSlot();

        void SetGuessSlot();
        void SetHintsSlot();
        void SetAutoHintsSlot();

        void TestSlot();

    private:
        static int mGameMode;
        static int mLevel;
        static int mPegCnt;
        static int mDoubles;
        static int mMaxZ;
        static int mXOffs;

        volatile static bool mNoAct;
        volatile static bool mIgnoreGuess;

        int mGameState;
        int mGameId;
        int mGuessCnt;
        int mHintsCnt;
        int mGameCnt;
        int mSolverStrength;

        std::vector<int> mSolution;

        QSize* mOrigSize;

        CCSolver* mSolver;
        PegFactory* mPegFactory;
        Msg* mMsg;
        BackGround* mBg;
        GraphicsBtn* mDoneBtn;
        GraphicsBtn* mOkBtn;
        GraphicsBtn* mTimerBtn;
        GraphicsBtn* mSolutionRowBg;
        PrefDialog* mPrefDialog;

        PegRow* mCurRow;
        SolutionRow* mSolutionRow;
        PegRow* mPegRows[10];
        RowHint* mHintBtns[10];

        QTimer* mHintsDelayTimer;
        TimeDisplay* mGameTimer;
        TimeDisplayBg* mGameTimerBg;
        GameNoDisplay* mGameNoDisplay;

        CCGame* mGameInput;
        CCGame* mFinishedGameInput;
        HighScore* mHighScore;

        void Init();
        void InitSolution();
        void InitActions();
        void InitMenus();
        void InitToolBars();
        void CreatePrefDialog();
        void ShowTimer();
        void UpdateGameNoDisplay();
        CCGame* GetCurrentGameInput() const;
        bool NeedsRestart() const;
        void TryNewGame();

        void SetGameMode();
        void UpdateActions();
        void SetPegCnt();
        void SetColorCnt();

        void ApplyGameMode();
        void ApplyPegCnt();
        void ApplySolverStrength();
        
        void CheckLevel();
        void CheckDoublesSetting();
        void SetState(const int s);
        void ResetGame();
        void SetSolution();
        void GetSolution();
        void ShowSolution();
        void NextRow();
        void SetCurRow(PegRow* const &row);
        void ResolveRow();
        std::vector<int> RateSol2Guess(const std::vector<int> sol, const std::vector<int> guess);
        void ResolveHints();
        void ResolveGame();
        void WaitForSolver();
        bool ForceRepaint();
        bool GamesRunning();
        void Scale();

        CCGame* GetRandGameInput();

        QMenu* mMenuGame;
        QMenu* mMenuRow;
        QMenu* mMenuRowContext;
        QMenu* mMenuSettings;
        QMenu* mMenuModes;
        QMenu* mMenuLevels;
        QMenu* mMenuGames;
        QMenu* mMenuHelp;        

        QMenuBar* mMenuBar;
        QToolBar* mGameToolbar;
        QToolBar* mLevelToolbar;

        QComboBox* mColorCntCmb;
        QComboBox* mPegCntCmb;
        QLabel* mStatusLabel;

        QActionGroup* mActGroupLevels;
        QActionGroup* mActGroupModes;

        QAction* mActNewGame;
        QAction* mActSelectGame;
        QAction* mActCopyGameNo;
        QAction* mActRestartGame;
        QAction* mActGiveIn;
        QAction* mActSaveGame;
        QAction* mActPauseGame;
        QAction* mActShowHighScores;
        QAction* mActShowPrevGames;
        QAction* mActShowSavedGames;
        QAction* mActExit;
        QAction* mActShowToolbar;
        QAction* mActShowMenubar;
        QAction* mActShowStatusbar;
        QAction* mActShowTimer;
        QAction* mActShowGameNo;
        QAction* mActDoubles;
        QAction* mActResetColorsOrder;
        QAction* mActShowIndicators;
        QAction* mActDoublesIcon;
        QAction* mActAutoClose;
        QAction* mActAutoHints;
        QAction* mActPreferences;
        QAction* mActAbout;
        QAction* mActAboutQt;

        QAction* mActSetPegCnt;

        QAction* mActModeHvM;
        QAction* mActModeMvH;

        QAction* mActLevelEasy;
        QAction* mActLevelClassic;
        QAction* mActLevelMedium;
        QAction* mActLevelChallenging;
        QAction* mActLevelHard;
        QAction* mLaunchHelpAction;

        QAction* mActRandRow;
        QAction* mActPrevRow;
        QAction* mActClearRow;

        QAction* mActSetGuess;
        QAction* mActSetHints;
};

#endif // COLORCODE_H
