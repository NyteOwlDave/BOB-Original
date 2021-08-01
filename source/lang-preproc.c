
/*
	lang-preproc.c

	Language macro preprocessor

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "inc/bob.h"

#define _DEBUG          FALSE       // Set TRUE for debug logging

#define BLANK           ' '			// blank space

#define MAX_LEVEL       (20)		// maximum # of open files (include depth)
#define MAX_LINE        (1024)		// longest input line
#define MAX_TOK         (80)		// longest token size 
#define MAX_EXPAND      (32767)		// largest size to expand a line to

static void expand(LPCSTR src);
static void sub_macro(Macro* mptr, LPSTR target);
static void add_macro(LPCSTR def);
static void remove_macro(LPCSTR name);
static void log_macro(PMACRO mptr);
static LPCSTR brute(LPCSTR text, LPCSTR pat, int tlen, int plen);
static void prepare(LPCSTR);
static void clean_up(void);
static int vfgets(LPSTR dst, int max_count, FILE* fp);
static LPCSTR get_next_token(LPCSTR text);
static int cpy_tok(LPSTR dst, LPCSTR src);
static int is_tok(int c);
static void log_print(LPCSTR msg);

static  PMACRO MacroHead = NULL;	// Head of macro list

static LPSTR	line;			    // fully expanded line
static LPSTR	mline;				// macro expansion buffer (inline)
static LPSTR	tline;				// macro expansion buffer (temp)
static int      cur;				// number of open files
static FILE*	fp[MAX_LEVEL];		// input file streams (raw)
static FILE*	outfp = NULL;		// output file stream (cooked)
static char		str[MAX_LINE];		// input line


/*
	preproc()

	Preprocesses an input file.
	Performs macro expansions and include directives.

	Expects 2 arguments: infile outfile

	Returns 0 if ok, last_call()s otherwise

	Macros must start with an alpha and may contain alphanum and '_'

	Note: Macros are sorted longest to shortest.

*/

