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

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "file.h"

#include <KUrl>
#include <KFileItemList>

//#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <QDeclarativeListProperty>
#include <QObject>

#include <QBasicTimer>

class QAbstractItemModel;
class QModelIndex;
class ProxyModel;
class KDirModel;
class KFileItem;
class KFileItemList;
//class KUrl;
class PreviewGenerator;

class Directory : public QObject {

    Q_OBJECT

    Q_PROPERTY(int filesCount READ filesCount)
    Q_PROPERTY(bool isTopUrl READ isTopUrl)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QDeclarativeListProperty<Directory> dirs READ dirs CONSTANT )
    Q_PROPERTY(QDeclarativeListProperty<File> files READ files CONSTANT )
    Q_PROPERTY(QString dirname READ dirname WRITE setDirname /*NOTIFY dirnameChanged*/)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString fileContent READ fileContent WRITE setFileContent NOTIFY fileContentChanged)

    public:
        Directory(/*ProxyModel *model, KUrl &url,*/ QObject *parent = 0);

	void setModel(QAbstractItemModel *model);
	QAbstractItemModel *model() const;
	void setTopUrl(const KUrl &url);
	//KUrl &topUrl() const;
	void setUrl(const KUrl &url);
	KUrl &url() const;
        bool isTopUrl() const;

	QString path() const;

        int dirsCount() const;
        int filesCount() const;
        QString dirname() const;
        QString filename() const;
        QString fileContent() const;
        QDeclarativeListProperty<Directory> dirs();
        QDeclarativeListProperty<File> files();

        void setDirname(const QString &str);
        void setFilename(const QString &str);
        void setFileContent(const QString &str);

	void setPreview(const KFileItem &item, const QPixmap &pixmap);
	void emitDialogHidden();

        Q_INVOKABLE void back();
        Q_INVOKABLE void activate(int index);
        Q_INVOKABLE void show(int index, int x, int y, int width, int height);
        Q_INVOKABLE void open();
        Q_INVOKABLE void activateDragAndDrop(int index);

    signals:
        void directoryChanged();
        void errorReceived(const QString &message);
        void filenameChanged();
        void fileContentChanged();
        void fileActivated();
        void gotPreview(int i);
        void dataAdded(const QModelIndex &parent, int start, int end);
        void dialogHidden();
        void showRequested(const QString &path, int x, int y, int width, int height);
        void activatedDragAndDrop(const KFileItem &file_item);

    protected:
	void timerEvent(QTimerEvent *event);

    protected slots:
	void rowsInserted(const QModelIndex &parent, int start, int end);
	void listingStarted(const KUrl &url);
	void listingClear();
	void listingCompleted();
	void listingCanceled();
	void listingError(const QString &message);
	void listingDeleted(const KFileItemList &items);

    private:
	ProxyModel *m_model;
	KDirModel *m_dirModel;
	KUrl m_url;
	KUrl m_topUrl;
        //QDir m_dir;
        QStringList m_dirDirs;
        QStringList m_dirFiles;
        File currentFile;

        QString m_fileContent;

        QList<Directory *> m_dirList;
        QList<File *> m_fileList;
        KFileItemList m_previewFileList;

	PreviewGenerator *m_previewGenerator;

	bool m_folderIsEmpty;
	QString m_errorMessage;

	bool m_directoryChanging;

    QBasicTimer m_delayedPreviewTimer;
    int m_previewRow;

        void clear();
        void refresh();

};

#endif
