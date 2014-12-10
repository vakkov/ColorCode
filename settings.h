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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <iostream>
#include "ccgame.h"

class Settings : public QObject
{
    Q_OBJECT

    public:
        Settings();
        ~Settings();

        static const int INDICATOR_LETTER;
        static const int INDICATOR_NUMBER;

        static const int DEF_COLOR_CNT;
        static const int DEF_PEG_CNT;
        static const int DEF_DOUBLES;
        static const int DEF_LIST_MAX_CNT;

        static const int HIGHSCORE_NO;
        static const int HIGHSCORE_PROMPT;
        static const int HIGHSCORE_AUTO;

        static const int MAX_LENGTH_USERNAME;

        void Init();
        void InitSettings();
        void ReadSettings();
        void Validate();
        void SaveLastSettings();
        void RestoreLastSettings();
        void RestoreDefSettings();
        void WriteSettings();

        QMap<QString, QVariant> GetDefaultsMap() const;

        CCGame* GetCurSettingsGame() const;
        QString GetActiveRowHFg() const;
        QString GetActiveRowHBg() const;

        QSettings mSettings;

        bool mShowToolBar;
        bool mShowMenuBar;
        bool mShowStatusBar;
        bool mShowTimer;
        bool mShowTenth;
        bool mShowGameNo;
        bool mShowIndicators;
        int mIndicatorType;
        bool mHideColors;
        bool mCustomRowHColor;
        QString mRowHColorFg;
        QString mRowHColorBg;

        int mPegCnt;
        int mColorCnt;
        bool mDoubles;
        int mGameMode;
        bool mAutoClose;
        bool mAutoHints;
        int mSolverStrength;
        int mHintsDelay;
        QString mPrevGamesStr;
        QString mSavedGamesStr;
        QString mHighGamesStr;

        int mGamesListMaxCnt;
        int mHighScoreHandling;
        QString mHighScoreUserName;

    public slots:

    signals:
        void ReadSettingsGamesDone();

    private:
        void RestoreSettingsMap(QMap<QString, QVariant> &map);
        void SaveSettingsMap(QMap<QString, QVariant> &map);
        QMap<QString, QVariant> mDefMap;
        QMap<QString, QVariant> mLastMap;
};

Settings* GetSettings();

#endif // SETTINGS_H
