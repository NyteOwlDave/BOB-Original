
/*

    prompt-state.c

    State command menu

*/

#include <stdio.h>
#include <stdlib.h>
#include "inc/bob.h"
#include "inc/prompt.h"


static void fn_bgnd(void);
static void fn_ray_vars(void);
static void fn_screen_vars(void);
static void fn_session_vars(void);
static void fn_studio_vars(void);
static void fn_light_vars(void);
static void fn_scene_vars(void);
static void fn_surface_vars(void);
static void fn_transform_vars(void);
static void fn_misc_vars(void);
static void fn_slab_vars(void);
static void fn_clip_vars(void);
static void fn_search_paths(void);


// State Menu LUT
static CMDINFO g_lutState[] = {
    { "bgnd", fn_bgnd, "List background variables"},
    { "ray", fn_ray_vars, "List ray traceing variables"},
    { "screen", fn_screen_vars, "List screen variables"},
    { "session", fn_session_vars, "List session variables"},
    { "studio", fn_studio_vars, "list studio variables"},
    { "light", fn_light_vars, "List lights and variables"},
    { "scene", fn_scene_vars, "List primitive and scene variables"},
    { "surface", fn_surface_vars, "List surfaces and variables"},
    { "transform", fn_transform_vars, "List transforms"},
    { "slab", fn_slab_vars, "List slabs"},
    { "clip", fn_clip_vars, "List clips"},
    { "path", fn_search_paths, "List search paths"},
    { "misc", fn_misc_vars, "List miscellaneous variables"},
	{ NULL, NULL, NULL }
};


// Start the state menu
void start_state_menu(void) {
    set_menu(g_lutState, "State Menu");
}

 
/*
    ================ COMMANDS ================
*/


// List background variables
static void fn_bgnd(void) {
	if (arg_count(0)) {
        print_bgnd();
		return;
	}
	wrong_count(0);
}


// List ray tracer variables
static void fn_ray_vars(void) {
	if (arg_count(0)) {
        print_ray_vars();
		return;
	}
	wrong_count(0);
}


// List screen variables
static void fn_screen_vars(void) {
	if (arg_count(0)) {
        print_screen_vars();
		return;
	}
	wrong_count(0);
}


// List session variables
static void fn_session_vars(void) {
	if (arg_count(0)) {
        print_session_vars();
		return;
	}
	wrong_count(0);
}


// List studio variables
static void fn_studio_vars(void) {
	if (arg_count(0)) {
        print_studio_vars();
		return;
	}
	wrong_count(0);
}


// List lights and variables
static void fn_light_vars(void) {
	if (arg_count(0)) {
        print_light_vars();
		return;
	}
	wrong_count(0);
}


// List primitives and scene variables
static void fn_scene_vars(void) {
	if (arg_count(0)) {
        print_scene_vars();
		return;
	}
	wrong_count(0);
}


// List surfaces and variables
static void fn_surface_vars(void) {
	if (arg_count(0)) {
        print_surface_vars();
		return;
	}
	wrong_count(0);
}


// List transforms
static void fn_transform_vars(void) {
	if (arg_count(0)) {
        print_transform_vars();
		return;
	}
	wrong_count(0);
}


// List miscellaneous variables
static void fn_misc_vars(void) {
	if (arg_count(0)) {
        print_misc_vars();
		return;
	}
	wrong_count(0);
}


// List slab variables
static void fn_slab_vars(void) {
	if (arg_count(0)) {
        print_slab_vars();
		return;
	}
	wrong_count(0);
}


// List clips
static void fn_clip_vars(void) {
	if (arg_count(0)) {
        print_clip_vars();
		return;
	}
	wrong_count(0);
}


// List paths
static void fn_search_paths(void) {
	if (arg_count(0)) {
        print_search_paths();
		return;
	}
	wrong_count(0);
}
