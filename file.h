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

#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QString>
#include <QPixmap>

class File : public QObject{

    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

    public:
        File(QObject *parent = 0);

        QString name() const;
        QString iconName() const;
        QPixmap pixmap() const;
        QString path() const;
        void setName(const QString &str);
        void setIconName(const QString &str);
        void setPixmap(const QPixmap &px);
        void setPath(const QString &str);

    signals:
        void nameChanged();
        void iconNameChanged();
        void pixmapChanged();
        void pathChanged();

    private:
        QString m_name;
        QString m_iconName;
        QPixmap m_pixmap;
        QString m_path;
};

#endif