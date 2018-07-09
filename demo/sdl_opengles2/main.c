/* nuklear - 1.40.8 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GLES2_IMPLEMENTATION
#include "../../nuklear.h"
#include "nuklear_sdl_gles2.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the include
 * and the corresponding function. */
#include "../style.c"
/*#include "../calculator.c"*/
/*#include "../overview.c"*/
/*#include "../node_editor.c"*/

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/

#include "ui.h"

/* Platform */
SDL_Window *win;
int displayWidth;
int displayHeight;
int running = nk_true;

struct UI_Main ui;

struct UI_ButtonGroup topLevelGroup;
struct UI_Button topLevelButtons[7];

struct UI_ButtonGroup loadSaveGroup;
struct UI_Button loadSaveButtons[3];

void UI_down(struct UI_ButtonGroup* group) {
	int newValue = group->selected + 1;

	if (newValue >= group->numButtons) {
		newValue = 0;
	}
	group->selected = newValue;

}

void UI_right(struct UI_ButtonGroup* group) {

	struct UI_Button* current = &group->buttons[group->selected];

	if (current->childGroup != NULL) {
		ui.active = current->childGroup;
	}
}

void UI_left(struct UI_ButtonGroup* group) {

	if (group->parent != NULL)
		ui.active = group->parent;
}

void UI_up(struct UI_ButtonGroup* group) {
	int newValue = group->selected - 1;

	if (newValue < 0) {
		newValue = group->numButtons - 1;
	}
	group->selected = newValue;
}

void drawGroup(struct nk_context *ctx, struct UI_ButtonGroup* group, float x, float centerY) {
	int rowHeight = group->rowHeight;
	int buttonsInGroup = group->numButtons;
	/* GUI */

	enum theme baseTheme = ui.active == group ? THEME_DARK : THEME_BLACK;

	set_style(ctx, baseTheme);

	struct UI_ButtonGroup* childToDraw = NULL;
	float childY = 0;
	int windowHeight = rowHeight * buttonsInGroup + buttonsInGroup * 2;
	float currentPosition = centerY - windowHeight / 2;
	if (nk_begin(ctx, group->name, nk_rect(x, currentPosition, group->width, windowHeight), NK_WINDOW_NO_SCROLLBAR)) {

		for (int i = 0; i < group->numButtons; i++) {
			if (i == group->selected) {
				set_style(ctx, THEME_RED);
				if (group->buttons[i].childGroup != NULL)
					childToDraw = group->buttons[i].childGroup;
				childY = currentPosition + rowHeight / 2;
			}
			nk_layout_space_begin(ctx, NK_STATIC, rowHeight, INT_MAX);
			nk_layout_space_push(ctx, nk_rect(0, 0, 180, 50));
			if (nk_button_label(ctx, group->buttons[i].text))
				fprintf(stdout, "button pressed\n");
			nk_layout_space_end(ctx);

			if (i == group->selected)
				set_style(ctx, baseTheme);

			currentPosition += rowHeight;
		}
	}
	nk_end(ctx);

	if (childToDraw != NULL) {
		drawGroup(ctx, childToDraw, x + group->width + 10, childY);
	}

}

