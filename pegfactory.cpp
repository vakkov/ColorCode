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
#include "pegfactory.h"

using namespace std;

const int PegFactory::XPOS_BTNS_COL = 279;

PegFactory::PegFactory()
{
    mWin = ColorCode::INSTANCE;
    mSettings = GetSettings();
    mHideColors = false;

    InitTypesMap();
    InitPegBtns();
}

ColorPeg* PegFactory::CreatePeg(int ix)
{
    if (ix < 0 || ix >= ColorCode::mColorCnt)
    {
        ix = mAllPegs.size() % ColorCode::mColorCnt;
    }

    QPoint btnpos = mBtnPos[ix];

    PegType *pt = mTypesMap[ix];
    ColorPeg *peg;

    if (mPegBuff.empty())
    {
        peg = new ColorPeg;
        peg->SetPegType(pt);
        peg->SetBtn(true);
        peg->SetIndicator(mSettings->mShowIndicators, mSettings->mIndicatorType, mHideColors);
        peg->SetId(mAllPegs.size());
        mWin->mScene->addItem(peg);
        peg->setPos(btnpos);
        peg->setZValue(ColorCode::LAYER_PEGS);
        mAllPegs.push_back(peg);
        mWin->mScene->clearSelection();
        mWin->mScene->update(btnpos.x(), btnpos.y(), 38, 38);

        connect(peg, SIGNAL(PegPressSignal(ColorPeg *)), this, SLOT(PegPressSlot(ColorPeg *)));
        connect(peg, SIGNAL(PegSortSignal(ColorPeg *)), this, SLOT(PegSortSlot(ColorPeg *)));
        connect(peg, SIGNAL(PegReleasedSignal(ColorPeg *)), this, SLOT(PegReleasedSlot(ColorPeg *)));
    }
    else
    {
        unsigned int sz = mPegBuff.size();
        peg = mPegBuff.at(sz - 1);
        mPegBuff.resize(sz - 1);

        peg->setVisible(true);

        peg->SetPegType(pt);
        peg->SetBtn(true);
        peg->SetEnabled(true);
        peg->setPos(btnpos);
        mWin->mScene->clearSelection();
        mWin->mScene->update(btnpos.x(), btnpos.y(), 38, 38);
    }

    return peg;
}

ColorPeg* PegFactory::GetPegBtn(int ix)
{
    return mPegBtns[ix];
}

void PegFactory::RemovePegSlot(ColorPeg* cp)
{
    if (cp != NULL)
    {
        cp->Reset();
        cp->setVisible(false);
        mPegBuff.push_back(cp);
        mWin->mScene->update(cp->pos().x(), cp->pos().y(), 45, 45);
    }
}

void PegFactory::PegPressSlot(ColorPeg* cp)
{
    if (cp == NULL) { return; }
    cp->setZValue(ColorCode::LAYER_DRAG);
    if (cp->IsBtn())
    {
        cp->SetBtn(false);
        mPegBtns[cp->GetPegType()->ix] = CreatePeg(cp->GetPegType()->ix);
        mPegBtns[cp->GetPegType()->ix]->SetEnabled(false);
    }
}

void PegFactory::PegSortSlot(ColorPeg* cp)
{
    if (cp == NULL) { return; }
    cp->setZValue(ColorCode::LAYER_DRAG);
    if (cp->IsBtn())
    {
        cp->SetBtn(false);
        mPegBtns[cp->GetPegType()->ix] = CreatePeg(cp->GetPegType()->ix);
        mPegBtns[cp->GetPegType()->ix]->SetEnabled(false);
    }
}

void PegFactory::PegReleasedSlot(ColorPeg* cp)
{
    if (cp == NULL || !cp) { return; }

    cp->setZValue(ColorCode::LAYER_PEGS);
    mWin->mScene->clearSelection();
    mWin->mScene->clearFocus();

    if (cp->GetSort() == 0)
    {
        emit(SnapPegSignal(cp));
    }
    else
    {
        int ystart = ColorCode::mRowY0 + (ColorCode::MAX_COLOR_CNT - ColorCode::mColorCnt) * 40;
        int ix1 = ((cp->pos().y()) - ystart + 18) / 40;
        ix1 = min(ColorCode::mColorCnt - 1, max(0, ix1));

        int ix0 = cp->GetPegType()->ix;
        if (ix0 != ix1)
        {
            ChangeColorOrder(cp->GetSort(), ix0, ix1);
        }
        RemovePegSlot(cp);
    }

    if (!mPegBtns[cp->GetPegType()->ix]->isEnabled())
    {
        mPegBtns[cp->GetPegType()->ix]->SetEnabled(true);
    }
}

void PegFactory::SetPaused()
{
    bool paused = (mWin->GetGameState() == ColorCode::STATE_PAUSED);
    vector<ColorPeg *> dragged;
    vector<ColorPeg *>::iterator it;
    for (it = mAllPegs.begin(); it < mAllPegs.end(); it++)
    {
        if ((*it)->GetIsDragged())
        {
            dragged.push_back(*it);
        }
        (*it)->SetPaused(paused);
    }
    if (!paused)
    {
        for (it = dragged.begin(); it < dragged.end(); it++)
        {
            (*it)->ForceRelease();
        }
    }
    SetIndicators();
}

