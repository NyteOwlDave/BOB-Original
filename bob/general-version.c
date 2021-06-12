
/*

	general-version.c

	Application version

*/

#include "inc/defs.h"

LPCSTR _Program		= "BOB Ray Tracer";
LPCSTR _Version		= "Version 1.2";
LPCSTR _Date		= "May 26, 2021";
LPCSTR _Copyright	= "Copyright 1989-1992 Christopher D. Watkins & Stephen B. Coy";

void PrintAppVersion() {
	fprintf(stderr, "\n%s    %s\n%s\n", _Program, _Version, _Copyright);
}

