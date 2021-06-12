
/*
	prompt.c

	Interactive command prompt

*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <signal.h>
#include "inc/bob.h"
#include "inc/tokens.h"
#include "inc/prompt.h"


// Common menu functions
static void fn_help(void);
static void fn_main_menu(void);
static void fn_system(void);
static void fn_cls(void);
static void fn_exit(void);

// Common Menu LUT
static CMDINFO g_lutCommon[] = {
	{ "help", fn_help, "Show this help info" },
	{ "main", fn_main_menu, "Go to the main menu" },
	{ "exit", fn_exit, "Exit the console" },
	{ "cls", fn_cls, "Clear screen" },
	{ "cmd", fn_system, "Performs a generic system command" },
	{ NULL, NULL, NULL }
};


// Active LUT
static PCMDINFO g_lut = NULL;

// Return code from last_call()
static int g_retcode = 0;

// Set to true for verbose mode (shows more hints)
static BOOL g_verbose = FALSE;

// Main input buffer
static char g_input[MAX_TOKEN] = "";

// Token buffers
static char g_token[NUM_ARGS][MAX_TOKEN];
static int g_numTokens = 0;


// Set menu
void set_menu(PCMDINFO pMenu, LPCSTR title) {
    g_lut = pMenu;
    if (title) {
        Print("");
        Print(title);
    }
}


// Checks for an exact argument count (number of tokens - 1)
BOOL arg_count(int n) {
	return (n == (g_numTokens - 1));
}


// Merges token buffers back into main input buffer
// In other words, it reverses the split_tokens() function
void merge_tokens(int first) {
	LPSTR p = g_input;
	for (int i = first; i < g_numTokens; i++) {
		strcpy(p, g_token[i]);
		p += strlen(p);
		*p++ = 32;
	}
	if (p != g_input) --p;
	*p = 0;
	if (g_verbose) PrintText("Merged", g_input);
}


// Separates main input buffer into token buffers
// Thrashes main input buffer (g_input)
// Token count goes into g_numTokens, tokens into g_token
void split_tokens(void) {
#define WHITESPC " \t\n"
	LPSTR sub;
	g_numTokens = 0;
	sub = strtok(g_input, WHITESPC);
	while (sub) {
		if (strlen(sub) >= MAX_TOKEN) {
			PrintText("Token too long", sub);
			g_numTokens = 0;
			g_token[0][0] = 0;
			return;
		}
		strcpy(g_token[g_numTokens], sub);
		if (g_verbose) PrintText("Token", sub);
		g_numTokens++;
		if (g_numTokens == NUM_ARGS) {
			Print("Some extra token(s) were ignored");
			return;
		}
		sub = strtok(NULL, WHITESPC);
	}
}


// Dispatches the tokenized commands using the lookup table
void dispatch(void) {
	// Bummer... no tokens to work with
	if (!g_token[0][0]) return;
	// Look for command in common menu first
	int i = 0;
	while (g_lutCommon[i].func) {
		if (!strcmp(g_token[0], g_lutCommon[i].name)) {
			g_lutCommon[i].func();
			return;
		}
		++i;
	}
	// Now look for command in current menu
	i = 0;
	while (g_lut[i].func) {
		if (!strcmp(g_token[0], g_lut[i].name)) {
			g_lut[i].func();
			return;
		}
		++i;
	}
	// Bummer!
	PrintText("Unknown command", g_token[0]);
}


// Accept and tokenize user input
// This reads text from stdin to the input buffer (g_input)
// The text is tokenized, then dispatched (g_token, g_numTokens)
void prompt(void) {
	fprintf(stderr, ">> ");
	fgets(g_input, sizeof (g_input) - 1, stdin);
	if (!g_input[0]) return;
	split_tokens();
	dispatch();
}


// Fetch a token to a buffer
LPCSTR read_token(int token, LPSTR buffer) {
    if ((token < 0) || (token >= g_numTokens)) {
        *buffer = 0;
        return NULL;
    }
    strcpy(buffer, g_token[token]);
    return buffer;
}


// Control-C handler
// static void _cdecl onbreak(int sig) {
static void onbreak(int sig) {
	// Dummy
}


// Main console entry point
void InteractiveConsole() {
	signal(SIGINT, onbreak);
    g_numTokens = 1;
	start_main_menu();
	Print("\nType help for assistance\n");
	while (TRUE) {
		prompt();
	}
}


// Operation terminated (possibly due to error)
// so, save return_code and begin Interactive mode
void last_call(int return_code) {
	char sz[80];
    sprintf(sz, "Return code: %i", return_code);
    LogPrint( sz );
	g_retcode = return_code;
	if (console) InteractiveConsole();
    exit(return_code);
}


// Display an error message
void error(LPCSTR sz) {
	fprintf(stderr, "ERROR: %s\n", sz);
}


// Display wrong argument count message
void wrong_count(int n) {
	fprintf(stderr, "This command takes %i arguments.\n", n);
}


// Display function incomplete message
void incomplete(LPCSTR fname) {
	if (fname) {
		fprintf(stderr, "Incomplete function: %s\n", fname);
	}
	else {
		Print("This feature is not yet implemented.");
	}
}


/*
    ================ COMMANDS ================
*/


// Common Menu
// Exit the application
static void fn_exit(void) {
	if (arg_count(0)) exit(0);
	wrong_count(0);
}

// Common Menu
// This merges tokens back together and sends them
// to the Windows command shell.
// The first token is ignored.
static void fn_system(void) {
	merge_tokens(1);
	Shell(g_input);
}


// Common Menu 
// Show console help
static void fn_help(void) {
	static LPCSTR fmt = "%-20s %s\n";
	if (arg_count(0)) {
		Print("\nCommands:\n");
		// List common menu commands first
		for (int i = 0; g_lutCommon[i].func; i++) {
			if (g_lutCommon[i].name) {
				fprintf(stderr, fmt, g_lutCommon[i].name, g_lutCommon[i].desc);
			}
		}
		// List current menu commands also
		for (int i = 0; g_lut[i].func; i++) {
			if (g_lut[i].name) {
				fprintf(stderr, fmt, g_lut[i].name, g_lut[i].desc);
			}
		}
		Print("");
		return;
	}
	if (arg_count(1)) {
		OpenHelpFile(g_token[1]);
		return;
	}
	error("Syntax is help [topic].");
}


// Common Menu
// Change to main menu mode
static void fn_main_menu(void) {
	if (arg_count(0)) {
        start_main_menu();
		return;
	}
	wrong_count(0);
}


// Common menu
// Clears the console
static void fn_cls(void) {
    Cls();
}
