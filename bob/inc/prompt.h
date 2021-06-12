
/*

    prompt.h

    Private include for prompt-xxx modules.

*/

#ifndef PROMPT_DEFINED
#define PROMPT_DEFINED


#include "defs.h"


#define MAX_COMMAND 1024
#define NUM_ARGS 32


// Menu command info entry
typedef struct t_cmd_info {
	LPCSTR name;	// Keyword
	pfnCmd func;	// Callback function
	LPCSTR desc;	// Description
} CMDINFO, *PCMDINFO;


extern BOOL arg_count(int n);
extern void merge_tokens(int first);
extern void split_tokens(void);
extern LPCSTR read_token(int token, LPSTR buffer);
extern void dispatch(void);
extern void prompt(void);

extern void error(LPCSTR sz);
extern void wrong_count(int n);

extern void set_menu(PCMDINFO pMenu, LPCSTR title);

extern void start_main_menu(void);
extern void start_bob_menu(void);
extern void start_diag_menu(void);
extern void start_state_menu(void);
extern void start_lang_menu(void);




#endif // !PROMPT_DEFINED

