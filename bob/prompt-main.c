 
/*

    prompt-main.c

    Main command menu

*/

#include <stdio.h>
#include <stdlib.h>
#include "inc/bob.h"
#include "inc/prompt.h"



static void fn_usage(void);
static void fn_bob(void);
static void fn_state(void);
static void fn_diag(void);


// Main Menu LUT
static CMDINFO g_lutMain[] = {
	{ "bob", fn_bob, "Changes to the rendering menu mode" },
	{ "state", fn_state, "Changes to the internal state menu mode" },
	{ "diag", fn_diag, "Changes to the diagnostic menu mode" },
	{ "usage", fn_usage, "Show command line usage" },
	{ NULL, NULL, NULL }
};


// Start the main menu
void start_main_menu(void) {
    set_menu(g_lutMain, "Main Menu");
}


/*
    ================ COMMANDS ================
*/


// Show command line uage
static void fn_usage(void) {
	if (arg_count(0)) PrintUsage();
	else wrong_count(0);
}


// Start the BOB menu
static void fn_bob(void) {
	if (arg_count(0)) {
        start_bob_menu();
		return;
	}
	wrong_count(0);
}

// Start the state menu
static void fn_state(void) {
	if (arg_count(0)) {
        start_state_menu();
		return;
	}
	wrong_count(0);
}

// Start the diagnostics menu
static void fn_diag(void) {
	if (arg_count(0)) {
        start_diag_menu();
		return;
	}
	wrong_count(0);
}

