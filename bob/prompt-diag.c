
/*

    prompt-diag.c

    Diagnostics command menu

*/

#include <stdio.h>
#include <stdlib.h>
#include "inc/bob.h"
#include "inc/prompt.h"


// Diagnostics Menu LUT
static CMDINFO g_lutDiag[] = {
	{ NULL, NULL, NULL }
};


// Start the diagnostics menu
void start_diag_menu(void) {
    set_menu(g_lutDiag, "Diagnostics Menu");
    incomplete("start_diag_menu");
}


/*
    ================ COMMANDS ================
*/
