/*
	lang-macro.c

	Macro management

	[ ] Done
*/

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "inc/bob.h"

#define MAC_CHECK(p) check(p,__FUNCTION__)

static void check(PMACROEX p, LPCSTR func) {
	ptrchk(p, func);
	if (!p->name < 0) {
		fprintf(stderr, "Macro missing name in %s(). Dying...\n", func);
		last_call(1);
	}
	if (!p->name->data) {
		fprintf(stderr, "Macro name missing data buffer in %s(). Dying...\n", func);
		last_call(1);
	}
}

// Macro table
static MacroEx g_table = {
	NULL,	// No name
	NULL,	// No text
	NULL	// No head macro
};

static PMACROEX alloc_macro(LPCSTR name) {
	PMACROEX p = (PMACROEX)malloc(sizeof(MacroEx));
	PTR_CHECK(p);
	PTR_CHECK(name);
	p->name = CreateString(name);
	p->text = NULL;
	p->next = NULL;
	return p;
}

// Create an unattached macro with name but no text
PMACROEX CreateEmptyMacro(LPCSTR name) {
	return alloc_macro(name);
}

// Create an unattached macro with name and text
PMACROEX CreateMacro(LPCSTR name, LPCSTR text) {
	PMACROEX p = alloc_macro(name);
	p->text = CreateString(text);
	return p;
}

// Destroy an unattached macro
// IMPORTANT: Must remove from table first!!!
void DestroyMacro(PMACROEX macro) {
	if (macro) {
		if (macro->next) {
			fprintf(stderr, "Attempted to destroy attached macro. Dying...\n");
			last_call(1);
		}
		if (macro->text) DestroyString(macro->text);
		DestroyString(macro->name);
		free(macro);
	}
	else {
		fprintf(stderr, "Attempted to destroy NULL macro. Dying...\n");
		last_call(1);
	}
}

// Search macro table by macro name
PMACROEX FindMacro(LPCSTR name) {
	PTR_CHECK(name);
	int len = strlen(name);
	PMACROEX macro = g_table.next;
	while (macro) {
		if (macro->name->len < len) break;
		if (!strcmp(name, macro->name->data)) {
			return macro;
		}
		macro = macro->next;
	}
	return NULL;
}

// Modify any macro's text
void SetMacroText(PMACROEX macro, LPCSTR text) {
	MAC_CHECK(macro);
	if (macro->text) {
		DestroyString(macro->text);
		macro->text = NULL;
	}
	if (text) {
		macro->text = CreateString(text);
	}
}

// Add unattached macro to the table
// IMPORTANT: Must be unattached and name must be unique in table
void AddMacro(PMACROEX macro) {
	MAC_CHECK(macro);
	LPCSTR name = macro->name->data;
	if (macro->next) {
		fprintf(stderr, "Can't add attached macro to table '%s'. Dying...", name);
		last_call(1);
	}
	if (FindMacro(name)) {
		fprintf(stderr, "Macro redefinition '%s'. Dying...", name);
		last_call(1);
	}
	int len = strlen(name);
	PMACROEX rover = g_table.next;
	if (!rover) {
		g_table.next = macro;
		return;
	}
	if (rover->name->len <= len) {
		macro->next = rover;
		g_table.next = macro;
		return;
	}
	while (rover->next) {
		if (rover->next->name->len <= len) {
			macro->next = rover->next;
			rover->next = macro;
			return;
		}
		rover = rover->next;
	}
	rover->next = macro;
}

// Removes an attached macro from the table
// NOTE: Destroys the macro also
void RemoveMacro(PMACROEX macro) {
	if (macro == g_table.next) {
		g_table.next = macro->next;
		macro->next = NULL;
		DestroyMacro(macro);
		return;
	}
	PMACROEX rover = g_table.next;
	while (rover->next) {
		if (macro == rover->next) {
			rover->next = macro->next;
			macro->next = NULL;
			DestroyMacro(macro);
			return;
		}
		rover = rover->next;
	}
	fprintf(stderr, "Macro couldn't be removed\n");
	last_call(1);
}

// Removes all macros from the table and destroys them
void RemoveAllMacros() {
	while (g_table.next) {
		RemoveMacro(g_table.next);
	}
}

// Prints a macro definition
static void print(PMACROEX macro) {
	LPCSTR name = macro->name->data;
	LPCSTR text = macro->text ? macro->text->data : "";
	fprintf(stderr, "%s = %s\n", name, text ? text : "");
}

// Prints a macro definition
void PrintMacro(LPCSTR name) {
	PMACROEX macro = FindMacro(name);
	if (macro) {
		print(macro);
		return;
	}
	fprintf(stderr, "%s is not defined\n", name);
}

// Prints definitions for all macros in the table
void PrintMacros() {
	PMACROEX macro = g_table.next;
	while (macro) {
		print(macro);
		macro = macro->next;
	}
}

