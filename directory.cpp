/*   StackFolder
 *   Copyright © 2012 ROSA  <support@rosalab.ru>
 *   License: GPLv2+
 *   Authors: Ural Mullabaev <ural.mullabaev@rosalab.ru>
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

#include "directory.h"

#include <QAbstractItemView>

#include <KDirModel>
#include <KDirLister>
#include <KFilePreviewGenerator>
#include <KUrl>
#include <KDebug>
#include <krun.h>

#include "proxymodel.h"
#include "previewgenerator.h"

Directory::Directory(QObject *parent)
    : QObject(parent),
    m_folderIsEmpty(false),
    m_directoryChanging(false)
{
    m_previewGenerator = PreviewGenerator::createInstance();
    //m_previewGenerator->setModel(this);
}

void Directory::setModel(QAbstractItemModel *model)
{
    m_model = static_cast<ProxyModel*>(model);
    m_dirModel = static_cast<KDirModel*>(m_model->sourceModel());

    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(rowsInserted(QModelIndex,int,int)));
    //connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(rowsRemoved(QModelIndex,int,int)));
    //connect(m_model, SIGNAL(modelReset()), SLOT(modelReset()));
    //connect(m_model, SIGNAL(layoutChanged()), SLOT(layoutChanged()));
    //connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(dataChanged(QModelIndex,QModelIndex)));

    KDirLister *lister = m_dirModel->dirLister();
    connect(lister, SIGNAL(started(KUrl)), SLOT(listingStarted(KUrl)));
    connect(lister, SIGNAL(clear()), SLOT(listingClear()));
    connect(lister, SIGNAL(completed()), SLOT(listingCompleted()));
    connect(lister, SIGNAL(canceled()), SLOT(listingCanceled()));
    connect(lister, SIGNAL(showErrorMessage(QString)), SLOT(listingError(QString)));
    // Next connection is activated, because moving file from stackfolder to stackfolder doesn't cause notifications
    // from KDirLister, so we use this signal to update content of widget dialog
    connect(lister, SIGNAL(itemsDeleted(KFileItemList)), SLOT(listingDeleted(KFileItemList)));
}

QAbstractItemModel *Directory::model() const
{
    return m_model;
}

void Directory::setTopUrl(const KUrl &url)
{
    m_topUrl = url;
}
/*
KUrl &Directory::topUrl() const
{
    return m_topUrl;
}
*/
void Directory::setUrl(const KUrl &url)
{
    m_directoryChanging = true;

    m_url = url;
    m_dirModel->dirLister()->openUrl(url);
    //refresh();
}
/*
KUrl &Directory::url() const
{
    return m_url;
}
*/

QString Directory::path() const
{
    return m_url.path(KUrl::AddTrailingSlash);
}


bool Directory::isTopUrl() const
{
    return m_topUrl.equals(m_url, KUrl::CompareWithoutTrailingSlash);
}


int Directory::dirsCount() const
{
    return m_dirList.size();
}

void appendDirs(QDeclarativeListProperty<Directory> * property, Directory * dir)
{
    Q_UNUSED(property);
    Q_UNUSED(dir);
}

Directory* dirAt(QDeclarativeListProperty<Directory> * property, int index)
{
    return static_cast< QList<Directory *> *>(property->data)->at(index);
}

int dirsSize(QDeclarativeListProperty<Directory> * property) 
{
    return static_cast< QList<Directory *> *>(property->data)->size();
}

void clearDirsPtr(QDeclarativeListProperty<Directory> *property)
{
    return static_cast< QList<Directory *> *>(property->data)->clear();
}

QDeclarativeListProperty<Directory> Directory::dirs()
{
    //refresh();
    return QDeclarativeListProperty<Directory>( this, &m_dirList, &appendDirs, &dirsSize, &dirAt,  &clearDirsPtr );
}

int Directory::filesCount() const
{
    return m_fileList.size();
}

void appendFiles(QDeclarativeListProperty<File> * property, File * file)
{
    Q_UNUSED(property);
    Q_UNUSED(file);
}

File* fileAt(QDeclarativeListProperty<File> * property, int index)
{
    return static_cast< QList<File *> *>(property->data)->at(index);
}

int filesSize(QDeclarativeListProperty<File> * property) 
{
    return static_cast< QList<File *> *>(property->data)->size();
}

void clearFilesPtr(QDeclarativeListProperty<File> *property)
{
    return static_cast< QList<File *> *>(property->data)->clear();
}

QDeclarativeListProperty<File> Directory::files()
{
    //refresh();
    return QDeclarativeListProperty<File>( this, &m_fileList, &appendFiles, &filesSize, &fileAt,  &clearFilesPtr );
}

QString Directory::dirname() const
{
    return m_url.fileName(KUrl::IgnoreTrailingSlash);
}

QString Directory::filename() const
{
    return currentFile.name();
}

