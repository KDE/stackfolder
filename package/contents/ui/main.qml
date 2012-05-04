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

    Rectangle {
	id: main

	    width: 120
	    height: 170

	color: "transparent"
	anchors.fill: parent

	signal selectChanged()
	signal notifyRefresh()
        signal currentChanged(/*int index*/)

	onNotifyRefresh: {
	    fileView.model = directory.files;
	    dir.text = directory.dirname;
	}

	/*
	Component.onCompleted: {
	    plasmoid.setAspectRatioMode(IgnoreAspectRatio);
	    plasmoid.addEventListener ('ConfigChanged', configChanged);
	}

	function configChanged() {
	    var url = plasmoid.readConfig("url");
	    if (url != directory.url())
		directory.setUrl(url);
	}
	*/

	Connections {
	    target: directory
	    onDirectoryChanged: {
		//console.log("onDirectoryChanged");
		main.notifyRefresh();

		if (directory.isTopUrl)
		    backButton.state = "disabled"
		else
		    backButton.state = "normal"

		openButton.state = "normal"

		if (!directory.filesCount) {
		    errorMessage.text = ""/*qsTr("This folder is empty.")*/;
		    scrollBar.visible = false
		}
		else {
		    errorMessage.text = "";
		    scrollBar.visible = true
		}

		fileView.currentIndex = -1
		fileView.forceActiveFocus()

		scrollBar.visible
	    }

	    onErrorReceived: {
		//console.log("onErrorReceived");
		openButton.state = "disabled"
		errorMessage.text = message;
	    }

	    onDialogHidden: {
		fileView.currentIndex = -1 
	    }
	}

	Row {
	    id: caption
	    width:parent.width
	    height: 40
	    anchors.top: parent.top

	    Button {
		id: backButton
		icon: "go-previous"
		normalSize: 22
		selectedSize: 20
		width: normalSize
		height: normalSize
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		onClicked: {
		    directory.back()
		}
	    }

	    Text {
		id: dir
		width: parent.width - /*backButton.width - openButton.width*/ 44 - 20
		text: directory.dirname
		color: "white" //"#000000"
		font.pixelSize: font.pixelSize+2; font.bold: true
		horizontalAlignment: Text.AlignHCenter
		elide: Text.ElideMiddle
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		opacity: 0.5
	    }

	    Button {
		id: openButton
		icon: "system-file-manager"
		normalSize: 22
		selectedSize: 20
		width: normalSize
		height: normalSize
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		onClicked: {
		    directory.open()
		}
	    }
	}

	Component {
	    id: fileHighlight
	    Rectangle {
		id: highlightRect
		width: fileView.cellWidth
		height: fileView.cellHeight
		color: "lightsteelblue"
	    }
	}

	GridView {
	    id:fileView
	    width: parent.width
	    height: parent.height - caption.height
	    anchors.bottom: parent.bottom
	    anchors.horizontalCenter: parent.horizontalCenter
	    cellWidth: 120; cellHeight: 120
	    model: directory.files
	    delegate: Delegate {}
    	    cacheBuffer: 1000
	    clip: true
	    highlight: fileHighlight
	    //highlightFollowsCurrentItem: false
	    //highlightMoveDuration: 40
	    focus: true

	    property int hoveredIndex: -1
	    property variant hoveredItem

	    onCurrentIndexChanged: {
	    	//console.log("onCurrentIndexChanged")
                main.currentChanged(/*fileView.currentIndex*/)
	    }

	    ScrollBar {
    		id: scrollBar
    		width: 10;
    		height: parent.height - width
    		anchors.right: parent.right
	    }
	}

	Text {
	    id: errorMessage
	    text: ""
	    font.bold: true
	    color: "white" //"#000000"
	    elide: Text.ElideMiddle
	    horizontalAlignment: Text.AlignHCenter
	    anchors.verticalCenter: fileView.verticalCenter
	    anchors.horizontalCenter: fileView.horizontalCenter

	}

        Keys.onPressed: {
	    if ((event.key == Qt.Key_Return) || (event.key == Qt.Key_Enter)) {
		if (event.modifiers & Qt.AltModifier) {
		    //console.log("onAltEnterPressed")
		    directory.open()
		}
		else {
		    //console.log("onReturnPressed")
		    directory.activate(fileView.currentIndex)
		}
	    }
	    else if (event.key == Qt.Key_Backspace) {
	    	//console.log("onBackspacePressed")
		directory.back()
	    }
	    else if (event.key == Qt.Key_Space) {
		//console.log("onSpacePressed contentX="+fileView.contentX+" contentY="+fileView.contentY)
		if (fileView.currentIndex != -1)
		    directory.show(fileView.currentIndex, fileView.x + fileView.currentItem.x - fileView.contentX, fileView.y + fileView.currentItem.y - fileView.contentY, fileView.currentItem.width, fileView.currentItem.height)
		else
    		    if (fileView.hoveredIndex != -1)
			directory.show(fileView.hoveredIndex, fileView.x + fileView.hoveredItem.x - fileView.contentX, fileView.y + fileView.hoveredItem.y - fileView.contentY, fileView.hoveredItem.width, fileView.hoveredItem.height)
	    }
	    /*
	    else if (event.key == Qt.Key_Tab) {
		fileView.moveCurrentIndexRight()
	    }
	    else if (event.key == Qt.Key_Backtab) {
		fileView.moveCurrentIndexLeft()
	    }
	    */
	}
    }