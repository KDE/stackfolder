/*
 *   Copyright © 2008 Fredrik Höglund <fredrik@kde.org>
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

#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>

namespace Plasma {
    class Applet;
    class FrameSvg;
}
class QGraphicsScene;
class QGraphicsView;
class QGraphicsWidget;
class QGraphicsObject;
class QDeclarativeView;
class QDeclarativeEngine;
//class QDeclarativeComponent;
class QDeclarativeContext;
class QDeclarativeItem;
class QVBoxLayout;

class IconProvider;
class Directory;
class Viewer;

class Dialog : public QWidget
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

    void setGraphicsWidget(QGraphicsWidget *widget);
    void setDeclarativeView(QDeclarativeView *view);
    void setViewer(Viewer *viewer);
    void show(Plasma::Applet *applet);

signals:
    void hidden();

protected:
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);

private:
    Plasma::FrameSvg *m_background;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QGraphicsWidget *m_widget;
    QGraphicsObject *m_object;
    QDeclarativeEngine *m_engine;
    //QDeclarativeComponent *m_component;
    QDeclarativeContext *m_context;
    QDeclarativeView *m_declView;
    QVBoxLayout *m_layout;
    Viewer *m_viewer;
};

#endif

