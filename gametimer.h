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

#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <QtWidgets>
#include <iostream>
#include "colorcode.h"
#include "timedisplay.h"

using namespace std;

class GameTimer : public QThread
{
    Q_OBJECT

    public:
        GameTimer(QObject* parent = 0);
        ~GameTimer();

        void run();

    public slots:
        void Start();
        void TimeOut();

    signals:
        void TimeOutSignal(int tick);

    private:
        int mInterval;
};

#endif // GAMETIMER_H
