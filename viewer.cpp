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

#include "viewer.h"

#include <QDebug>

Viewer::Viewer(QObject *parent)
    : QObject(parent),
      m_process(0)
{

}

Viewer::~Viewer()
{

}

void Viewer::run(const QString &path, int x, int y, int width, int height)
{
    //qDebug() << "Viewer::run : m_process=" << bool(m_process);
    if (!m_process) {

	//m_container = new QX11EmbedContainer;
	//m_container->show();
	QString executable;
	QStringList paramList;

	m_process = new QProcess(/*m_container*/);

	executable = "killall";
	paramList << "klook";
	m_process->start(executable, paramList);

    	if (!m_process->waitForFinished(3000)) {
    	    qDebug() << "Viewer::run(): Could not stop Klook";
    	    return;
    	}

	connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), /*m_process*/this, SLOT(/*deleteLater*/erase(int, QProcess::ExitStatus)));

	executable = "klook";
	//QString executable("kwrite");
	paramList.clear();
	paramList << "--nofork";
	paramList << "--embedded" << path;
	paramList << "-x" << QString::number(x);
	paramList << "-y" << QString::number(y);
	paramList << "-w" << QString::number(width);
	paramList << "-h" << QString::number(height);

	//paramList << QString::number(m_container->winId());

	m_process->start(executable, paramList);
	//qDebug() << "Viewer::run(): " << paramList;
	if (!m_process->waitForStarted(3000)) {
    	    qDebug() << "Viewer::run(): Could not start Klook";
    	    return;
	}
    }

}

void Viewer::stop()
{
    if (m_process) {
	//qDebug() << "Viewer::stop:  process state" << m_process->state();
	if (m_process->state() == QProcess::Running) {
    	    m_process->terminate();
    	    m_process->waitForFinished(3000);
	}
    }
}

void Viewer::erase(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    if (m_process) {
	delete m_process;
	//delete m_container;
	m_process = 0;
    }
}

bool Viewer::isRun()
{
    return m_process;
}
