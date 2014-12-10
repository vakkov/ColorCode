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
#include "gamenodisplay.h"
#include "settings.h"

using namespace std;

GameNoDisplay::GameNoDisplay(QGraphicsItem* parent) : QGraphicsTextItem(parent)
{
    mFont = QFont("Arial", 18, QFont::Normal, false);
    mFont.setStyleHint(QFont::SansSerif);
    mFont.setPixelSize(24);
    setFont(mFont);
    setDefaultTextColor(QColor("#808183"));

    mUpdateRect = QRectF(0, 0, 600, 40);

    setTextWidth(600);
    setTransform(QTransform::fromScale(0.5, 0.5), true);
    //scale(0.5, 0.5);
}

void GameNoDisplay::ShowStr(QString str)
{
    str = QString("<center>") + str + QString("</center>");
    setHtml(str);
}