QString Directory::fileContent() const
{
    return m_fileContent;
}

void Directory::setDirname(const QString &str)
{
    Q_UNUSED(str);
}

void Directory::setFilename(const QString &str)
{
    if( str != currentFile.name() ) {
        currentFile.setName(str);
        emit filenameChanged();
    }
}

void Directory::setFileContent(const QString &str)
{
    if(str != m_fileContent){
        m_fileContent = str;
        emit fileContentChanged();
    }
}

void Directory::back()
{
    if(!isTopUrl()) {
	setUrl(m_url.upUrl());
    }
}

void Directory::activate(int index)
{
    const QModelIndex modelIndex = m_model->index(index, 0);
    const KFileItem item = m_model->itemForIndex(modelIndex);

    if (item.isDir()) {
	setUrl(item.url());
    }
    else if (item.isFile()) {
  	item.run();
  	emit fileActivated();
    }
}

void Directory::runViewer(int index, int x, int y, int width, int height)
{
    const QModelIndex modelIndex = m_model->index(index, 0);
    const KFileItem item = m_model->itemForIndex(modelIndex);
    const int margin  = 10;

    emit viewerRequested(item.url().path(), x + 2 * margin, y + 1 * margin, width, height);
}

void Directory::stopViewer()
{
    emit viewerCanceled();
}

void Directory::open()
{
    const QModelIndex index = m_model->indexForUrl(m_url);
    const KFileItem item = m_model->itemForIndex(index);
    if (m_errorMessage.isEmpty()) {
	item.run();
	emit fileActivated();
    }
}

void Directory::activateDragAndDrop(int index) {
    const QModelIndex modelIndex = m_model->index(index, 0);
    const KFileItem item = m_model->itemForIndex(modelIndex);
    emit activatedDragAndDrop(item);
}

void Directory::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if (!m_directoryChanging)
	emit dataAdded(parent, start, end);
}

void Directory::listingStarted(const KUrl &url)
{
    Q_UNUSED(url)
    if (!m_errorMessage.isEmpty() || m_folderIsEmpty) {
        m_errorMessage.clear();
        m_folderIsEmpty = false;
    }
}

void Directory::listingClear()
{
    //kDebug() << "Directory::listingClear";
    //clear();
    //emit directoryChanged();
}

void Directory::listingCompleted()
{
    refresh();
    m_directoryChanging = false;
    emit directoryChanged();
}

void Directory::listingCanceled()
{
    //kDebug() << "Directory::listingCanceled";
    //clear();
    //emit directoryChanged();
}

void Directory::listingError(const QString &message)
{
    //kDebug() << "Directory::listingError" << message;
    clear();
    m_directoryChanging = false;
    emit directoryChanged();
    m_errorMessage = message;
    emit errorReceived(message);
}

void Directory::listingDeleted(const KFileItemList &items)
{
    //kDebug() << "Directory::listingDeleted" << items;
    if (items.contains(m_dirModel->dirLister()->rootItem())) {
        const QString path = m_dirModel->dirLister()->url().toLocalFile();
        listingError(KIO::buildErrorString(KIO::ERR_DOES_NOT_EXIST, path));
    }

    if (!m_model->rowCount()) {
        m_folderIsEmpty = true;
    }

    // update content of widget dialog (window)
    refresh();
    emit directoryChanged();
}

void Directory::clear()
{
    m_fileList.clear();
}

void Directory::refresh()
{
    m_fileList.clear();
    m_previewFileList.clear();

    File * file;
    for(int i = 0; i < m_model->rowCount() ; i++) {
	QModelIndex index = m_model->index(i, 0);
	KFileItem item = m_model->itemForIndex(index);
        file = new File();
        file->setName(item.name());
        file->setIconName(item.iconName());
        //file->setPixmap(item.pixmap(64));
        file->setPath(path()+item.name());
        m_fileList.append(file);

	if (!item.isDir()) {
    	    m_previewFileList.append(item);
        }
    }
    m_previewGenerator->start(this, m_previewFileList);
}

void Directory::setPreview(const KFileItem &item, const QPixmap &pixmap)
{
    Q_UNUSED(pixmap);
    QModelIndex index = m_model->indexForUrl(item.url());
    //qDebug() << "PreviewGenerator::setPreview()  url=" << item.url() << " row=" << index.row();
    if (index.row() >= 0 && index.row() < m_fileList.count()) {
	File *file = m_fileList.at(index.row());
	file->setPixmap(pixmap);
	//emit gotPreview(index.row());
    }
}

void Directory::emitDialogHidden()
{
    emit dialogHidden();
}

void Directory::emitKeyPressed(const QKeyEvent *event)
{
    emit keyPressed(event->key(), event->modifiers());
}

void Directory::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_delayedPreviewTimer.timerId()) {
        m_delayedPreviewTimer.stop();
	m_previewGenerator->start(this, m_previewFileList);
    }
}
