
/*

    prompt-bob.c

    BOB command menu

*/

#include <stdio.h>
#include <stdlib.h>
#include "inc/bob.h"
#include "inc/prompt.h"

// BOB menu functions
static void fn_lang_menu(void);
static void fn_render(void);
static void fn_stats(void);


// BOB Menu LUT
static CMDINFO g_lutBOB[] = {
	{ "language", fn_lang_menu, "Got to the language menu" },
	{ "render", fn_render, "Renders the scene" },
	{ "stats", fn_stats, "Shows rendering statistics" },
	{ NULL, NULL, NULL }
};


// Start the BOB menu
void start_bob_menu(void) {
    set_menu(g_lutBOB, "BOB Menu");
}


/*
    ================ COMMANDS ================
*/


// Change to language menu mode
static void fn_lang_menu(void) {
	if (arg_count(0)) {
        start_lang_menu();
		return;
	}
	wrong_count(0);
}


// Renders the scene
static void fn_render(void) {
	if (arg_count(0)) {
        ShootScene();
		return;
	}
	wrong_count(0);
}


// Shows rendering statistics
static void fn_stats(void) {
	if (arg_count(0)) {
		statistics(0);
		return;
	}
	wrong_count(0);
}

