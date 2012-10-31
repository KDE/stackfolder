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

#ifndef VIEWER_H
#define VIEWER_H

#include <QProcess>

//class QX11EmbedContainer;

class Viewer : public QObject
{
    Q_OBJECT

public:
    Viewer(QObject *parent = 0);
    ~Viewer();

    void run(const QString &path, int x, int y, int width, int height);
    bool isRun();

public slots:
    void stop();

protected slots:
    void erase(int exitCode, QProcess::ExitStatus exitStatus);

private:
    //QX11EmbedContainer* m_container;
    QProcess* m_process;
};

#endif
