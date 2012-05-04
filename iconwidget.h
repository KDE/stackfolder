/*   StackFolder
 *   Copyright © 2012 ROSA  <support@rosalab.ru>
 *   License: GPLv3+
 *   Authors: Ural Mullabaev <ural.mullabaev@rosalab.ru>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 3,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <Plasma/IconWidget>

class QIcon;
class QGraphicsItem;
class QSequentialAnimationGroup;
class QPropertyAnimation;

class IconWidget : public Plasma::IconWidget
{
    Q_OBJECT

public:
    IconWidget(const QIcon &icon, const QString &text, QGraphicsItem *parent = 0);
    ~IconWidget();

    void startAnimation();
    void stopAnimation();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

private:
    QSequentialAnimationGroup *m_iconAnimationGroup;
    QPropertyAnimation *m_iconAnimation1;
    QPropertyAnimation *m_iconAnimation2;
    QRectF m_geometry;
};

#endif //ICONWIDGET_H

