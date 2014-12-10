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

#include "settings.h"
#include "colorcode.h"
#include "ccsolver.h"
#include "prevgamesmodel.h"
#include "savedgamesmodel.h"
#include "highscoresmodel.h"

using namespace std;

Settings* GetSettings()
{
    static Settings* s = new Settings();
    return s;
}

const int Settings::INDICATOR_LETTER    = 0;
const int Settings::INDICATOR_NUMBER    = 1;

const int Settings::DEF_COLOR_CNT       = 8;
const int Settings::DEF_PEG_CNT         = 4;
const int Settings::DEF_DOUBLES         = 1;
const int Settings::DEF_LIST_MAX_CNT    = 15;

const int Settings::HIGHSCORE_NO        = 0;
const int Settings::HIGHSCORE_PROMPT    = 1;
const int Settings::HIGHSCORE_AUTO      = 2;

const int Settings::MAX_LENGTH_USERNAME = 20;


Settings::Settings()
{

}

void Settings::Init()
{
    mSettings.setDefaultFormat(QSettings::IniFormat);
    mSettings.setFallbacksEnabled(false);

    InitSettings();
    SaveSettingsMap(mDefMap);
    ReadSettings();
    SaveSettingsMap(mLastMap);

    emit ReadSettingsGamesDone();
}

Settings::~Settings()
{
}

void Settings::InitSettings()
{
    mShowToolBar   = true;
    mShowMenuBar   = true;
    mShowStatusBar = false;
    mShowTimer     = true;
    mShowTenth     = true;
    mShowGameNo    = true;

    mShowIndicators = false;
    mIndicatorType = INDICATOR_LETTER;

    mHideColors = false;

    mPegCnt = DEF_PEG_CNT;
    mColorCnt = DEF_COLOR_CNT;
    mDoubles = (bool)DEF_DOUBLES;
    mGameMode = ColorCode::MODE_HVM;
    mAutoClose = false;
    mAutoHints = false;
    mHintsDelay = 500;

    mSolverStrength = CCSolver::STRENGTH_HIGH;

    mPrevGamesStr = "";
    mSavedGamesStr = "";
    mHighGamesStr = "";

    mGamesListMaxCnt = DEF_LIST_MAX_CNT;
    mCustomRowHColor = false;
    mRowHColorFg = QApplication::palette().color(QPalette::Active, QPalette::ButtonText).name();
    mRowHColorBg = QApplication::palette().color(QPalette::Active, QPalette::Button).name();
    mHighScoreHandling = HIGHSCORE_PROMPT;
    mHighScoreUserName = "";
}

QMap<QString, QVariant> Settings::GetDefaultsMap() const
{
    return mDefMap;
}

void Settings::SaveLastSettings()
{
    SaveSettingsMap(mLastMap);
}

void Settings::RestoreLastSettings()
{
    RestoreSettingsMap(mLastMap);
}

void Settings::RestoreDefSettings()
{
    RestoreSettingsMap(mDefMap);
}

void Settings::RestoreSettingsMap(QMap<QString, QVariant> &map)
{
    mShowToolBar = map["ShowToolBar"].toBool();
    mShowMenuBar = map["ShowMenuBar"].toBool();
    mShowStatusBar = map["ShowStatusBar"].toBool();
    mShowTimer = map["ShowTimer"].toBool();
    mShowTenth = map["ShowTenth"].toBool();
    mShowGameNo = map["ShowGameNo"].toBool();

    mShowIndicators = map["ShowIndicators"].toBool();
    mIndicatorType = map["IndicatorType"].toInt();
    mHideColors = map["HideColors"].toBool();

    mPegCnt = map["ColumnCount"].toInt();
    mColorCnt = map["ColorCount"].toInt();
    mDoubles = map["Doubles"].toBool();

    mGameMode = map["GameMode"].toInt();
    mAutoClose = map["AutoClose"].toBool();
    mAutoHints = map["AutoHints"].toBool();
    mHintsDelay = map["HintsDelay"].toInt();

    mSolverStrength = map["SolverStrength"].toInt();

    mGamesListMaxCnt = map["GamesListMaxCnt"].toInt();
    mCustomRowHColor = map["CustomRowHColor"].toBool();
    mRowHColorFg = map["RowHColorFg"].toString();
    mRowHColorBg = map["RowHColorBg"].toString();
    mHighScoreHandling = map["HighScoreHandling"].toInt();
    mHighScoreUserName = map["HighScoreUserName"].toString();

    Validate();
}

