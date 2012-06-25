/*   StackFolder
 *   Copyright (c) 2011-2012 ROSA  <support@rosalab.ru>
 *   Authors: Julia Mineeva <julia.mineeva@osinit.ru>,
 *   	      Evgeniy Auzhin <evgeniy.augin@osinit.ru>,
 *   	      Sergey Borovkov <sergey.borovkov@osinit.ru>,
 *   	      Ural Mullabaev <ural.mullabaev@rosalab.ru>
 *   License: GPLv3
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

#ifndef PREVIEWGENERATOR_H
#define PREVIEWGENERATOR_H

#include <QtCore/QObject>
#include <QtCore/QHash>


#include <kio/previewjob.h>
#include <kfileitem.h>
#include <QDeclarativeEngine>
#include <QtCore/QPair>

class Image;
class Directory;
//U class FileModel;

class PreviewGenerator : public QObject
{
    Q_OBJECT

public:
    //PreviewGenerator(QObject *parent);
    ~PreviewGenerator();

    static PreviewGenerator *createInstance();
    //void setFiles(const QStringList& list);

    QPixmap getPreviewPixmap(QString filePath);
//    void setModel(Directory *model);
    void setPlugins(const QStringList &plugins);
//U    void setModel(FileModel *model);
    /// Return true if there is generated non-default preview for file specified by path
    bool hasPreviewPixmap(const QString &file_path) const;

public slots:
    void start(/*const QStringList &list*/ Directory* model, const KFileItemList &fileList);
    //void stop();

private slots:
    void setPreview(const KFileItem&, const QPixmap&);
    void deleteJob(KJob *);

private:
    explicit PreviewGenerator(QObject *parent = 0);

    void notifyModel(const QString& filePath);

    QHash<QString, QPixmap> previews;
    QPixmap defaultPreview;
    Directory *m_model;
//U    FileModel *m_model;

    static PreviewGenerator *instance;
    QPixmap videoPixmap;


    QList<KJob*> m_previewJobs;
    QHash<KJob*, Directory *> m_models;
    //KIO::PreviewJob *m_job;
    QStringList m_plugins;
    //KFileItemList m_fileList;
};

#endif // PREVIEWGENERATOR_H
