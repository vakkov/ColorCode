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

#include "prefdialog.h"

using namespace std;

PrefDialog::PrefDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);
    setSizeGripEnabled(false);
    mSuppressSlots = false;
    InitControls();
}

PrefDialog::~PrefDialog()
{
}

void PrefDialog::InitControls()
{
    mLevelPresetsCmb->addItem(tr("... Select from predefined level settings"), -1);
    mLevelPresetsCmb->addItem(tr("Beginner (2 Colors, 2 Slots, Doubles)"), 0);
    mLevelPresetsCmb->addItem(tr("Easy (4 Colors, 3 Slots, No Doubles)"), 1);
    mLevelPresetsCmb->addItem(tr("Classic (6 Colors, 4 Slots, Doubles)"), 2);
    mLevelPresetsCmb->addItem(tr("Challenging (8 Colors, 4 Slots, Doubles)"), 3);
    mLevelPresetsCmb->addItem(tr("Hard (10 Colors, 5 Slots, Doubles)"), 4);

    mPegCntCmb->addItem("2 " + tr("Slots"), 2);
    mPegCntCmb->addItem("3 " + tr("Slots"), 3);
    mPegCntCmb->addItem("4 " + tr("Slots"), 4);
    mPegCntCmb->addItem("5 " + tr("Slots"), 5);

    mColorCntCmb->addItem("2 " + tr("Colors"), 2);
    mColorCntCmb->addItem("3 " + tr("Colors"), 3);
    mColorCntCmb->addItem("4 " + tr("Colors"), 4);
    mColorCntCmb->addItem("5 " + tr("Colors"), 5);
    mColorCntCmb->addItem("6 " + tr("Colors"), 6);
    mColorCntCmb->addItem("7 " + tr("Colors"), 7);
    mColorCntCmb->addItem("8 " + tr("Colors"), 8);
    mColorCntCmb->addItem("9 " + tr("Colors"), 9);
    mColorCntCmb->addItem("10 " + tr("Colors"), 10);

    mGameModeCmb->addItem(tr("Human vs. Computer"), ColorCode::MODE_HVM);
    mGameModeCmb->addItem(tr("Computer vs. Human"), ColorCode::MODE_MVH);

    mStrengthCmb->addItem(tr("Low"), CCSolver::STRENGTH_LOW);
    mStrengthCmb->addItem(tr("Medium"), CCSolver::STRENGTH_MEDIUM);
    mStrengthCmb->addItem(tr("High"), CCSolver::STRENGTH_HIGH);
    mDelaySb->setMaximum(10000);
    mDelaySb->setMinimum(0);
    mDelaySb->setSingleStep(100);

    mUserNameLe->setMaxLength(Settings::MAX_LENGTH_USERNAME);

    connect(mCustomRowHColorCb, SIGNAL(stateChanged(int)), this, SLOT(CustomRowHColorChangedSlot(int)));
    connect(mRowHColorFgBtn, SIGNAL(clicked()), this, SLOT(SetBtnColorSlot()));
    connect(mRowHColorBgBtn, SIGNAL(clicked()), this, SLOT(SetBtnColorSlot()));

    mRestoreBtn = mButtonBox->button(QDialogButtonBox::RestoreDefaults);
    mOkBtn = mButtonBox->button(QDialogButtonBox::Ok);
    mApplyBtn = mButtonBox->button(QDialogButtonBox::Apply);
    mCancelBtn = mButtonBox->button(QDialogButtonBox::Cancel);

    connect(mLevelPresetsCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(LevelPresetChangedSlot()));
    connect(mColorCntCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(ColorCntChangedSlot()));
    connect(mPegCntCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(PegCntChangedSlot()));
    connect(mDoublesCb, SIGNAL(stateChanged(int)), this, SLOT(DoublesChangedSlot()));
    connect(mShowIndicatorsCb, SIGNAL(stateChanged(int)), this, SLOT(ShowIndicatorsChangedSlot(int)));
    connect(mShowTimerCb, SIGNAL(stateChanged(int)), this, SLOT(ShowTimerChangedSlot(int)));

    connect(mResetColorOrderBtn, SIGNAL(clicked()), this, SLOT(ResetColorOrderSlot()));

    connect(mRestoreBtn, SIGNAL(clicked()), this, SLOT(RestoreDefSlot()));
    connect(mApplyBtn, SIGNAL(clicked()), this, SLOT(ApplySlot()));
    connect(mCancelBtn, SIGNAL(clicked()), this, SLOT(CancelSlot()));
    connect(mOkBtn, SIGNAL(clicked()), this, SLOT(OkSlot()));

    connect(mUseHighscoresCb, SIGNAL(stateChanged(int)), this, SLOT(UseHighscoresChangedSlot(int)));
}

