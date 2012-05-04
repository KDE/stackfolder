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
//#include <QProcess>
#include <Solid/Networking>
#include <plasma/applet.h>

class QModelIndex;
class QItemSelectionModel;
class QGraphicsLinearLayout;
class QDeclarativeView;
class KDirModel;
class KFilePlacesModel;
class ProxyModel;
class Dialog;
class Directory;
class IconWidget;
class Viewer;

class DirModel;

class StackFolder : public Plasma::Applet
{
    Q_OBJECT

public:
    StackFolder(QObject *parent, const QVariantList &args);
    ~StackFolder();

    void init();
    void configChanged();

protected:
    void constraintsEvent(Plasma::Constraints);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
    bool eventFilter(QObject *watched, QEvent *event);
    void timerEvent(QTimerEvent *event);

protected slots:
    void refreshIcons();

    void updateIconWidget();
    void iconWidgetClicked();
    void dataAdded(const QModelIndex &parent, int start, int end);
    void dialogHidden();
    void folderChanged(const KUrl& url);
    void fontSettingsChanged();
    void iconGeometryChanged();
    void themeChanged();
    void networkStatusChanged(Solid::Networking::Status status);
    void fileActivated();
    void runViewer(const QString &path, int x, int y, int width, int height);
    //void stopViewerProcess();
    //void deleteViewerProcess(int exitCode, QProcess::ExitStatus exitStatus);
    //void currentChanged(/*int index*/);

private:
    QColor textColor() const;
    void setUrl(const KUrl &url);
    QSize iconSize() const;
    void updateIconViewState();
    QSize sizeToFitIcons(const int count) const;
    void showDialog();
    void hideDialog();

private:
    Dialog *m_dialog;
    Directory *m_directory;
    QGraphicsWidget *m_graphicsWidget;
    QGraphicsLayoutItem *m_graphicsObject;
    QItemSelectionModel *m_selectionModel;
    ProxyModel *m_model;
    KDirModel *m_dirModel;
    KFilePlacesModel *m_placesModel;
    IconWidget *m_iconWidget;
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
//    IconView::Flow m_flow;
    int m_firstChangings;
    bool m_folderChanging;
    bool m_needShow;
};

#endif
