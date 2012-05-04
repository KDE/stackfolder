/*
 *   Copyright © 2008, 2009 Fredrik Höglund <fredrik@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#include "dialog.h"

#include <QApplication>
#include <QBitmap>
#include <QLayout>
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsScene>
#include <QDeclarativeView>
#include <QDeclarativeEngine> //q
#include <QDeclarativeComponent> //q
#include <QDeclarativeContext> //q
#include <QDeclarativeItem> //q

#include <KWindowSystem>

#include <Plasma/Applet>
#include <Plasma/FrameSvg>

#ifdef Q_WS_X11
#  include <QX11Info>
#  include <X11/Xlib.h>
#endif

#include "directory.h"
#include "imageprovider.h"
#include "viewer.h"

Dialog::Dialog(QWidget *parent)
    : QWidget(parent, Qt::Popup), 
    m_widget(0), 
    m_object(0),
    m_viewer(0)
{
    setAttribute(Qt::WA_TranslucentBackground);

#ifdef Q_WS_X11
    setAttribute(Qt::WA_X11NetWmWindowTypeDropDownMenu);

    if (!QX11Info::isCompositingManagerRunning()) {
        setAttribute(Qt::WA_NoSystemBackground);
    }

    Atom atom = XInternAtom(QX11Info::display(), "_KDE_SHADOW_OVERRIDE", False);
    XChangeProperty(QX11Info::display(), winId(), atom, atom, 32, PropModeReplace, 0, 0);
#endif

    KWindowSystem::setState(effectiveWinId(), NET::SkipTaskbar | NET::SkipPager);

    QPalette pal = palette();
    pal.setColor(backgroundRole(), Qt::transparent);
    setPalette(pal);

    m_background = new Plasma::FrameSvg(this);
    m_background->setImagePath("dialogs/background");

    m_scene = new QGraphicsScene(this);
    //m_scene->setFocus();

    m_view = new QGraphicsView(m_scene, this);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->viewport()->setAutoFillBackground(false);
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 0, 10, 0);
}

Dialog::~Dialog()
{
}

void Dialog::setGraphicsWidget(QGraphicsWidget *widget)
{
    m_widget = widget;
    m_scene->addItem(widget);
}

void Dialog::setDeclarativeView(QDeclarativeView *view)
{
    m_declView = view;
    m_layout->addWidget(view);
    setFocusProxy(view);
}

void Dialog::setViewer(Viewer *viewer)
{
    m_viewer = viewer;
}

void Dialog::show(Plasma::Applet *applet)
{
    Plasma::FrameSvg::EnabledBorders borders = Plasma::FrameSvg::AllBorders;
    m_background->setEnabledBorders(borders);

    int left   = m_background->marginSize(Plasma::LeftMargin);
    int top    = m_background->marginSize(Plasma::TopMargin);
    int right  = m_background->marginSize(Plasma::RightMargin);
    int bottom = m_background->marginSize(Plasma::BottomMargin);

    switch (applet->location())
    {
    case Plasma::BottomEdge:
        borders &= ~Plasma::FrameSvg::BottomBorder;
        bottom = qMin(bottom, 2);
        break;

    case Plasma::TopEdge:
        borders &= ~Plasma::FrameSvg::TopBorder;
        top = qMin(top, 2);
        break;

    case Plasma::LeftEdge:
        borders &= ~Plasma::FrameSvg::LeftBorder;
        left = qMin(left, 2);
        break;

    case Plasma::RightEdge:
        borders &= ~Plasma::FrameSvg::RightBorder;
        right = qMin(right, 2);
        break;

    default:
        break;
    }

    const QSize margin(left + right, top + bottom);
    QSize size = /*QSize(1000,400)*//*m_view->sizeHint()*/m_widget->preferredSize().toSize() + margin;
    QPoint pos = applet->popupPosition(size);
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(pos);
    //const QSize maxSize = availableGeometry.size();

    if (pos.y() < 0) {
        size.rheight() += pos.y();
        pos.ry() = 0;
    } else if (applet->location() == Plasma::TopEdge &&
               pos.y() + size.height() > availableGeometry.bottom()) {
        size.rheight() -= pos.y() + size.height() - availableGeometry.bottom();
    }

    if (pos.x() < 0) {
        size.rwidth() += pos.x();
        pos.rx() = 0;
    } else if (applet->location() == Plasma::LeftEdge &&
               pos.x() + size.width() > availableGeometry.right()) {
        size.rwidth() -= pos.x() + size.width() - availableGeometry.right();
    }

    m_background->setEnabledBorders(borders);
    //kDebug() << " ContentsMargins: left " << left << ", top" << top << ", right" << right << ", bottom" << bottom;
    setContentsMargins(left, top, right, bottom);

    m_widget->resize(size - margin);
    resize(size);
    move(pos);

    QWidget::show();
}

void Dialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    m_background->resizeFrame(rect().size());
    m_view->setGeometry(contentsRect());

#ifdef Q_WS_X11
    if (!QX11Info::isCompositingManagerRunning()) {
        setMask(m_background->mask());
    }
#endif
}

void Dialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(rect(), Qt::transparent);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    m_background->paintFrame(&p, event->rect(), event->rect());
}

void Dialog::mousePressEvent(QMouseEvent *event)
{
    if (!rect().contains(event->pos())) {
        hide();
    }
    QWidget::mousePressEvent(event);
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
    	event->accept();
        if (m_viewer && m_viewer->isRun()) {
    	    m_viewer->stop();
    	}
    	else {
    	    close();
    	}
    }
    else {
	event->ignore();
    }
    //QWidget::keyPressEvent(event);
}

void Dialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    emit hidden();
}

void Dialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

	//qDebug() << "Dialog::showEvent hasFocus=" << hasFocus();

    setFocus();

    QWidget::showEvent(event);
}
