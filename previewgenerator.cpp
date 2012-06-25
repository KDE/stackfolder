/*   StackFolder
 *   Copyright (c) 2011-2012 ROSA  <support@rosalab.ru>
 *   License: GPLv3
 *   Authors: Julia Mineeva <julia.mineeva@osinit.ru>,
 *   	      Evgeniy Auzhin <evgeniy.augin@osinit.ru>,
 *   	      Sergey Borovkov <sergey.borovkov@osinit.ru>,
 *   	      Ural Mullabaev <ural.mullabaev@rosalab.ru>
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

#include "previewgenerator.h"
#include "directory.h"
//U #include "filemodel.h"

#include <QtCore/QStringList>
#include <QtCore/QRect>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtCore/QtDebug>
#include <QtCore/QFileInfo>
#include <kfile.h>


PreviewGenerator *PreviewGenerator::instance = 0;

PreviewGenerator::PreviewGenerator(QObject *parent) :
    QObject(parent),/* //U, m_model(0)*/ /*, m_job(0)*/
    m_previewJobs()

{
    qDebug() << "PreviewGenerator::PreviewGenerator";
    defaultPreview.load(":images/pla-empty-box.png");
    videoPixmap.load(":images/play-empty.png");
    m_plugins = KIO::PreviewJob::availablePlugins();
}

PreviewGenerator::~PreviewGenerator()
{
    foreach (KJob* job, m_previewJobs) {
        Q_ASSERT(job != 0);
        job->kill();
    }
    m_previewJobs.clear();
}
/*
void PreviewGenerator::setModel(Directory *model)
{
    m_model = model;
}
*/
void PreviewGenerator::setPlugins(const QStringList &plugins)
{
    m_plugins = plugins;
}

void PreviewGenerator::notifyModel(const QString& filePath)
{
    Q_UNUSED(filePath);

/* //U
    if(m_model)
    {
        for(int i = 0; i < m_model->rowCount(); i++)
        {
            QModelIndex index = m_model->indexFromRowNumber(i);
            if(m_model->data(index, ListItem::FilePathRole).toString() == filePath)
            {
                m_model->refreshRow(index);
            }
        }
    }
*/
}

void PreviewGenerator::setPreview(const KFileItem &item, const QPixmap &pixmap)
{
    //qDebug("PreviewGenerator::setPreview  %s", item.localPath().toAscii().data());
    QPixmap pict = pixmap;
    //m_fileList.removeAll(item);
    if(item.mimetype().startsWith("video/"))
    {
        QPainter p(&pict);
        QPixmap scaledPixmap = videoPixmap.scaled(pict.width()/2, pict.height()/2,  Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap(pict.width()/2 - scaledPixmap.width()/2, pict.height()/2 - scaledPixmap.height()/2 ,  scaledPixmap );
    }
    previews.insert(item.localPath(), pict);

    KJob *job = static_cast<KJob *>(sender());
    Directory *model;
    if(m_models.contains(job)) {
	model = m_models.value(job);
	if(model)
	    model->setPreview(item, pixmap);
    }
    //notifyModel(item.localPath());
}

void PreviewGenerator::deleteJob(KJob *job)
{
    const int index = m_previewJobs.indexOf(job);
    m_previewJobs.removeAt(index);
    m_models.remove(job);
    //job->kill();

/*    delete m_job;
    qDebug() << "DELETE";
    m_job = 0;
*/
}

QPixmap PreviewGenerator::getPreviewPixmap(QString filePath)
{
    if(previews.contains(filePath))
        return previews[filePath];

    return defaultPreview;
}

bool PreviewGenerator::hasPreviewPixmap(const QString &file_path) const {
    return previews.contains(file_path);
}

PreviewGenerator * PreviewGenerator::createInstance()
{
    if(!instance)
        instance = new PreviewGenerator;
    return instance;
}
/*
void PreviewGenerator::setFiles(const QStringList &list)
{
    //stop();
    m_fileList.clear();
    for(int i = 0; i < list.size(); i++)
    {
        KFileItem fileItem(KFileItem::Unknown, KFileItem::Unknown, list[i], false);
        m_fileList.append(fileItem);
    }
}
*/
/* //U 
void PreviewGenerator::setModel(FileModel *model)
{
    m_model = model;
}
*/
void PreviewGenerator::start(/*const QStringList &list*/ Directory* model, const KFileItemList &fileList)
{
/*
    if(m_fileList.isEmpty())
        return;

    if(m_job && m_job->isSuspended())
    {
        m_job->resume();
        return;
    }
    else if(m_job)
    {
        m_job->kill();
    }
*/
    //m_fileList.clear();
/*
    KFileItemList fileList;
    for(int i = 0; i < list.size(); i++)
    {
	//if(!previews.contains(list[i])) {
    	    KFileItem fileItem(KFileItem::Unknown, KFileItem::Unknown, list[i], false);
    	    fileList.append(fileItem);
	//    qDebug() << "PreviewGenerator::start, item=" << list[i];
    	//}
    }
*/
    //qDebug() << "PreviewGenerator::start, count=" << fileList.size();

    KIO::PreviewJob* job = KIO::filePreview(fileList, 1000, 0 , 0, 0, true, true, &m_plugins);
    job->setIgnoreMaximumSize();
    job->setAutoDelete(true);
    //job->setAutoDelete(false);
    connect(job, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)), SLOT(setPreview(const KFileItem&, const QPixmap&)));
    connect(job, SIGNAL(finished(KJob*)/*result()*/), SLOT(deleteJob(KJob*)));
    m_previewJobs.append(job);
    m_models.insert(job, model);
}
/*
void PreviewGenerator::stop()
{
    if(m_job )
       m_job->suspend();
}
*/