void PrefDialog::InitSettings()
{
    mSettings = GetSettings();
    SetSettings();
}

void PrefDialog::SetSettings()
{
    bool sup = SetSuppressSlots(true);

    mMenuBarCb->setChecked(mSettings->mShowMenuBar);
    mStatusBarCb->setChecked(mSettings->mShowStatusBar);
    mToolBarCb->setChecked(mSettings->mShowToolBar);
    mShowGameNoCb->setChecked(mSettings->mShowGameNo);

    mShowTimerCb->setChecked(mSettings->mShowTimer);
    mShowTenthCb->setChecked(mSettings->mShowTenth);
    ShowTimerChangedSlot(mShowTimerCb->checkState());

    mShowIndicatorsCb->setChecked(mSettings->mShowIndicators);
    if (mSettings->mIndicatorType == Settings::INDICATOR_NUMBER)
    {
        mNumbersRb->setChecked(true);
    }
    else
    {
        mLettersRb->setChecked(true);
    }
    mHideColorsCb->setChecked(mSettings->mHideColors);

    int i;
    i = mColorCntCmb->findData(mSettings->mColorCnt);
    if (i != -1 && mColorCntCmb->currentIndex() != i)
    {
        mColorCntCmb->setCurrentIndex(i);
    }
    i = mPegCntCmb->findData(mSettings->mPegCnt);
    if (i != -1 && mPegCntCmb->currentIndex() != i)
    {
        mPegCntCmb->setCurrentIndex(i);
    }

    mDoublesCb->setChecked(mSettings->mDoubles);
    CheckDoubles();
    CheckLevelPresets();

    i = mGameModeCmb->findData(mSettings->mGameMode);
    if (i != -1 && mGameModeCmb->currentIndex() != i)
    {
        mGameModeCmb->setCurrentIndex(i);
    }

    i = mStrengthCmb->findData(mSettings->mSolverStrength);
    if (i != -1 && mStrengthCmb->currentIndex() != i)
    {
        mStrengthCmb->setCurrentIndex(i);
    }

    mAutoCloseCb->setChecked(mSettings->mAutoClose);
    mAutoHintsCb->setChecked(mSettings->mAutoHints);

    mDelaySb->setValue(mSettings->mHintsDelay);

    mGamesListMaxCntSb->setValue(mSettings->mGamesListMaxCnt);
    mUserNameLe->setText(mSettings->mHighScoreUserName);
    if (mSettings->mHighScoreHandling == Settings::HIGHSCORE_PROMPT)
    {
        mUseHighscoresCb->setChecked(true);
        mHighscorePromptRb->setChecked(true);
    }
    else if(mSettings->mHighScoreHandling == Settings::HIGHSCORE_AUTO)
    {
        mUseHighscoresCb->setChecked(true);
        mHighscoreAutoRb->setChecked(true);
    }
    else if (mSettings->mHighScoreHandling == Settings::HIGHSCORE_NO)
    {
        mUseHighscoresCb->setChecked(false);
    }

    mCustomRowHColorCb->setChecked(mSettings->mCustomRowHColor);
    CustomRowHColorChangedSlot(mCustomRowHColorCb->checkState());

    if (sup)
    {
        SetSuppressSlots(false);
        ShowIndicatorsChangedSlot(mShowIndicatorsCb->checkState());
        UseHighscoresChangedSlot(mUseHighscoresCb->checkState());
    }
}

void PrefDialog::LevelPresetChangedSlot()
{
    int ix, i;
    ix = mLevelPresetsCmb->itemData(mLevelPresetsCmb->currentIndex()).toInt();

    if (ix < 0 || ix > 4)
    {
        return;
    }

    bool sup = SetSuppressSlots(true);

    i = mColorCntCmb->findData(ColorCode::LEVEL_SETTINGS[ix][0]);
    if (i != -1 && mColorCntCmb->currentIndex() != i)
    {
        mColorCntCmb->setCurrentIndex(i);
    }
    i = mPegCntCmb->findData(ColorCode::LEVEL_SETTINGS[ix][1]);
    if (i != -1 && mPegCntCmb->currentIndex() != i)
    {
        mPegCntCmb->setCurrentIndex(i);
    }

    mDoublesCb->setChecked((ColorCode::LEVEL_SETTINGS[ix][2] == 1));

    if (sup)
    {
        SetSuppressSlots(false);
    }
}

