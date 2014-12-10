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
#include "timedisplay.h"
#include "gametimer.h"


TimeDisplay::TimeDisplay(QGraphicsItem* parent) : QGraphicsTextItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);

    mFont = QFont("Arial", 18, QFont::Normal, false);
    mFont.setStyleHint(QFont::SansSerif);
    mFont.setPixelSize(24);
    setFont(mFont);
    setDefaultTextColor(QColor("#d7d8da"));

    mShowTenth = true;
    mSs = mLastSs = "";
    mHs = "";

    mInterval = 50;
    mPaused = false;
    mStopped = true;
    mGameTime = 0;
    mTime = 0;

    mTimer = new GameTimer(this);
    connect(mTimer, SIGNAL(TimeOutSignal(int)), this, SLOT(DisplayTime(int)));
    mTimer->Start();

    setTextWidth(440);
    setTransform(QTransform::fromScale(0.5, 0.5), true);
    //scale(0.5, 0.5);
}

TimeDisplay::~TimeDisplay()
{
}

int TimeDisplay::GetGameTime() const
{
    return mGameTime;
}

bool TimeDisplay::IsActive() const
{
    return (!mPaused && !mStopped);
}

bool TimeDisplay::IsPaused() const
{
    return mPaused;
}

bool TimeDisplay::IsStopped() const
{
    return mPaused;
}

void TimeDisplay::SetTenth(bool b)
{
    mShowTenth = b;
}

void TimeDisplay::StartT()
{
    mTime = 0;
    mT0.start();
    DisplayTime();
    mPaused = false;
    mStopped = false;
}

void TimeDisplay::PauseT()
{
    if (!mStopped && !mPaused)
    {
        mTime += mT0.restart();
        DisplayTime();
        mPaused = true;
    }
}

void TimeDisplay::ResumeT()
{
    if (mPaused && !mStopped)
    {
        mT0.restart();
        mPaused = false;
    }
}

void TimeDisplay::StopT()
{
    if (!mStopped && !mPaused)
    {
        mTime += mT0.restart();
        DisplayTime();
        mStopped = true;
        mPaused = false;
        mTime = 0;
    }
}

void TimeDisplay::DisplayTime(int tick)
{
    if (mPaused || mStopped)
    {
        return;
    }

    mGameTime = mT0.elapsed() + mTime;
    mT1 = QTime(0, 0, 0, 0);
    mT1 = mT1.addMSecs(mGameTime);

    if (mShowTenth)
    {
        if (tick == 0)
        {
            mHs = QString::number((int)(mT1.msec()/10));
        }
        else
        {
            mHs = QString::number(((int)(mT1.msec()/50))*5);
        }
        if (mHs.length() < 2)
        {
            mHs.prepend("0");
        }
    }

    if (isVisible())
    {
        QString tstr;
        if (!mShowTenth)
        {
            mSs = mT1.toString("ss");
            if (mSs == mLastSs)
            {
                return;
            }
            tstr = QString("<table align='center' cellpadding='0' cellspacing='0' border='0'><tr><td width='30' align='center'>%1</td><td width='10' align='center'>:</td><td width='30' align='center'>%2</td><td width='10' align='center'>:</td><td width='30' align='center'>%3</td></tr></table>").arg( mT1.toString("hh"), mT1.toString("mm"), mSs);
            mLastSs = mSs;
        }
        else
        {
            tstr = QString("<table align='center' cellpadding='0' cellspacing='0' border='0'><tr><td width='30' align='center'>%1</td><td width='10' align='center'>:</td><td width='30' align='center'>%2</td><td width='10' align='center'>:</td><td width='30' align='center'>%3</td><td width='10' align='center'>:</td><td width='30' align='center'>%4</td></tr></table>").arg( mT1.toString("hh"), mT1.toString("mm"), mT1.toString("ss"), mHs);
        }
        setHtml(tstr);
    }
}
