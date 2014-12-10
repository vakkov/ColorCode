#ifndef BUTTONSCELL_H
#define BUTTONSCELL_H

#include <QtWidgets>
#include <QWidget>
#include "cellbtn.h"

class ButtonsCell : public QWidget
{
    Q_OBJECT

    public:
        static const int ACTIONID_NONE;
        static const int ACTIONID_PLAY;
        static const int ACTIONID_SAVE;
        static const int ACTIONID_COPY;
        static const int ACTIONID_DELETE;

        ButtonsCell(QWidget* parent = 0, int hideBtn = 0);

        int GetActionId();

    signals:
        void CellButtonClickedSignal();

    protected slots:
        void ButtonClickedSlot();

    private:
        CellBtn *mDeleteBtn;
        CellBtn *mPlayBtn;
        CellBtn *mSaveBtn;
        CellBtn *mCopyBtn;

        QList<CellBtn*> mButtonsList;

        int mActionId;
};

#endif // BUTTONSCELL_H