int preproc(LPCSTR infile, LPCSTR outfile) {

	static	char	newfile[MAX_LINE];	// filename
	static	char    command[MAX_TOK];	// preprocessor directive

	// Report file open event
	if (tickflag) {
		printf("Preprocessing file:\n\t%s\n", infile);
	}

    // Prepare for preprocessing
    prepare(outfile);

	// Open the input file
	fp[cur] = env_fopen(infile, "r");
	if (!fp[cur]) {
		fprintf(stderr, "Preprocessor error opening file %s for input.\n", infile);
		last_call(1);
	}

	// Write newfile token (for interpreter)
	fprintf(outfp, "vv_newfile \"%s\"\n", infile);
	
    log_print("\nPreprocessing...");

	// Preprocess input
	do {
		
		// Read string (if possible)
		if (!vfgets(str, MAX_LINE, fp[cur])) {

			// Close input file
			fclose(fp[cur]);
			
            // Backup to prior input file
			cur--;
			
            // Write popfile token (for interpreter)
			fputs("vv_popfile\n", outfp);

            log_print("Close file");
		} 
		
		// Process the input string
		else {

			// Start at beginning of input line
			LPSTR rest = str;
			
			// Skip leading whitespace
			while (*rest == ' ' || *rest == '\t') {
				rest++;
			}

            LPSTR cptr;

			// Take out EOL type comments
			int len = strlen(rest);
			if (cptr = (LPSTR)brute(rest, "//", len, 2)) {
				strcpy(cptr, "\n\0");
			}

            /*
                Preprocessor Directives
            */

			// Is this a preprocessor command?
			if (*rest == '#') {

				// Read preprocessor directive (hash and name)
				sscanf(rest, "#%s", command);

				// Update rest pointer (past directive token)
				rest += strlen(command) + 1;

				// Line too short?
				if (!rest[0]) {
					fprintf(stderr, "Invalid preprocessor directive format\n");
					last_call(1);
				}

				// Include command?
				if (strcmp(command, "include") == 0) {

					// Read filename into newfile buffer
					sscanf(rest, "%s", newfile);

					// Bump input file #
					++cur;

                    // Detect too many open files
                    if (cur >= MAX_LEVEL) {
                        fprintf(stderr, "Too many open files.\n");
                        last_call(1);
                    }

					// Open new input file
					fp[cur] = env_fopen(newfile, "r");
					if (!fp[cur]) {
						fprintf(stderr, "Preprocessor error opening file %s for input.\n", newfile);
						last_call(1);
					}

					// Write newfile token
					fprintf(outfp, "vv_newfile \"%s\"\n", newfile);
					if (tickflag) {
						printf("Opened %s\n", newfile);
					}

                    log_print("Open file");
                    log_print(newfile);

				} 
				
				// Define command?
				else if (strcmp(command, "define") == 0) {

					// Skip whitespace
					while (*rest == ' ' || *rest == '\t') {
						rest++;
					}

					// Add macro definition
					add_macro(rest);
					
					// Write newline to O/P file
					fputc('\n', outfp);

                    log_print("Macro defined");

				} 
				
				// Undefine macro command?
				else if (strcmp(command, "undef") == 0) {

					// Skip whitespace
					while (*rest == ' ' || *rest == '\t') {
						rest++;
					}

					// Remove macro definition
					remove_macro(rest);

					// Write newline to O/P file
					fputc('\n', outfp);

                    log_print("Macro undefined");
				} 
				
				// Bummer... bad directive
				else {

					fprintf(stderr, "Unknown preprocessor command \"%s\"\n", command);
					last_call(1);	// No return
				}

			} 

            /*
                Normal line
            */

			// No directives in line
			else {

                log_print("Expanding");

				// Expand any macro definitions --> line buffer
				expand(rest);

				// Write line buffer to o/p file
				fputs(line, outfp);

			}

		}

	} while(cur >= 0);	// Repeat until all input files are closed

    log_print("Cleaning up");

	// Do final cleanup
	clean_up();

	// Why zero?
	return 0;

}


/*

	Remove excess white space and expand any macros in
	the line passed.  
    
    Return new text in line.

	Note that on input src points to somewhere in the
	str[] array from the preproc() function -- OR --
    the mline[] array from the add_macro() function.

*/

// Find a macro that has a matching name length
static inline Macro* find_likely_macro(int len) {
	// Locate first macro with matching length
	Macro* mptr = MacroHead;
	while (mptr && mptr->mlen > len) {
		mptr = mptr->next;
	}
	return mptr;
}

// Expand macro
static void expand(LPCSTR src) {

	static char	token[MAX_TOK];
    int     token_length = 0;
	BOOL	found = FALSE;

	// Skip leading white space
	while (*src == ' ' || *src == '\t') {
		src++;
	}

	// Copy input string to line buffer
	strcpy(line, src);

	// Pointer to start of line buffer
	LPCSTR ptr = line;

	// Read in line token by token
	while (ptr = get_next_token(ptr)) {

		// Get token --> token[]
		// and token's length
        // NOTE: Preproc token have different grammar rules than Interpreter tokens!
		token_length = cpy_tok(token, ptr);
		
		// Locate first macro with matching length
		PMACRO mptr = find_likely_macro(token_length);

		//
		// Look for a matching macro
		// 
		
		// No macro found yet
		found = FALSE;

		// So long as macro length matches
		while (mptr && mptr->mlen==token_length) {

			// Is this the correct macro name?
			if (!strcmp(mptr->macro, token)) {

                log_print("\nLINE");
                log_print(line);

				// Substitute macro text for macro name
				sub_macro(mptr, (LPSTR)ptr);
				
                log_print(line);

				// We gone one!
				found = TRUE;

				// Exit loop
				break;

			}

			// Keep looking
			mptr = mptr->next;

		}

		// Macro not found
		if (!found) {    
		
			// Move beyond token since it's not a macro
			ptr += token_length;

		}

	}

}


