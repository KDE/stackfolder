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
import Effects 1.0
import org.kde.plasma.core 0.1 as PlasmaCore

Component {
    id: fileDelegate

    Item {
	id:fileItem
	width: GridView.view.cellWidth
	height: GridView.view.cellHeight

	Image {
	    id: icon
	    source: "image://type/"+iconName
	    anchors.top: parent.top
	    anchors.topMargin: 10
	    anchors.horizontalCenter: parent.horizontalCenter 
	}

	Text {
	    id:fileName
	    width: parent.width - 10
	    anchors.top: icon.bottom
	    anchors.topMargin: 2
	    anchors.horizontalCenter: parent.horizontalCenter
	    text: name
	    color: theme.textColor
	    font.bold: true
	    elide: Text.ElideRight
	    wrapMode: Text.Wrap
	    maximumLineCount: 2
	    horizontalAlignment: Text.AlignHCenter
	    /*
            effect: DropShadow {
                    blurRadius: 3
                    color: Qt.rgba(0, 0, 0, 1)
                    xOffset: 0.3
                    yOffset: 0.7
            }
            */
	}

	MouseArea {
	    id: fileMouseArea
	    anchors.fill: parent
	    hoverEnabled: true

	    onClicked: {
		directory.activate(index)
	    }
	    onEntered: {
		parent.GridView.view.hoveredIndex = index
		parent.GridView.view.hoveredItem = fileItem
	    }
	    onExited: {
		parent.GridView.view.hoveredIndex = -1
	    }

        onPressAndHold: {
            directory.activateDragAndDrop(index)
        }

        onPositionChanged: {
            if ( pressed && pressedButtons == Qt.LeftButton ) {
                directory.activateDragAndDrop(index) // stops until the end of dragging
            }
        }
	}

	Connections {
	    target: directory.files[index]
	    onPixmapChanged: {
		//console.log("onPixmapChanged index="+index+" name="+name);
		icon.source = "image://preview/"+directory.path+name
		icon.anchors.topMargin =  10 + (32 - icon.sourceSize.height/2)
		fileName.anchors.topMargin =  2 + (32 - icon.sourceSize.height/2)
	    }
	}

    }
}
