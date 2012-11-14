/*   StackFolder
 *   Copyright (c) 2011-2012 ROSA  <support@rosalab.ru>
 *   License: GPLv2+
 *   Authors: Julia Mineeva <julia.mineeva@osinit.ru>,
 *            Evgeniy Auzhin <evgeniy.augin@osinit.ru>,
 *            Sergey Borovkov <sergey.borovkov@osinit.ru>,
 *            Ural Mullabaev <ural.mullabaev@rosalab.ru>
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

#include "previewgenerator.h"
#include "directory.h"

#include <QtCore/QStringList>
#include <QtCore/QRect>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtCore/QtDebug>
#include <QtCore/QFileInfo>
#include <kfile.h>


PreviewGenerator *PreviewGenerator::instance = 0;

PreviewGenerator::PreviewGenerator(QObject *parent) :
    QObject(parent),
    m_previewJobs()

{
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

void PreviewGenerator::setPlugins(const QStringList &plugins)
{
    m_plugins = plugins;
}

void PreviewGenerator::notifyModel(const QString& filePath)
{
    Q_UNUSED(filePath);
}

void PreviewGenerator::setPreview(const KFileItem &item, const QPixmap &pixmap)
{
    //kDebug("PreviewGenerator::setPreview  %s", item.localPath().toAscii().data());
    QPixmap pict = pixmap;
    if(item.mimetype().startsWith("video/")) {
        QPainter p(&pict);
        QPixmap scaledPixmap = videoPixmap.scaled(pict.width()/2, pict.height()/2,  Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap(pict.width()/2 - scaledPixmap.width()/2, pict.height()/2 - scaledPixmap.height()/2 ,  scaledPixmap );
    }
    previews.insert(item.localPath(), pict);

    KJob *job = static_cast<KJob *>(sender());
    Directory *model;
    if(m_models.contains(job)) {
        model = m_models.value(job);
    if(model) {
        model->setPreview(item, pixmap);
    }
    }
}

void PreviewGenerator::deleteJob(KJob *job)
{
    const int index = m_previewJobs.indexOf(job);
    m_previewJobs.removeAt(index);
    m_models.remove(job);
}

QPixmap PreviewGenerator::getPreviewPixmap(QString filePath)
{
    if(previews.contains(filePath)) {
        return previews[filePath];
    }

    return defaultPreview;
}

bool PreviewGenerator::hasPreviewPixmap(const QString &file_path) const {
    return previews.contains(file_path);
}

PreviewGenerator * PreviewGenerator::createInstance()
{
    if(!instance) {
        instance = new PreviewGenerator;
    }
    return instance;
}
void PreviewGenerator::start(Directory* model, const KFileItemList &fileList)
{
    KIO::PreviewJob* job = KIO::filePreview(fileList, 1000, 0 , 0, 0, true, true, &m_plugins);
    job->setIgnoreMaximumSize();
    job->setAutoDelete(true);
    connect(job, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)), SLOT(setPreview(const KFileItem&, const QPixmap&)));
    connect(job, SIGNAL(finished(KJob*)), SLOT(deleteJob(KJob*)));
    m_previewJobs.append(job);
    m_models.insert(job, model);
}
