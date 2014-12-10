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

#include "timedisplaybg.h"

TimeDisplayBg::TimeDisplayBg()
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    mUpdateRect = QRectF(0, 0, 220, 20);
    mRect = QRectF(65.5, 3.5, 89, 14);

    QLinearGradient solgrad(0, 1, 0, 15);
    solgrad.setColorAt(0.0, QColor("#9fa0a2"));
    solgrad.setColorAt(1.0, QColor("#8c8d8f"));
    mBgBrush = QBrush(solgrad);

    QLinearGradient framegrad(0, 1, 0, 15);
    framegrad.setColorAt(0.0, QColor("#4e4f51"));
    framegrad.setColorAt(1.0, QColor("#ebecee"));
    mFramePen = QPen(QBrush(framegrad), 1);
}

QRectF TimeDisplayBg::boundingRect() const
{
    return mUpdateRect;
}

void TimeDisplayBg::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    painter->setBrush(mBgBrush);
    painter->setPen(mFramePen);
    painter->drawRoundedRect(mRect, 7.5, 7.5);
}