void PegFactory::SetIndicators()
{
    bool show_indicators;
    if (mWin->GetGameState() == ColorCode::STATE_PAUSED)
    {
        mHideColors = true;
        show_indicators = false;
    }
    else if (!mSettings->mShowIndicators)
    {
        mHideColors = false;
        show_indicators = false;
    }
    else
    {
        mHideColors = mSettings->mHideColors;
        show_indicators = mSettings->mShowIndicators;
    }
    vector<ColorPeg *>::iterator it;
    for (it = mAllPegs.begin(); it < mAllPegs.end(); it++)
    {
        (*it)->SetIndicator(show_indicators, mSettings->mIndicatorType, mHideColors);
    }
}

void PegFactory::ChangeColorOrder(const int sorttype, const int ix0, const int ix1)
{
    QRadialGradient tmp = mGradMap[ix0];

    if (sorttype == 1)
    {
        mGradMap[ix0] = mGradMap[ix1];
    }
    else
    {
        int step = (ix1 > ix0) ? 1 : -1;
        int i;
        for (i = ix0;; i += step)
        {
            mGradMap[i] = mGradMap[i + step];
            if (i == ix1 - step)
            {
                break;
            }
        }
    }

    mGradMap[ix1] = tmp;
}

void PegFactory::ResetColorsOrderSlot()
{
    int ix;
    for (ix = 0; ix < ColorCode::MAX_COLOR_CNT; ++ix)
    {
        mGradMap[ix] = mGradBuff[ix];
    }
    mWin->mScene->update(mWin->mScene->sceneRect());
}

void PegFactory::ApplyColorCnt()
{
    int ccnt = mSettings->mColorCnt;
    ccnt = max(2, min(10, ccnt));

    if (ColorCode::mColorCnt == ccnt)
    {
        return;
    }

    ColorCode::mColorCnt = ccnt;

    int ystart = ColorCode::mRowY0 + (ColorCode::MAX_COLOR_CNT - ccnt) * 40;
    int ypos;
    int i;

    for (i = 0; i < ColorCode::MAX_COLOR_CNT; ++i)
    {
        ypos = ystart + i * 40 + 2;
        mBtnPos[i] = QPoint(XPOS_BTNS_COL, ypos);

        if (i < ColorCode::mColorCnt)
        {
            if (mPegBtns[i] == NULL)
            {
                mPegBtns[i] = CreatePeg(i);
            }
            mPegBtns[i]->setPos(mBtnPos[i]);
            mPegBtns[i]->setVisible(true);
        }
        else if (mPegBtns[i] != NULL)
        {
            mPegBtns[i]->setVisible(false);
        }
    }
}

void PegFactory::InitTypesMap()
{
    int ix;

    QRadialGradient grad = QRadialGradient(20, 20, 40, 5, 5);
    QRadialGradient grad2 = QRadialGradient(18, 18, 18, 12, 12);

    ix = 0;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#FFFF80"));
    grad.setColorAt(1.0, QColor("#C05800"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::green;

    ix = 1;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#FF3300"));
    grad.setColorAt(1.0, QColor("#400040"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::green;

    ix = 2;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#33CCFF"));
    grad.setColorAt(1.0, QColor("#000080"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::red;

    ix = 3;
    mTypesMap[ix] = new PegType;
    grad2.setColorAt(0.0, Qt::white);
    grad2.setColorAt(0.5, QColor("#f8f8f0"));
    grad2.setColorAt(0.7, QColor("#f0f0f0"));
    grad2.setColorAt(1.0, QColor("#d0d0d0"));
    mGradMap[ix] = grad2;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::green;

    ix = 4;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#808080"));
    grad.setColorAt(1.0, Qt::black);
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::red;

    ix = 5;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#66FF33"));
    grad.setColorAt(1.0, QColor("#385009"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::red;

    ix = 6;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#FF9900"));
    grad.setColorAt(1.0, QColor("#A82A00"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::red;

    ix = 7;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#BA88FF"));
    grad.setColorAt(1.0, QColor("#38005D"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::green;

    ix = 8;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#00FFFF"));
    grad.setColorAt(1.0, QColor("#004040"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::green;

    ix = 9;
    mTypesMap[ix] = new PegType;
    grad.setColorAt(0.0, QColor("#FFC0FF"));
    grad.setColorAt(1.0, QColor("#800080"));
    mGradMap[ix] = grad;
    mTypesMap[ix]->grad = &mGradMap[ix];
    mTypesMap[ix]->ix = ix;
    mTypesMap[ix]->pencolor = Qt::green;

    for (ix = 0; ix < ColorCode::MAX_COLOR_CNT; ++ix)
    {
        mTypesMap[ix]->let = 'A' + ix;
        mGradBuff[ix] = mGradMap[ix];
    }
}

void PegFactory::InitPegBtns()
{
    for (int i = 0; i < ColorCode::MAX_COLOR_CNT; ++i)
    {
        mPegBtns[i] = NULL;
    }
}
