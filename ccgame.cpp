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

#include "ccgame.h"
#include "settings.h"

using namespace std;

CCGame::CCGame()
{
    mUserNm = "";
    mTime = 0;
    mScore = 0;

    mGameNo   = 0;
    mGuessCnt = 0;
    mHintCnt  = 0;
    mDuration = 0;

    mColorCnt = 0;
    mPegCnt   = 0;
    mDoubles  = 0;

    mGameMode = ColorCode::MODE_HVM;
}

CCGame::CCGame(const QString &str)
{
    QStringList fields = str.split(QString("|"));
    if (fields.size() == 11)
    {
        SetUserName(QUrl::fromPercentEncoding(fields.at(0).toUtf8()));
        bool ok;
        mTime = fields.at(1).toInt(&ok);
        mScore = fields.at(2).toInt(&ok);

        mGameNo   = fields.at(3).toUInt(&ok);
        mGuessCnt = fields.at(4).toInt(&ok);
        mHintCnt  = fields.at(5).toInt(&ok);
        mDuration = fields.at(6).toUInt(&ok);

        mGameMode = fields.at(7).toInt(&ok);
        mColorCnt = fields.at(8).toInt(&ok);
        mPegCnt   = fields.at(9).toInt(&ok);
        mDoubles  = fields.at(10).toInt(&ok);
    }
    else
    {
        mUserNm = "";
        mTime = 0;
        mScore = 0;

        mGameNo   = 0;
        mGuessCnt = 0;
        mHintCnt  = 0;
        mDuration = 0;

        mGameMode = ColorCode::MODE_HVM;
        mColorCnt = 0;
        mPegCnt   = 0;
        mDoubles  = 0;
    }
}

CCGame::CCGame(const QString &unm, const uint t, const int sc, const uint gameno, const int gcnt, const int hcnt, const uint dur, const int gmode, const int ccnt, const int pcnt, const int d)
{
    mUserNm   = unm;
    mTime     = t;
    mScore    = sc;

    mGameNo   = gameno;
    mGuessCnt = gcnt;
    mHintCnt  = hcnt;
    mDuration = dur;

    mGameMode = gmode;
    mColorCnt = ccnt;
    mPegCnt   = pcnt;
    mDoubles  = d;
}

bool CCGame::operator==(const CCGame &other) const
{
    return ( GetUserName() == other.GetUserName()
            && mTime == other.mTime
            && mScore == other.mScore

            && mGameNo == other.mGameNo
            && mGuessCnt == other.mGuessCnt
            && mHintCnt == other.mHintCnt
            && mDuration == other.mDuration

            && mGameMode == other.mGameMode
            && mColorCnt == other.mColorCnt
            && mPegCnt == other.mPegCnt
            && mDoubles == other.mDoubles );
}

bool CCGame::operator!=(const CCGame &other) const
{
    return !(*this == other);
}

QString CCGame::GetUserName() const
{
    return mUserNm;
}

void CCGame::SetUserName(QString unm)
{
    QRegExp rx("[^\\S ]+");
    unm.replace(rx, "");
    unm = unm.trimmed();
    unm.truncate(Settings::MAX_LENGTH_USERNAME);

    mUserNm = unm;
}

uint CCGame::GetTime() const
{
    return mTime;
}

int CCGame::GetScore() const
{
    return mScore;
}

QString CCGame::GetDatePartStr() const
{
    return QDateTime::fromTime_t(mTime).date().toString(Qt::ISODate);
}

QString CCGame::GetTimePartStr() const
{
    return QDateTime::fromTime_t(mTime).time().toString(Qt::ISODate);
}

QString CCGame::GetDateTimeStr() const
{
    return QDateTime::fromTime_t(mTime).toString(Qt::TextDate);
}

QString CCGame::GetCmbStr() const
{
    return QString::number(mGameNo) + QString("    (") + QString::number(mDoubles) + QString("|") + QString::number(mPegCnt) + QString("|") + QString::number(mColorCnt) + QString(")    ") + GetDateTimeStr();
}

void CCGame::Anonymize()
{
    SetUserName("");
    mHintCnt = mGuessCnt = mDuration = mScore = 0;
}

QString CCGame::ToString() const
{
    QStringList slist = QStringList();
    slist << QString(QUrl::toPercentEncoding(mUserNm));
    slist << QString::number(mTime);
    slist << QString::number(mScore);

    slist << QString::number(mGameNo);
    slist << QString::number(mGuessCnt);
    slist << QString::number(mHintCnt);
    slist << QString::number(mDuration);

    slist << QString::number(mGameMode);
    slist << QString::number(mColorCnt);
    slist << QString::number(mPegCnt);
    slist << QString::number(mDoubles);
    return slist.join("|");
}

bool CCGame::HasSameSettings(const CCGame &g) const
{
    return (mGameNo == g.mGameNo && mColorCnt == g.mColorCnt && mPegCnt == g.mPegCnt && mDoubles == g.mDoubles);
}

bool CCGame::IsValidGame() const
{
    return ( mGameNo > 0
             && mColorCnt >= ColorCode::MIN_COLOR_CNT && mColorCnt <= ColorCode::MAX_COLOR_CNT
             && mPegCnt >= 2 && mPegCnt <= 5
             && (mDoubles == 1 || mDoubles == 0) );
}
