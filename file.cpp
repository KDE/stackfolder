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

#include "file.h"

#include <QDebug>

File::File(QObject *parent) : QObject(parent)
{
    m_name = "";
    m_iconName = "";
    m_pixmap = 0;
    m_path = "";
}

QString File::name() const{
    return m_name;
}

QString File::iconName() const{
    return m_iconName;
}

QPixmap File::pixmap() const{
    return m_pixmap;
}

QString File::path() const{
    return m_path;
}


void File::setName(const QString &str){
    if(str != m_name){
        m_name = str;
        emit nameChanged();
    }
}

void File::setIconName(const QString &str){
    if(str != m_iconName){
        m_iconName = str;
        emit iconNameChanged();
    }
}

void File::setPixmap(const QPixmap &px){
    m_pixmap = px;
    emit pixmapChanged();
}


void File::setPath(const QString &str){
    if(str != m_path){
        m_path = str;
        emit pathChanged();
    }
}