/*

	Do a macro substitution in place.

    The target buffer must contain a macro name at the start.
    The mptr pointer must point to the macro for substitution
    into the target buffer.

    Text after the macro name is moved either forward or backward
    such that the replacement text can be fitted into the space
    it occupied.

    The macro name is replaced by the substitution text (already expanded)
    plus two guard spaces (blanks), one at the start and one at the end.

    Generally, the target buffer is somewhere in either line or mline.

*/

static void sub_macro(Macro* mptr, LPSTR target) {
	
    log_print("\nSUBSTITUTE");
    log_print("TARGET");
    log_print(target);
    log_macro(mptr);

	// If not empty 
	if (mptr->tlen > 0) {

       // Capture text following the macro name
       strcpy(tline, target+mptr->mlen);

        LPCSTR old_target = target;

       // Write a leading blank
       *target++ = BLANK;

       // Overwrite macro name with macro text
       strcpy(target, mptr->text);

       // Skip macro text
       target += mptr->tlen;

       // Write a trailing blank
       *target++ = BLANK;

       // Copy in the captured text
       strcpy(target, tline);

       log_print("TARGET");  log_print(old_target);
       log_print("tline");   log_print(tline);
	} 
	
	// Macro definition is empty
	else {

		// Copy remainder of input line over macro name
		strcpy(target, target+mptr->mlen);
	}

}


/*

	Add a new macro to the list.
	Macros are sorted largest to smallest.

    Macros are expanded recursively.

	Expanded stuff is written to mline

    The line buffer is clobbered.

*/

// Write macro to log
static void log_macro(PMACRO mptr) {
    log_print("");
    log_print( mptr->macro );
    log_print( mptr->text );
    char sz[MAX_TOK];
    sprintf( sz, "Length = %u, %u", mptr->mlen, mptr->tlen );
    log_print( sz );
}

// Add macro to list
static void add_macro(LPCSTR def) {

	char    token[MAX_TOK];

	// Copy to mline
	strcpy(mline, def);

	// Copy to mline and keep adding until end of macro
	while (TRUE) {

		// Skip over existing expanded text
		LPSTR txt = mline + strlen(mline) - 1;
		
		// Move txt back to last non-white char
		while (isspace(*txt)) {
			txt--;
		}

		// Line continuation character?
		if (*txt == '\\') {

			// Unget char
			txt--;

			// Move back to last non-white char
			while (isspace(*txt)) {
				txt--;
			}

			// Put a blank between lines
            // This replaces the "\" line continuation character
			txt++;
			*txt = ' '; 
			txt++;

			// Input buffer pointer
			LPSTR where = line;
			
			// Read next input line (after "/")
			vfgets(where, MAX_LINE, fp[cur]);
			
			// Skip whitespace
			while (isspace(*where)) {
				where++;
			}

            LPSTR cptr;

			// Take out EOL type comments
			if (cptr = (LPSTR)brute(where, "//", strlen(where), 2)) {
				strcpy(cptr, "\n\0");
			}

			// Append new input line to macro line (mline)
			strcpy(txt, where);

			// keep those line #'s straight
			fputc('\n', outfp);

		}

		// Not a line continuation character
		else {
			break;
		}
	}

	// Start of macro line
	LPCSTR mactxt = mline;

	//
	// Create new macro definition and fill it in
	//

	// Allocate macro definition
	PMACRO mptr = (Macro *)malloc(sizeof(Macro));
	ptrchk(mptr, "macro structure");

	// Read macro name into token buffer
	sscanf(mactxt, "%s", token);

	// Copy to macro definition
	mptr->macro = strdup(token);
	ptrchk(mptr->macro, "macro define");
	mptr->mlen = strlen(mptr->macro);

	// Skip macro name
	mactxt += mptr->mlen;

	// Expand the macro def to line buffer
	expand(mactxt);

	// Save to macro definition
	mptr->text = strdup(line);
	ptrchk(mptr->text, "macro text");
	mptr->tlen = strlen(mptr->text);

	// Replace all \n \r in macro with blanks
	LPSTR macdef = mptr->text;
	while(*macdef) {

		if (*macdef == '\n' || *macdef == 13) {
			*macdef = BLANK;
		}

		macdef++;
	}

	//
	// Add to list, sort by mlen, largest to smallest
	//

	// Trivial case where this new macro needs to be the new head
	if (MacroHead == NULL || MacroHead->mlen <= mptr->mlen) {
		// put at top of list
		mptr->next = MacroHead;
		MacroHead = mptr;
	} 
	
	// Have to sort into list
	else {

		// Top of macro list
		PMACRO sptr = MacroHead;
		
		// Skip macros with shorter names
		while (sptr->next && sptr->next->mlen > mptr->mlen) {
			sptr = sptr->next;
		}
		
		// Attach macros with longer names to follow this new one
		mptr->next = sptr->next;
		
		// Attach this new one to follow its sibling with shorter name
		sptr->next = mptr;
	}

    log_macro( mptr );

}


