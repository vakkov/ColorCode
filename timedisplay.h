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

#ifndef TIMEDISPLAY_H
#define TIMEDISPLAY_H

#include <QGraphicsTextItem>
#include <QTime>
#include <QtWidgets>
#include <QFont>

class GameTimer;

class TimeDisplay : public QGraphicsTextItem
{
    Q_OBJECT

    public:
        TimeDisplay(QGraphicsItem* parent = 0);
        ~TimeDisplay();

        int GetGameTime() const;
        bool IsActive() const;
        bool IsPaused() const;
        bool IsStopped() const;
        void SetTenth(bool b);

    public slots:
        void StartT();
        void PauseT();
        void ResumeT();
        void StopT();
        void DisplayTime(int tick = 0);

    private:
        GameTimer* mTimer;
        bool mPaused;
        bool mStopped;
        int mTime;
        int mGameTime;
        QTime mT0;
        QTime mT1;

        int mInterval;

        bool mShowTenth;
        QString mSs;
        QString mLastSs;
        QString mHs;

        QFont mFont;
};


#endif
