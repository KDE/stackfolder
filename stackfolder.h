/*
 *   Copyright © 2011 ROSA Lab. <support@rosalab.ru>
 *   Copyright © 2008, 2009 Fredrik Höglund <fredrik@kde.org>
 *   Copyright © 2008 Rafael Fernández López <ereslibre@kde.org>
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

#ifndef STACKFOLDER_H
#define STACKFOLDER_H

#include <QAbstractItemView>
#include <QBasicTimer>
#include <KIcon>
#include <plasma/popupapplet.h>

class QModelIndex;
class QItemSelectionModel;
class QGraphicsLinearLayout;
class QDeclarativeView;
class QSequentialAnimationGroup;
class QPropertyAnimation;
class KDirModel;
class KFilePlacesModel;
class ProxyModel;
class Dialog;
class Directory;
class IconWidget;
class Viewer;
class KFileItem;

class DirModel;

class StackFolder : public Plasma::PopupApplet
{
    Q_OBJECT
    Q_PROPERTY(QSizeF popupIconSize READ popupIconSize WRITE setPopupIconSize )

public:
    StackFolder(QObject *parent, const QVariantList &args);
    ~StackFolder();

    QGraphicsWidget *graphicsWidget();

    void init();
    void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contents);
    void configChanged();

    QSizeF popupIconSize() const;
    void setPopupIconSize(QSizeF size);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void timerEvent(QTimerEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void popupEvent(bool show);

protected slots:
    void refreshIcons();

    void updateIconWidget();
    void dataAdded(const QModelIndex &parent, int start, int end);
    void folderChanged(const KUrl& url);
    void fontSettingsChanged();
    void themeChanged();
    void fileActivated();
    void runViewer(const QString &path, int x, int y, int width, int height);
    void stopViewer();
    void activatedDragAndDrop(const KFileItem &item);
    void iconAnimationFinished();

private:
    QColor textColor() const;
    void setUrl(const KUrl &url);
    QSize sizeToFitIcons(const int count) const;
    void startAnimation();
    void stopAnimation();

private:
    Directory *m_directory;
    QGraphicsWidget *m_graphicsWidget;
    QItemSelectionModel *m_selectionModel;
    ProxyModel *m_model;
    KDirModel *m_dirModel;
    KFilePlacesModel *m_placesModel;
    KIcon m_icon;
    QDeclarativeView *m_declView;
    QGraphicsLinearLayout *m_layout;
    KUrl m_url;
    KUrl m_topUrl;
    KUrl m_downloadUrl;
    QColor m_textColor;
    QBasicTimer m_delayedShowTimer;
    Viewer* m_viewer;
    int m_filterType;
    int m_sortColumn;
    bool m_sortDirsFirst;
    bool m_showPreviews;
    bool m_drawShadows;
    bool m_iconsLocked;
    bool m_hideForChangeFolder;
    QStringList m_previewPlugins;
    int m_customIconSize;
    int m_numTextLines;
    int m_firstChangings;
    bool m_folderChanging;
    bool m_needShow;
    bool m_hoverShow;
    bool m_hoverState;
    QSizeF m_popupIconSize;
    QSequentialAnimationGroup *m_iconAnimationGroup;
    QPropertyAnimation *m_iconAnimation1;
    QPropertyAnimation *m_iconAnimation2;

};

#endif
