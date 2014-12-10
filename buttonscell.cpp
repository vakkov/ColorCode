#include "buttonscell.h"

const int ButtonsCell::ACTIONID_NONE   = 0;
const int ButtonsCell::ACTIONID_PLAY   = 1;
const int ButtonsCell::ACTIONID_SAVE   = 2;
const int ButtonsCell::ACTIONID_COPY   = 4;
const int ButtonsCell::ACTIONID_DELETE = 8;

ButtonsCell::ButtonsCell(QWidget* parent, int hideBtn) : QWidget(parent)
{
    setAutoFillBackground(false);

    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_AlwaysShowToolTips, true);

    mActionId = ACTIONID_NONE;

    QHBoxLayout* lay = new QHBoxLayout;
    lay->setSpacing(0);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setAlignment(Qt::AlignLeft);

    int hide_cnt = 0;
    
    if ((hideBtn & ACTIONID_PLAY) == 0)
    {
        mPlayBtn = new CellBtn(parent);
        mPlayBtn->setIcon(QIcon(":/img/document-new.png"));
        mPlayBtn->setToolTip(tr("Play this Game again"));
        mPlayBtn->setFixedSize(QSize(18, 18));
        mPlayBtn->setAutoRaise(true);
        connect(mPlayBtn, SIGNAL(clicked()), this, SLOT(ButtonClickedSlot()));

        lay->addWidget(mPlayBtn);
        lay->addSpacing(2);
        mButtonsList.append(mPlayBtn);
    }
    else
    {
        ++hide_cnt;
    }

    if ((hideBtn & ACTIONID_SAVE) == 0)
    {
        mSaveBtn = new CellBtn(parent);
        mSaveBtn->setIcon(QIcon(":/img/bookmark_add.png"));
        mSaveBtn->setToolTip(tr("Save this Game as Bookmark"));
        mSaveBtn->setFixedSize(QSize(18, 18));
        mSaveBtn->setAutoRaise(true);
        connect(mSaveBtn, SIGNAL(clicked()), this, SLOT(ButtonClickedSlot()));

        lay->addWidget(mSaveBtn);
        lay->addSpacing(2);
        mButtonsList.append(mSaveBtn);
    }
    else
    {
        ++hide_cnt;
    }

    if ((hideBtn & ACTIONID_COPY) == 0)
    {
        mCopyBtn = new CellBtn(parent);
        mCopyBtn->setIcon(QIcon(":/img/edit-copy.png"));
        mCopyBtn->setToolTip(tr("Copy Game Number to Clipboard"));
        mCopyBtn->setFixedSize(QSize(18, 18));
        mCopyBtn->setAutoRaise(true);
        connect(mCopyBtn, SIGNAL(clicked()), this, SLOT(ButtonClickedSlot()));

        lay->addWidget(mCopyBtn);
        lay->addSpacing(2);
        mButtonsList.append(mCopyBtn);
    }
    else
    {
        ++hide_cnt;
    }

    if ((hideBtn & ACTIONID_DELETE) == 0)
    {
        if (hide_cnt > 0)
        {
            lay->addSpacing(hide_cnt * 20);
        }

        mDeleteBtn = new CellBtn(parent);
        mDeleteBtn->setIcon(QIcon(":/img/edit-delete.png"));
        mDeleteBtn->setToolTip(tr("Remove from List"));
        mDeleteBtn->setFixedSize(QSize(18, 18));
        mDeleteBtn->setAutoRaise(true);
        connect(mDeleteBtn, SIGNAL(clicked()), this, SLOT(ButtonClickedSlot()));

        lay->addWidget(mDeleteBtn);
        mButtonsList.append(mDeleteBtn);
    }

    setLayout(lay);
}

int ButtonsCell::GetActionId()
{
    int aid = mActionId;
    mActionId = ACTIONID_NONE;
    return aid;
}

void ButtonsCell::ButtonClickedSlot()
{
    if (sender() == 0)
    {
        return;
    }

    CellBtn* btn = qobject_cast<CellBtn*>(sender());
    if (btn == mDeleteBtn)
    {
            mActionId = ACTIONID_DELETE;
    }
    else if (btn == mPlayBtn)
    {
            mActionId = ACTIONID_PLAY;
    }
    else if (btn == mSaveBtn)
    {
            mActionId = ACTIONID_SAVE;
    }
    else if (btn == mCopyBtn)
    {
            mActionId = ACTIONID_COPY;
    }
    emit CellButtonClickedSignal();
}