void Settings::SaveSettingsMap(QMap<QString, QVariant> &map)
{
    map["ShowToolBar"] = mShowToolBar;
    map["ShowMenuBar"] = mShowMenuBar;
    map["ShowStatusBar"] = mShowStatusBar;
    map["ShowTimer"] = mShowTimer;
    map["ShowTenth"] = mShowTenth;
    map["ShowGameNo"] = mShowGameNo;

    map["ShowIndicators"] = mShowIndicators;
    map["IndicatorType"] = mIndicatorType;
    map["HideColors"] = mHideColors;

    map["ColumnCount"] = mPegCnt;
    map["ColorCount"] = mColorCnt;
    map["Doubles"] = mDoubles;

    map["GameMode"] = mGameMode;
    map["AutoClose"] = mAutoClose;
    map["AutoHints"] = mAutoHints;
    map["HintsDelay"] = mHintsDelay;

    map["SolverStrength"] = mSolverStrength;

    map["GamesListMaxCnt"] = mGamesListMaxCnt;
    map["CustomRowHColor"] = mCustomRowHColor;
    map["RowHColorFg"] = mRowHColorFg;
    map["RowHColorBg"] = mRowHColorBg;
    map["HighScoreHandling"] = mHighScoreHandling;
    map["HighScoreUserName"] = mHighScoreUserName;
}

void Settings::ReadSettings()
{
    mSettings.beginGroup("Appearance");
    mShowToolBar = mSettings.value("ShowToolBar", mShowToolBar).toBool();
    mShowMenuBar = mSettings.value("ShowMenuBar", mShowMenuBar).toBool();
    mShowStatusBar = mSettings.value("ShowStatusBar", mShowStatusBar).toBool();
    mShowTimer = mSettings.value("ShowTimer", mShowTimer).toBool();
    mShowTenth = mSettings.value("ShowTenth", mShowTenth).toBool();
    mShowGameNo = mSettings.value("ShowGameNo", mShowGameNo).toBool();

    mShowIndicators = mSettings.value("ShowIndicators", mShowIndicators).toBool();
    mIndicatorType = mSettings.value("IndicatorType", mIndicatorType).toInt();
    mHideColors = mSettings.value("HideColors", mHideColors).toBool();
    mSettings.endGroup();

    mSettings.beginGroup("Game");
    mPegCnt = mSettings.value("ColumnCount", mPegCnt).toInt();
    mColorCnt = mSettings.value("ColorCount", mColorCnt).toInt();
    mDoubles = mSettings.value("Doubles", mDoubles).toBool();

    mGameMode = mSettings.value("GameMode", mGameMode).toInt();
    mAutoClose = mSettings.value("AutoClose", mAutoClose).toBool();
    mAutoHints = mSettings.value("AutoHints", mAutoHints).toBool();
    mHintsDelay = mSettings.value("HintsDelay", mHintsDelay).toInt();

    mSolverStrength = mSettings.value("SolverStrength", mSolverStrength).toInt();

    mSettings.endGroup();

    mSettings.beginGroup("History");

    mPrevGamesStr = mSettings.value("PrevGames", "").toString();
    mSavedGamesStr = mSettings.value("SavedGames", "").toString();
    mHighGamesStr = mSettings.value("HighScoreList", "").toString();

    mGamesListMaxCnt = mSettings.value("GamesListMaxCnt", mGamesListMaxCnt).toInt();
    mCustomRowHColor = mSettings.value("CustomRowHColor", mCustomRowHColor).toBool();
    mRowHColorFg = mSettings.value("RowHColorFg", mRowHColorFg).toString();
    mRowHColorBg = mSettings.value("RowHColorBg", mRowHColorBg).toString();
    mHighScoreHandling = mSettings.value("HighScoreHandling", mHighScoreHandling).toInt();
    mHighScoreUserName = mSettings.value("HighScoreUserName", mHighScoreUserName).toString();

    mSettings.endGroup();

    Validate();
}

