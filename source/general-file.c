/*

	general-file.c

	File I/O

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "inc/bob.h"

#define BOB_ENV         "BOB"		// Environment variable name
#define DELIMS          ";"			// Search path separator
#define MAX_PATHS       (32)		// Maximum search paths

static void qstrcat(LPSTR d, LPCSTR s);

// Array of search paths
static int      g_num_paths;
static LPSTR	g_paths[1+MAX_PATHS];

/*

	init_env()

		Reads the BOB environment string and parses it into
		individual seach paths.
*/

void init_env() {

    static char* szBobPath = "bobpath";
    static char* szBobPathDefault = "prj;img;map;obj;scene;studio;surf";

	LPSTR path;
	LPSTR sub;
	// LPSTR ctx;

	// First search path is empty (working directory)
	g_num_paths = 1;
	g_paths[0] = "";

    if (FileExists(szBobPath))
    {
        char temp[MAX_PATH+1];
        FILE* fp = fopen(szBobPath, "r");
        while(fgets(temp, MAX_PATH, fp))
        {
            sub = temp + strlen(temp);
            while (sub > temp)
            {
                --sub;
                if (isspace(*sub)) *sub = 0;
            }
            g_paths[g_num_paths] = strdup(temp);
            g_num_paths++;
        }
    }

    else
    {
        sub = getenv(BOB_ENV);
        if (!sub) {
            LogPrint("Missing BOB environment variable.");
            sub = szBobPathDefault;
        }
        path = strdup(sub);

        sub = strtok(path, DELIMS);
        while(sub && g_num_paths < MAX_PATHS) {
            g_paths[g_num_paths] = strdup(sub);
            g_num_paths++;
            sub = strtok(NULL, DELIMS);
        }
    }

	g_paths[g_num_paths] = NULL;
}


/*
	find()

		Searches all BOB directories for a relative filename
		and composes its full pathname if successful.
	
		The full pathname goes in g_full_path for success.

		Names enclosed in double quotes are allowed.

		Return FALSE to indicate file not found.
*/

static char g_full_path[1024];

static BOOL find(LPCSTR filename) {

	// Scan all search directories
	for (int i = 0; i<g_num_paths; i++) {

		// Copy search directory
		strcpy(g_full_path, g_paths[i]);
		
		// If not working directory
		if (g_full_path[0]) {
			// Append backslash
			strcat(g_full_path, "/");
		}

		// Append filename
		qstrcat(g_full_path, filename);
		
		// Return TRUE if file exists
		if (FileExists(g_full_path)) return TRUE;
	}

	// Reset full path to empty string
	*g_full_path = 0;
	
	// Failed
	return FALSE;
}


/*
	env_fopen() 
	
		Acts just like fopen except uses the paths
		taken from the environment variable BOB.
		Also handles filenames wrapped in quotes.
*/

FILE* env_fopen(LPCSTR filename, LPCSTR mode) {

	FILE* fp;

	// Compose fill pathname and ensure file exists
	if (find(filename)) {

		// Try to open the file
        fp = fopen(g_full_path, mode);
		if (fp) {
			
            char sz[1024];
            sprintf(sz, "Opened: %s", g_full_path);
            LogPrint(sz);

			// Gooooood... :)
			return fp;
		}
	}

	// Set errno code
	errno = ENOENT;

	// Failed :(
	return NULL;
}


/*
	qstrcat()
	
		Just like strcat() except ignores double quotes.
*/

static void qstrcat(LPSTR d, LPCSTR s) {

	// Find end of dest
	while (*d) {
		d++;
	}

	// Tack on source
	while (*s) {
		if (*s != '"') {
			*d = *s;
			d++;
		}
		s++;
	}

	// NULL terminate
	*d = 0;
}


/*
	CompletePath()

	Searches all our directories for the specified file
	and returns its full pathname if located.

	Returns NULL for error.
*/

PSTRING CompletePath(LPCSTR filename) {
	if (find(filename)) {
		return CreateString(g_full_path);
	}
	return NULL;
}

/*

	ReadMapFile()

	Reads a color palette from a MAP file.

	Format is 256 lines of text.
	
	Each line is an RGB value (separated by whitespace).
	Color channels are BYTE values: [0 ... 255].

*/

BOOL ReadMapFile(LPCSTR filename, PBACKGROUND bgnd) {
	int     i, r, g, b;
	char    str[256];
	FILE* fp = env_fopen(filename, "rt");
	if (!fp) {
		fprintf(stderr, "Error opening file %s for input as palette file.\n", filename);
		return FALSE;
	}
	for (i = 0; i<256; i++) {
		fgets(str, sizeof (str), fp);
		sscanf(str, "%d %d %d", &r, &g, &b);
		bgnd->pal[i][0] = r;
		bgnd->pal[i][1] = g;
		bgnd->pal[i][2] = b;
		if (ferror(fp)) {
			fclose(fp);
			fprintf(stderr, "Error reading file %s for input as palette file.\n", filename);
			return FALSE;
		}
	}
	MakeVector(-1, -1, -1, bgnd->color);
	return TRUE;
}


// List search paths
void print_search_paths(void) {
	PrintTextArray("Search Paths", (LPCSTR*)g_paths);
}
