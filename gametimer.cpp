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

#include "gametimer.h"

GameTimer::GameTimer(QObject* parent) : QThread(parent)
{
    mInterval = 50;
}

GameTimer::~GameTimer()
{
    wait();
}

void GameTimer::run()
{
    QTimer* timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeOut()), Qt::DirectConnection);
    timer->start(mInterval);
    exec();
}

void GameTimer::Start()
{
    start();
}

void GameTimer::TimeOut()
{
    emit TimeOutSignal(1);
}
