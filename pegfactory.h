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

#ifndef PEGFACTORY_H
#define PEGFACTORY_H

#include <iostream>
#include <QRadialGradient>
#include "colorcode.h"
#include "colorpeg.h"
#include "settings.h"


class PegFactory : public QObject
{
    Q_OBJECT

    public:
        PegFactory();

        static const int XPOS_BTNS_COL;

        ColorPeg* CreatePeg(int ix);
        ColorPeg* GetPegBtn(int ix);
        void SetPaused();
        void SetIndicators();
        void ChangeColorOrder(const int sorttype, const int ix0, const int ix1);
        void ApplyColorCnt();

    public slots:
        void RemovePegSlot(ColorPeg* cp);
        void ResetColorsOrderSlot();
        void PegPressSlot(ColorPeg* cp);
        void PegSortSlot(ColorPeg* cp);
        void PegReleasedSlot(ColorPeg* cp);

    signals:
        void SnapPegSignal(ColorPeg* cp);

    private:
        ColorCode* mWin;
        Settings* mSettings;

        PegType* mTypesMap[10];
        ColorPeg* mPegBtns[10];
        QPoint mBtnPos[10];
        QRadialGradient mGradMap[10];
        QRadialGradient mGradBuff[10];

        std::vector<ColorPeg *> mAllPegs;
        std::vector<ColorPeg *> mPegBuff;

        bool mHideColors;

        void InitTypesMap();
        void InitPegBtns();
};

#endif // PEGFACTORY_H
