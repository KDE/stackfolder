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

#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class TypeImageProvider : public QDeclarativeImageProvider
{
public:
    TypeImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};


class ModeImageProvider : public QDeclarativeImageProvider
{
public:
    ModeImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

class PreviewGenerator;

class PreviewImageProvider : public QDeclarativeImageProvider
{
public:
    PreviewImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
private:
    PreviewGenerator *previewGenerator;
};

#endif //IMAGEPROVIDER_H