void PrefDialog::CheckLevelPresets()
{
    int ix = -1;
    for (int i = 0; i < 5; ++i)
    {
        if ( ColorCode::LEVEL_SETTINGS[i][0] == mColorCntCmb->itemData(mColorCntCmb->currentIndex()).toInt()
             && ColorCode::LEVEL_SETTINGS[i][1] == mPegCntCmb->itemData(mPegCntCmb->currentIndex()).toInt()
             && ColorCode::LEVEL_SETTINGS[i][2] == (int) mDoublesCb->isChecked() )
        {
            ix = i;
            break;
        }
    }

    mLevelPresetsCmb->setCurrentIndex(mLevelPresetsCmb->findData(ix));
}

void PrefDialog::CheckDoubles()
{
    if (mColorCntCmb->itemData(mColorCntCmb->currentIndex()).toInt() < mPegCntCmb->itemData(mPegCntCmb->currentIndex()).toInt())
    {
        if (!mDoublesCb->isChecked())
        {
            mDoublesCb->setChecked(true);
        }
        mDoublesCb->setEnabled(false);
    }
    else
    {
        mDoublesCb->setEnabled(true);
    }
}

void PrefDialog::CustomRowHColorChangedSlot(int state)
{
    bool en = (state == Qt::Checked);
    mRowHColorFgBtn->setEnabled(en);
    mRowHColorBgBtn->setEnabled(en);
    mFgLbl->setEnabled(en);
    mBgLbl->setEnabled(en);

    QColor color;
    color = GetCurBtnColor(mRowHColorFgBtn);
    if (!en)
    {
        color = Desaturate(color);
    }
    if (color.isValid())
    {
        SetSwatchStyleSheet(mRowHColorFgBtn, color.name());
    }

    color = GetCurBtnColor(mRowHColorBgBtn);
    if (!en)
    {
        color = Desaturate(color);
    }
    if (color.isValid())
    {
        SetSwatchStyleSheet(mRowHColorBgBtn, color.name());
    }
}

QColor PrefDialog::Desaturate(QColor c)
{
    qreal h;
    qreal s;
    qreal v;
    qreal a;
    c.getHsvF(&h, &s, &v, &a);

    v = qMin((0.8 + v * 0.2), 1.0);
    c.setHsvF(h, (s * 0.2), v, 1.0);
    return c;
}

void PrefDialog::SetSwatchStyleSheet(QPushButton* btn, const QString colorstr)
{
    btn->setStyleSheet(QString("QPushButton { background-color: " + colorstr + "; border: 1px solid palette(dark) } QPushButton:hover { border: 1px solid palette(highlight) }"));
    btn->update();
}

void PrefDialog::SetBtnColorSlot()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    SetBtnColor(btn);
}

void PrefDialog::SetBtnColor(QPushButton* btn)
{
    if (mSettings == NULL || btn == NULL)
    {
        return;
    }

    QColor curcolor = GetCurBtnColor(btn);

    if (curcolor.isValid())
    {
        curcolor = QColorDialog::getColor(curcolor, this);
    }

    if (curcolor.isValid())
    {
        SetSwatchStyleSheet(btn, curcolor.name());

        if (btn == mRowHColorFgBtn)
        {
            mSettings->mRowHColorFg = curcolor.name();
        }
        else if (btn == mRowHColorBgBtn)
        {
            mSettings->mRowHColorBg = curcolor.name();
        }
    }
}

QColor PrefDialog::GetCurBtnColor(const QPushButton* btn)
{
    QColor color;
    
    if (mSettings != NULL)
    {
        if (btn == mRowHColorBgBtn)
        {
            color.setNamedColor(mSettings->mRowHColorBg);
        }
        else
        {
            color.setNamedColor(mSettings->mRowHColorFg);
        }
    }
    if (mSettings == NULL || !color.isValid())
    {
        if (btn == mRowHColorBgBtn)
        {
            color = QApplication::palette().color(QPalette::Active, QPalette::Button);
        }
        else
        {
            color = QApplication::palette().color(QPalette::Active, QPalette::ButtonText);
        }
    }

    return color;
}

void PrefDialog::ColorCntChangedSlot()
{
    if (!mSuppressSlots)
    {
        CheckDoubles();
        CheckLevelPresets();
    }
}