/*
	Remove a macro from the list.
*/

static void remove_macro(LPCSTR name) {

    char token[MAX_TOKEN+1];

	int     i;

	// Clean off any following garbage
	LPCSTR cptr = name;

    // Remove any leading whitespace
    while (isspace(*cptr)) ++cptr;

    // Capture token
	for (i=0; i < MAX_TOKEN && is_tok(*cptr); i++) {
		token[i] = *cptr++;
	}
    token[i]=0;

	PMACRO mptr = MacroHead;
	int token_length = i;

	// Check if at top of list
	if (strcmp( token, mptr->macro ) == 0) {     // winner!
		MacroHead = MacroHead->next;
		free( mptr->macro );
		free( mptr->text );
		free( mptr );
	} 
    // gotta look for it
    else { 
		while (mptr->next && mptr->next->mlen > token_length) {      // skip cruft
			mptr = mptr->next;
		}
		while (mptr->next && mptr->next->mlen==token_length) {       // while possible
			if (strcmp(token, mptr->next->macro) == 0) {             // winner!
				PMACRO mptr2 = mptr->next;
				mptr->next = mptr2->next;                   // remove from list
				free( mptr2->macro );                       // free up
				free( mptr2->text );
				free( mptr2 );
				return;
			} else {
				mptr = mptr->next;
			}
		}
	}

}


/*
	Brute force pattern matching.
    text = Text to test
    pat = Pattern for testing
    tlen = Length of text
    plen = Length of pattern
    Returns NULL for no match, else a pointer to the
    first character in text beyond the pattern
*/

static LPCSTR brute(LPCSTR text, LPCSTR pat, int tlen, int plen) {

	register int     i, j;

	i = j = 0;
	do {
		if (text[i] == pat[j] && 
            (j!=0 || i==0 || isspace(text[i-1]) || 
            (ispunct(text[i-1]) && 
            text[i-1] != '_'))) {
			i++;
			j++;
		} else {
			i = i-j+1;
			j = 0;
		}
	} while (j<plen && i<tlen);

	if (j==plen)
		return text+i-plen;
	else
		return NULL;

}


// Prepare for preprocessing
static void prepare(LPCSTR outfile) {

	// Allocate an expansion buffer for line text
	line = (LPSTR)malloc(MAX_EXPAND);
	ptrchk(line, "preprocessor space");

	// Allocate an expansion buffer for macro text
	mline = (LPSTR)malloc(MAX_EXPAND);
	ptrchk(mline, "preprocessor space");

	// Allocate an expansion buffer for macro text
	tline = (LPSTR)malloc(MAX_EXPAND);
	ptrchk(mline, "preprocessor space");

	// Open the O/P file
	outfp = fopen(outfile, "w");
	if (!outfp) {
		fprintf(stderr, "Preprocessor error opening temp file %s for output.\n", outfile);
		last_call(1);
	}

	// Set cur to zero (i/p file #)
	cur = 0;

}

