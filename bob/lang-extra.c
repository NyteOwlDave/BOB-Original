/*
	lang-extra.c

	Additional parsing routines

*/

/*
	Additional parse routines put here to try and keep parse.c
	down to a reasonable size.  Doesn't help much, does it?
*/

#include <stdio.h>
#include <string.h>
#include "inc/bob.h"
#include "inc/tokens.h"
#include "inc/hacks.h"

// No return
int yyerror(LPCSTR str) {

	Infile* iptr = (Infile *)InfileTop->what;

	fprintf(stderr, "\n\nError at line %d\n", yylinecount+1);
	fprintf(stderr, "file \"%s\"\n", iptr->file_name);
	fprintf(stderr, "%s\n", str);
	fprintf(stderr, "Error at or near = \"%s\"\n", cur_text);
	
	last_call(1);

	return 0;	// keep lint/compilers quiet

}

// This is the main enty point for the parser
// It's called from the main() function
void ReadSceneFile(LPCSTR real_name, LPCSTR tmp_name) {

	Infile* iptr;

    LogPrint("Reading scene file");

	// Init global clips before parser is called
    LogPrint("Initialize global clip list");
	if (!GlobalClipTop) {
		GlobalClipTop = (GlobalClip *)vmalloc(sizeof(GlobalClip));
		ptrchk(GlobalClipTop, "global clip structure");
		GlobalClipTop->next = NULL;
		GlobalClipTop->clip = NULL;
	}

	// Open input file using temporay name
	// (may be different than the real name)
    LogPrint("Open source file");
	if ((yyin = env_fopen(tmp_name, "r")) == NULL) {
		fprintf(stderr, "Error, cannot open %s\n", tmp_name);
		last_call(1);
	}

	// Copy real name to input filename (why? dunno)
	strcpy(Infilename, real_name);

	//
	// Set up input file stack
	//

	// Allocate stack item and make it the head
    LogPrint("Initialize input file stack");
	InfileTop = (Stack *)malloc(sizeof(Stack));
	ptrchk(InfileTop, "input file stack object");
	InfileTop->prev = NULL;

	// Allocate input file item and attach to stack item
	iptr = (Infile *)malloc(sizeof(Infile));
	ptrchk(iptr, "input file structure");
	InfileTop->what = (void *)iptr;

	// Allocate input filename
	// write it and the line counter to file item dat
    LogPrint(Infilename);
	iptr->file_name = strdup(Infilename);
	ptrchk(iptr->file_name, "input file name");
	iptr->line = 0;

	// parse the input file
	if (yyparse() == 1) {
		fprintf(stderr, "Invalid input specification\n");
		last_call(1);
	}

	// clean up transform structures
	trans_popall();

	// bummer! no studio settings were detected
	// (note stop_line set in main
	if (stop_line == -1) {
		fprintf(stderr, "\n\nError, no studio structure in input file.\n");
		last_call(1);
	}

	// show final details
	if (tickflag) {
		fprintf(stderr, "%c\tinputfile = \"%s\"                    \n", 13, Infilename);
		fprintf(stderr, "\tlights %d, prims %u\n", nLights, nPrims);
		fprintf(stderr, "\tresolution %d %d\n", Xresolution, Yresolution);
	}

}

// pointer check
void ptrchk(const void* ptr, LPCSTR str) {

	if (ptr) return;

	fprintf(stderr, "\n\nError allocating memory for a %s.\n", str);
	
	last_call(1);

}

// statistics report
void yystats(void) {

	static int toc = 0;

	if (tickflag && ((toc&0x0f)==0)) {
		fprintf(stderr, "%c\tlights %d, prims %d, memory %u", 13, nLights, nPrims, MemAllocated);
	}

	toc++;
}

// open new input file and push to input file stack
void yy_newfile(LPCSTR new_file) {

	Infile  *iptr;
	Stack   *sptr;

    LogPrint("yy_newfile");

	// save line number for current file
	iptr = (Infile*)(InfileTop->what);
	iptr->line = yylinecount;

	// Allocate new stack item
	sptr = (Stack *)vmalloc(sizeof(Stack));
	ptrchk(sptr, "input file stack object");

	// push stack item
	sptr->prev = InfileTop;
	InfileTop = sptr;

	// allocate new input file data
	iptr = (Infile *)vmalloc(sizeof(Infile));
	ptrchk(iptr, "input file structure");

	// attach to stack item
	InfileTop->what = (void *)iptr;

	// initialize input file data
	iptr->file_name = strdup(new_file);
	iptr->line = 0;

	// reset input line count
	yylinecount = -1;

}

// pop input file off input file stack
void yy_popfile(void) {

	// if not end of input file stack
	if (InfileTop->prev) {

		Infile  *iptr;
		Stack   *sptr;

		// pop stack item
		sptr = InfileTop;
		InfileTop = InfileTop->prev;
		
		// get file info pointer
		iptr = (Infile *)InfileTop->what;
		
		// global input line count bak to previous file's count
		yylinecount = iptr->line;
	} 
	
	// Ummm... too many pops!
	else {

		fprintf(stderr, "\nTemp input file corrupted. Dying...\n");
		last_call(1);

	}
}

// Pop transform off transform stack
void trans_pop(void) {

	Transform* tptr = TransTop;
	
	if (tptr == NULL) {
		yyerror("Trying to pop a transformation from an empty stack.");
	}

	TransTop = TransTop->next;

}

// Pop all trransforms (clear stack)
void trans_popall(void) {

	while (TransTop) {
		trans_pop();
	}
}

