
/*

    prompt-lang.c

    Language command menu

*/

#include <stdio.h>
#include <stdlib.h>
#include "inc/bob.h"
#include "inc/prompt.h"


// Language Menu LUT
static CMDINFO g_lutLang[] = {
	{ NULL, NULL, NULL }
};


// Start the language menu
void start_lang_menu(void) {
    set_menu(g_lutLang, "Language Menu");
    incomplete("start_lang_menu");
}


/*
    ================ COMMANDS ================
*/