//	Cleanup after preprocessing.
static void clean_up(void) {

	Macro   *mptr;

	free( line );
	free( mline );
    free( tline );

	while (MacroHead) {
		mptr = MacroHead;
//        log_macro(mptr);
		MacroHead = MacroHead->next;
        ptrchk(mptr->macro, "Macro name pointer");
		free(mptr->macro);
        ptrchk(mptr->text, "Macro text pointer");
		free(mptr->text);
		free(mptr);
	}

	// Close the o/p file
	fclose(outfp);

}


/*
	Homegrown fgets type function which takes care of eliminating
	comments and purging whitespace.
*/

static int vfgets(LPSTR dst, int max_count, FILE* fp) {

	int     c=0, p=0;       // current and previous characters
	int	    count;          // current line count

	if (feof(fp)) {
        *dst = 0;
		return 0;
	}

    LPCSTR saved_dst = dst;

	c = p = count = 0;
	do {
		p = c;
		c = fgetc(fp);
		if (c == EOF) {
			*dst++ = '\n';
			*dst = '\0';
			return 1;
		}
		if (p=='/' && c=='*') {     // we're in a comment
			dst--;                  // back off the '/'
			count--;
			p = c = 0;
			do {                    // go until end of comment
				p = c;
				c = fgetc(fp);
				if (c == '\n') {
					fputc('\n', outfp);     // keep those line #'s straight
				}
			} while (p != '*' || c != '/');
		} else {
			*dst++ = c;
			count++;
			if (count >= max_count) {
				fprintf(stderr, "Preprocessor error, input line too long.\n");
				fprintf(stderr, "Input lines must be less than %d characters in length.\n", MAX_LINE);
				last_call(1);
			}
		}
	} while (c != '\n');
	*dst = '\0';

    log_print("READLINE:");  
    log_print(saved_dst);

	return 1;

}


/*
	Find the beginning of the next token in the
	text string and return its location.
*/

static LPCSTR get_next_token(LPCSTR text) {

	while (*text && isalpha(*text)==0) {
		text++;
	}
	if (*text) {
		return text;
	} else {
		return NULL;
	}

}


/*
	Copy the token at src to dst and return its length.
*/

static int cpy_tok(LPSTR dst, LPCSTR src) {

	// Character count for token
	int cnt = 0;

	// Copy token to o/p buffer and track its character count
	while (is_tok(*src)) {
		if (cnt >= MAX_TOK) {
			fprintf(stderr, "Exceeded maximum preprocessor token length.");
			last_call(1);
		}
		*dst++ = *src++;
		cnt++;
	}
	// Null terminate
	*dst = '\0';

	// Return token length
	return cnt;
}


/*
	Determine if c is a valid token character.
	
    Valid token characters are:
		[A..Z] [a..z] [0..9]
		and _ . \ :
	returns 1 is yes, 0 if no
*/

static int is_tok(int c) {

	if (isalnum(c) || c=='_' || c=='.' || c=='\\' || c==':') {
		return 1;
	} 
	
	else {
		return 0;
	}
}

// Print to log (if debug mode)
static void log_print(LPCSTR msg) {
    if (_DEBUG) {
        LogPrint(msg);
    }
}


// Print macros to screen
void print_macros(void) {
    int nMacro = 0;
    PMACRO pMacro = MacroHead;
    while (pMacro) {
        printf("Macro #%i", ++nMacro);
        Print( pMacro->macro );
        Print( pMacro->text );
        char sz[MAX_TOK];
        sprintf( sz, "Length = %u, %u", pMacro->mlen, pMacro->tlen );
        Print( sz );
        pMacro = pMacro->next;
    }
}