void PrefDialog::PegCntChangedSlot()
{
    if (!mSuppressSlots)
    {
        CheckDoubles();
        CheckLevelPresets();
    }
}

void PrefDialog::DoublesChangedSlot()
{
    if (!mSuppressSlots)
    {
        CheckDoubles();
        CheckLevelPresets();
    }
}

void PrefDialog::UseHighscoresChangedSlot(int state)
{
    if (!mSuppressSlots)
    {
        bool en = (state != Qt::Unchecked);
        mHighscorePromptRb->setEnabled(en);
        mHighscoreAutoRb->setEnabled(en);
        mUserNameLe->setEnabled(en);
        mUserNameLbl->setEnabled(en);
    }
}

void PrefDialog::ShowIndicatorsChangedSlot(int state)
{
    if (!mSuppressSlots)
    {
        bool en = (state != Qt::Unchecked);
        mLettersRb->setEnabled(en);
        mNumbersRb->setEnabled(en);
        mHideColorsCb->setEnabled(en);
    }
}

void PrefDialog::ShowTimerChangedSlot(int state)
{
    bool en = (state == Qt::Checked);
    mShowTenthCb->setEnabled(en);
}

void PrefDialog::ResetColorOrderSlot()
{
    emit ResetColorOrderSignal();
}

void PrefDialog::RestoreDefSlot()
{
    mSettings->RestoreDefSettings();
    SetSettings();
}

void PrefDialog::ApplySlot()
{
    ApplySettings();
    setResult(QDialog::Accepted);
    accept();
}

void PrefDialog::OkSlot()
{
    ApplySettings();
    setResult(QDialog::Accepted);
    accept();
}

void PrefDialog::CancelSlot()
{
    setResult(QDialog::Rejected);
    reject();
}

bool PrefDialog::SetSuppressSlots(bool b, bool force)
{
    if (mSuppressSlots == b && !force)
    {
        return false;
    }
    mSuppressSlots = b;
    return true;
}

void PrefDialog::ApplySettings()
{
    mSettings->mShowMenuBar = mMenuBarCb->isChecked();
    mSettings->mShowStatusBar = mStatusBarCb->isChecked();
    mSettings->mShowToolBar = mToolBarCb->isChecked();
    mSettings->mShowTimer = mShowTimerCb->isChecked();
    mSettings->mShowTenth = mShowTenthCb->isChecked();
    mSettings->mShowGameNo = mShowGameNoCb->isChecked();

    mSettings->mShowIndicators = mShowIndicatorsCb->isChecked();

    if (mNumbersRb->isChecked())
    {
        mSettings->mIndicatorType = Settings::INDICATOR_NUMBER;
    }
    else
    {
        mSettings->mIndicatorType = Settings::INDICATOR_LETTER;
    }
    mSettings->mHideColors = mHideColorsCb->isChecked();

    mSettings->mColorCnt = mColorCntCmb->itemData(mColorCntCmb->currentIndex()).toInt();
    mSettings->mPegCnt = mPegCntCmb->itemData(mPegCntCmb->currentIndex()).toInt();
    mSettings->mDoubles = mDoublesCb->isChecked();

    mSettings->mGameMode = mGameModeCmb->itemData(mGameModeCmb->currentIndex()).toInt();
    mSettings->mSolverStrength = mStrengthCmb->itemData(mStrengthCmb->currentIndex()).toInt();

    mSettings->mAutoClose = mAutoCloseCb->isChecked();
    mSettings->mAutoHints = mAutoHintsCb->isChecked();
    mSettings->mHintsDelay = mDelaySb->value();

    mSettings->mHighScoreUserName = mUserNameLe->text();
    mSettings->mGamesListMaxCnt = mGamesListMaxCntSb->value();
    if (mUseHighscoresCb->isChecked())
    {
        if (mHighscorePromptRb->isChecked())
        {
            mSettings->mHighScoreHandling = Settings::HIGHSCORE_PROMPT;
        }
        else
        {
            mSettings->mHighScoreHandling = Settings::HIGHSCORE_AUTO;
        }
    }
    else
    {
        mSettings->mHighScoreHandling = Settings::HIGHSCORE_NO;
    }

    mSettings->mCustomRowHColor = mCustomRowHColorCb->isChecked();
    mSettings->mRowHColorFg = GetCurBtnColor(mRowHColorFgBtn).name();
    mSettings->mRowHColorBg = GetCurBtnColor(mRowHColorBgBtn).name();

    mSettings->Validate();
}

QSize PrefDialog::sizeHint () const
{
    return QSize(500, 400);
}