static void MainLoop(void* loopArg) {

	struct nk_context *ctx = (struct nk_context *) loopArg;

	/* Input */
	SDL_Event evt;
	nk_input_begin(ctx);
	while (SDL_PollEvent(&evt)) {
		if (evt.type == SDL_QUIT)
			running = nk_false;
		else if (evt.type == SDL_KEYDOWN) {
			if (evt.key.keysym.sym == SDLK_UP) {
				UI_up(ui.active);
			} else if (evt.key.keysym.sym == SDLK_DOWN) {
				UI_down(ui.active);
			} else if (evt.key.keysym.sym == SDLK_RIGHT) {
				UI_right(ui.active);
			} else if (evt.key.keysym.sym == SDLK_LEFT) {
				UI_left(ui.active);
			}
		}
		nk_sdl_handle_event(&evt);
	}
	nk_input_end(ctx);

	drawGroup(ctx, &topLevelGroup, 0, displayHeight / 2);

	/* -------------- EXAMPLES ---------------- */
	/*calculator(ctx);*/
	/*overview(ctx);*/
	/*node_editor(ctx);*/
	/* ----------------------------------------- */

	/* Draw */
	{
		float bg[4];
		int win_width, win_height;
		nk_color_fv(bg, nk_rgb(28, 48, 62));
		SDL_GetWindowSize(win, &win_width, &win_height);
		glViewport(0, 0, win_width, win_height);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(bg[0], bg[1], bg[2], bg[3]);
		/* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
		 * with blending, scissor, face culling, depth test and viewport and
		 * defaults everything back into a default state.
		 * Make sure to either a.) save and restore or b.) reset your own state after
		 * rendering the UI. */
		nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
		SDL_GL_SwapWindow(win);
	}
}

int main(int argc, char* argv[]) {

	topLevelGroup.name = "Top";
	topLevelGroup.numButtons = 7;
	topLevelGroup.selected = 0;
	topLevelGroup.buttons = topLevelButtons;
	topLevelGroup.rowHeight = 70;
	topLevelGroup.width = 190;

	loadSaveGroup.name = "LoadSave";
	loadSaveGroup.numButtons = 3;
	loadSaveGroup.selected = 0;
	loadSaveGroup.buttons = loadSaveButtons;
	loadSaveGroup.rowHeight = 70;
	loadSaveGroup.width = 190;
	loadSaveGroup.parent = &topLevelGroup;

	memset(topLevelButtons, 0, sizeof(struct UI_Button) * topLevelGroup.numButtons);
	topLevelButtons[0].text = "Save & Load";
	topLevelButtons[0].childGroup = &loadSaveGroup;

	topLevelButtons[1].text = "Controls";
	topLevelButtons[2].text = "Instructions";
	topLevelButtons[3].text = "High Scores";
	topLevelButtons[4].text = "Friends";
	topLevelButtons[5].text = "Profile";
	topLevelButtons[6].text = "Settings";

	memset(loadSaveButtons, 0, sizeof(struct UI_Button) * loadSaveGroup.numButtons);
	loadSaveButtons[0].text = "Load";
	loadSaveButtons[1].text = "Save";
	loadSaveButtons[2].text = "Autosave";

	ui.active = &topLevelGroup;

	/* GUI */
	struct nk_context *ctx;
	SDL_GLContext glContext;
	/* SDL setup */
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS); // - do NOT init SDL on GL ES 2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	win = SDL_CreateWindow("Demo",
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);

	SDL_GetWindowSize(win, &displayWidth, &displayHeight);

	glContext = SDL_GL_CreateContext(win);

	/* OpenGL setup */
	glViewport(0, 0, displayWidth, displayHeight);

	ctx = nk_sdl_init(win);
	/* Load Fonts: if none of these are loaded a default font will be used  */
	/* Load Cursor: if you uncomment cursor loading please hide the cursor */
	{
		struct nk_font_atlas *atlas;
		nk_sdl_font_stash_begin(&atlas);
		struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 28, 0);
		/*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
		/*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
		/*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
		/*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
		/*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
		nk_sdl_font_stash_end();
		/*nk_style_load_all_cursors(ctx, atlas->cursors);*/
		nk_style_set_font(ctx, &droid->handle);
	}

	/* style.c */
	/*set_style(ctx, THEME_WHITE);*/
	set_style(ctx, THEME_RED);
	/*set_style(ctx, THEME_BLUE);*/
	/*set_style(ctx, THEME_DARK);*/

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
	emscripten_set_main_loop_arg(MainLoop, (void*)ctx, 0, nk_true);
#else
	while (running)
		MainLoop((void*) ctx);
#endif

	nk_sdl_shutdown();
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}

