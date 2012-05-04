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

#include "iconwidget.h"

#include <QIcon>
#include <QString>
#include <QGraphicsItem>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>

IconWidget::IconWidget(const QIcon &icon, const QString &text, QGraphicsItem *parent)
    : Plasma::IconWidget(icon, text, parent),
    m_geometry(QRectF(0, 0, 0, 0))
{
    m_iconAnimation1 = new QPropertyAnimation(this, "geometry");
    m_iconAnimation1->setDuration(600);
    m_iconAnimation2 = new QPropertyAnimation(this, "geometry");
    m_iconAnimation1->setDuration(600);
    m_iconAnimationGroup = new QSequentialAnimationGroup;
    m_iconAnimationGroup->addAnimation(m_iconAnimation1);
    m_iconAnimationGroup->addAnimation(m_iconAnimation2);
    m_iconAnimationGroup->setLoopCount(4);
}

IconWidget::~IconWidget()
{
    delete m_iconAnimationGroup;
    delete m_iconAnimation1;
    delete m_iconAnimation2;
}

void IconWidget::startAnimation()
{
    stopAnimation();

    m_geometry = geometry();
    qreal left1 = m_geometry.left();
    qreal top1 = m_geometry.top();
    qreal width1 = m_geometry.width();
    qreal height1 = m_geometry.height();
    qreal left2 = left1 + width1/8;
    qreal top2 = top1 + height1/8;
    qreal width2 = width1*3/4;
    qreal height2 = height1*3/4;

    //qDebug() << "Icon::startAnimation" << left1 << top1 << width1 << height1;
    //qDebug() << "Icon::startAnimation" << left2 << top2 << width2 << height2;

    m_iconAnimation1->setStartValue(QRectF(left1, top1, width1, height1));
    m_iconAnimation1->setEndValue(QRectF(left2, top2, width2, height2));
    m_iconAnimation2->setStartValue(QRectF(left2, top2, width2, height2));
    m_iconAnimation2->setEndValue(QRectF(left1, top1, width1, height1));

    m_iconAnimationGroup->start();

}

void IconWidget::stopAnimation()
{
    if (m_iconAnimationGroup->state() != QAbstractAnimation::Stopped) {
	m_iconAnimationGroup->stop();
	setGeometry(m_geometry);
    }
}

void IconWidget::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    stopAnimation();
    Plasma::IconWidget::hoverEnterEvent(event);
}
