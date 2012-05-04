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

import QtQuick 1.1

Item {
    id: scrollBar

    property variant view: parent

    Rectangle {
        radius: 5
	color: "transparent"
        border.color: "white"
        border.width: 2
        opacity: 0.3
        y: view.visibleArea.yPosition * scrollBar.height
        width: parent.width
        height: view.visibleArea.heightRatio * scrollBar.height
	visible: view.visibleArea.heightRatio < 1.0
    }
}
