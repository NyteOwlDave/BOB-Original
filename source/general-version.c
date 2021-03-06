
/*

	general-version.c

	Application version

*/

#include "inc/defs.h"

LPCSTR _Program = "BOB Ray Tracer";
LPCSTR _Version = STABLE_VERSION;
LPCSTR _Date = STABLE_DATE;
LPCSTR _Copyright = "Copyright 1989-1992 Christopher D. Watkins & Stephen B. Coy";

void PrintAppVersion()
{
    fprintf(stderr, "%s        %s        %s\n%s\n\n", _Program, _Version, _Date, _Copyright);
}
