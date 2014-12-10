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

#ifndef CCGAME_H
#define CCGAME_H

#include <QtCore>
#include <iostream>
#include "colorcode.h"

class CCGame
{
    public:
        CCGame();
        CCGame(const QString &str);
        CCGame(const QString &unm, const uint t, const int sc, const uint gameno, const int gcnt, const int hcnt, const uint dur, const int gmode, const int ccnt, const int pcnt, const int d);

        bool operator==(const CCGame &other) const;
        bool operator!=(const CCGame &other) const;

        uint mTime;
        int mScore;

        uint mGameNo;
        int mGuessCnt;
        int mHintCnt;
        uint mDuration;

        int mGameMode;
        int mColorCnt;
        int mPegCnt;
        int mDoubles;

        QString GetUserName() const;
        void SetUserName(QString unm);
        uint GetTime() const;
        int GetScore() const;
        QString GetDateTimeStr() const;
        QString GetDatePartStr() const;
        QString GetTimePartStr() const;
        QString GetCmbStr() const;

        void Anonymize();
        QString ToString() const;
        bool HasSameSettings(const CCGame &g) const;
        bool IsValidGame() const;

    private:
        QString mUserNm;

};

#endif // CCGAME_H
