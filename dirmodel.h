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

#ifndef DIRMODEL_H
#define DIRMODEL_H

#include <KDirModel>

class DirModel : public KDirModel
{
    Q_OBJECT

public:
    DirModel(QObject *parent = 0);
    ~DirModel();

    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
};

#endif

