/*
 * ui.h
 *
 *  Created on: 9. jul. 2018
 *      Author: barkholt
 */

#ifndef NUKLEAR_DEMO_SDL_OPENGLES2_UI_H_
#define NUKLEAR_DEMO_SDL_OPENGLES2_UI_H_

struct UI_Main {
	struct UI_ButtonGroup* active;
};

struct UI_ButtonGroup {
	// Unique name of group
	char* name;

	int rowHeight;
	int width;

	// Index of selected button
	int selected;

	int numButtons;
	struct UI_Button* buttons;

	struct UI_ButtonGroup* parent;
};

struct UI_Button {
	char* text;
	struct UI_ButtonGroup* childGroup;
};

#endif /* NUKLEAR_DEMO_SDL_OPENGLES2_UI_H_ */
