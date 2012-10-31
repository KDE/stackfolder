/*   StackFolder
 *   Copyright © 2012 ROSA  <support@rosalab.ru>
 *   License: GPLv2+
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

#include "imageprovider.h"
#include "previewgenerator.h"

#include <KIcon>

TypeImageProvider::TypeImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{
}

QPixmap TypeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const int width = 64;
    const int height = 64;

    if (size)
        *size = QSize(width, height);
    //qDebug("TypeImageProvider::requestPixmap() %s", id.toAscii().data());
    return KIcon(id).pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                    	    requestedSize.height() > 0 ? requestedSize.height() : height);
};

ModeImageProvider::ModeImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{
}

QPixmap ModeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);

    const int width = 22;
    const int height = 22;

    if (size)
        *size = QSize(width, height);

    QStringList list = id.split("/");
    if (list.count() > 1) {
	QString str = list[0];
	QIcon icon = KIcon(list[1]);
	QIcon::Mode mode;
	//if (icon != null) {
	    if (str.contains("disabled"))
		mode = QIcon::Disabled;
	    else if (str.contains("active"))
		mode = QIcon::Active;
	    else if (str.contains("selected")) {
		mode = QIcon::Selected;
	    }
	    else
		mode = QIcon::Normal;

	    QPixmap pixmap = icon.pixmap(*size, mode);
	    //if (mode == QIcon::Selected)
		//pixmap = pixmap.scaled(*size * 0.9, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	    return pixmap;
	//}
    }
    return QPixmap();
};

PreviewImageProvider::PreviewImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap),
      previewGenerator(PreviewGenerator::createInstance())
{
}

QPixmap PreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const int width = 64;
    const int height = 64;

    if (size)
        *size = QSize(width, height);

    //qDebug("PreviewImageProvider::requestPixmap() %s", id.toAscii().data());

    return previewGenerator->getPreviewPixmap(id).scaled(requestedSize.width() > 0 ? requestedSize.width() : width,
                    	    requestedSize.height() > 0 ? requestedSize.height() : height,
                    	    Qt::KeepAspectRatio, Qt::SmoothTransformation);
};