void Settings::Validate()
{
    if (mIndicatorType != INDICATOR_LETTER && mIndicatorType != INDICATOR_NUMBER)
    {
        mIndicatorType = INDICATOR_LETTER;
    }
    if (mPegCnt < 2 || mPegCnt > 5)
    {
        mPegCnt = 4;
    }
    if (mColorCnt < 2 || mColorCnt > 10)
    {
        mColorCnt = 8;
    }
    if (mGameMode != ColorCode::MODE_HVM && mGameMode != ColorCode::MODE_MVH)
    {
        mGameMode = ColorCode::MODE_HVM;
    }
    if (mSolverStrength < CCSolver::STRENGTH_LOW || mSolverStrength > CCSolver::STRENGTH_HIGH)
    {
        mSolverStrength = CCSolver::STRENGTH_HIGH;
    }
    if (mHintsDelay > 10000)
    {
        mHintsDelay = 10000;
    }
    else if (mHintsDelay < 0)
    {
        mHintsDelay = 0;
    }
    if (mHighScoreHandling == HIGHSCORE_AUTO && mHighScoreUserName == "")
    {
        mHighScoreHandling = HIGHSCORE_PROMPT;
    }
    else if (mHighScoreHandling < HIGHSCORE_NO || mHighScoreHandling > HIGHSCORE_AUTO)
    {
        mHighScoreHandling = HIGHSCORE_PROMPT;
    }
    if (mGamesListMaxCnt < 5 || mGamesListMaxCnt > 50)
    {
        mGamesListMaxCnt = DEF_LIST_MAX_CNT;
    }

    QColor color;
    color.setNamedColor(mRowHColorFg);
    if (!color.isValid())
    {
        mRowHColorFg = mDefMap["RowHColorFg"].toString();
    }
    color.setNamedColor(mRowHColorBg);
    if (!color.isValid())
    {
        mRowHColorBg = mDefMap["RowHColorBg"].toString();
    }
}

CCGame* Settings::GetCurSettingsGame() const
{
    CCGame* g = new CCGame();

    g->mColorCnt = mColorCnt;
    g->mPegCnt = mPegCnt;
    g->mDoubles = int(mDoubles);
    g->mGameMode = mGameMode;
    g->mTime = QDateTime::currentDateTime().toTime_t();

    return g;
}

QString Settings::GetActiveRowHFg() const
{
    if (mCustomRowHColor)
    {
        return mRowHColorFg;
    }
    return mDefMap["RowHColorFg"].toString();
}

QString Settings::GetActiveRowHBg() const
{
    if (mCustomRowHColor)
    {
        return mRowHColorBg;
    }
    return mDefMap["RowHColorBg"].toString();
}

void Settings::WriteSettings()
{
    mSettings.beginGroup("Appearance");
    mSettings.setValue("ShowToolBar", mShowToolBar);
    mSettings.setValue("ShowMenuBar", mShowMenuBar);
    mSettings.setValue("ShowStatusBar", mShowStatusBar);
    mSettings.setValue("ShowTimer", mShowTimer);
    mSettings.setValue("ShowTenth", mShowTenth);
    mSettings.setValue("ShowGameNo", mShowGameNo);

    mSettings.setValue("ShowIndicators", mShowIndicators);
    mSettings.setValue("IndicatorType", mIndicatorType);
    mSettings.setValue("HideColors", mHideColors);
    mSettings.endGroup();

    mSettings.beginGroup("Game");
    mSettings.setValue("ColumnCount", mPegCnt);
    mSettings.setValue("ColorCount", mColorCnt);
    mSettings.setValue("Doubles", mDoubles);

    mSettings.setValue("GameMode", mGameMode);
    mSettings.setValue("AutoClose", mAutoClose);
    mSettings.setValue("AutoHints", mAutoHints);
    mSettings.setValue("HintsDelay", mHintsDelay);

    mSettings.setValue("SolverStrength", mSolverStrength);
    mSettings.endGroup();

    mSettings.beginGroup("History");
    mSettings.setValue("PrevGames", GetPrevGamesModel()->GetListStr());
    mSettings.setValue("SavedGames", GetSavedGamesModel()->GetListStr());
    mSettings.setValue("HighScoreList", GetHighScoresModel()->GetListStr());

    mSettings.setValue("GamesListMaxCnt", mGamesListMaxCnt);
    mSettings.setValue("CustomRowHColor", mCustomRowHColor);
    mSettings.setValue("RowHColorFg", mRowHColorFg);
    mSettings.setValue("RowHColorBg", mRowHColorBg);
    mSettings.setValue("HighScoreUserName", mHighScoreUserName);
    mSettings.setValue("HighScoreHandling", mHighScoreHandling);

    mSettings.endGroup();

    mSettings.sync();
}

