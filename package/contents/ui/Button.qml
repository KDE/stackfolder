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

import QtQuick 1.1

Image {
    id: button

    property string icon
    property int normalSize
    property int selectedSize
    signal clicked

    source: "image://mode/disabled/" + icon

    MouseArea {
	id: buttonMouseArea
	anchors.fill: parent
	hoverEnabled: true

	onClicked: {
	    button.clicked()
	}

	onPressed: {
	    if (button.state != "disabled")
		button.state = "selected"
	}

	onReleased: {
	    if (button.state != "disabled")
		if (button.containsMouse)
		    button.state = "active"
		else
		    button.state = "normal"
	}

	onEntered: {
	    if (button.state != "disabled"){
		if (buttonMouseArea.pressed)
		    button.state = "selected"
		else
		    button.state = "active"
	    }
	}

	onExited: {
	    if (button.state != "disabled")
		button.state = "normal"
	}

    }

    states: [
	State {
	    name: "disabled"
	    PropertyChanges {
		target: button
		source: "image://mode/disabled/" + icon
		width:  normalSize
		height: normalSize
	    }
	},
	State {
	    name: "normal"
	    PropertyChanges {
		target: button
		source: "image://mode/normal/" + icon
		width:  normalSize
		height: normalSize
	    }
	},
	State {
	    name: "active"
	    PropertyChanges {
		target: button
		source: "image://mode/active/" + icon
		width:  normalSize
		height: normalSize
	    }
	},
	State {
	    name: "selected"
	    PropertyChanges {
		target: button
		source: "image://mode/active/" + icon
		width:  selectedSize
		height: selectedSize
	    }
	}
    ]
}


