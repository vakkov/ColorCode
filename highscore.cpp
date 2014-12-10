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

#include "highscore.h"

using namespace std;

HighScore::HighScore()
{
    mCombinationsCnt = 0;
    mRowsCnt = 0;
    mTime = 0;

}

void HighScore::NewGame(int ccnt)
{
    mRowsCnt = 0;
    mTime = 0;
    mCombinationsCnt = ccnt;
}

int HighScore::GetScore(int rcnt, int t)
{
    mRowsCnt = rcnt;
    mTime = t;
    return GetScore();
}

int HighScore::GetScore() const
{
    if (mCombinationsCnt == 0 || mRowsCnt == 0 || mTime == 0)
    {
        return 0;
    }

    double dd = mCombinationsCnt * 10000;
    double dv = mRowsCnt * mTime / 100;
    return int(dd / dv);
}
