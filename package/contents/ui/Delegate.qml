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

Component {
    id: fileDelegate

    Item {
	id:fileItem
	width: GridView.view.cellWidth
	height: GridView.view.cellHeight

	Item {
	    id: spacer1
	    anchors.top: parent.top
	    height: 10
	}

	Image {
	    id: icon
	    source: "image://type/"+iconName
	    anchors.top: spacer1.bottom
	    anchors.horizontalCenter: parent.horizontalCenter 
	}

	Item {
	    id: spacer2
	    anchors.top: icon.bottom
	    height: 2
	}

	Text {
	    id:fileName
	    width: parent.width - 10
	    anchors.top: spacer2.bottom
	    anchors.horizontalCenter: parent.horizontalCenter
	    text: name
	    color: "white"
	    font.bold: true
	    elide: Text.ElideRight
	    wrapMode: Text.Wrap
	    maximumLineCount: 2
	    horizontalAlignment: Text.AlignHCenter
	}

	MouseArea {
	    id: fileMouseArea
	    anchors.fill: parent
	    hoverEnabled: true

	    onClicked: {
		//console.log("onClicked")
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
		spacer1.height =  10 + (32 - icon.sourceSize.height/2)
		spacer2.height =  2 + (32 - icon.sourceSize.height/2)
	    }
	}

    }
}
