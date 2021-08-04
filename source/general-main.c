/*
	general-main.c

	Module module

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/bob.h"


#define EXT_INFILE  ".bob"
#define EXT_OUTFILE ".img"
#define PREPROC_FILE "yyz.bbs"

// Internal helper to display command line usage
static void usage(LPCSTR Progname);


// Filename and extension for the executable file
// (in case it has been renamed by the user)
static char g_exename[MAX_PATH] = "";

// Filenames
static char g_infilename[MAX_TOKEN] = "";             // Input file
static char g_outfilename[MAX_TOKEN] = "";            // Output file

// Command line options data structure
typedef struct t_command_options {
	int     xres;					// screen width
	int		yres;					// screen height
	int		depth;					// maximum recursion depth (maxlevel)
	int		amode;					// antialias mode
	int		start;					// Session start line
	int		stop;					// Session stop line
	int		bunch;					// Composite shape bunch size
	BOOL	no_shadows;				// shadows off
	BOOL	resume;					// Resume prior session
	BOOL	verbose;				// a.k.a. tickflag
	BOOL    preprocess;				// Invoke preprocessor
	BOOL	console;				// Interactive console mode
	BOOL	err_console;			// Interactive console mode on error
    BOOL    no_render;              // Preprocessor only mode
	char    session[MAX_TOKEN];		// Session name
} CommandOptions, *PCOMMANDOPTIONS;


/*
	Parses command line arguments into the command options structure.
*/

static void init_command_options(PCOMMANDOPTIONS opts, int ac, char** av) {
	opts->xres = (-1);
	opts->yres = (-1);
	opts->depth = (-1);
	opts->amode = (-1);
	opts->start = (-1);
	opts->stop = (-1);
	opts->bunch = (-1);
	opts->resume = FALSE;
	opts->verbose = TRUE;
	opts->preprocess = TRUE;
	opts->console = FALSE;
	opts->err_console = FALSE;
	opts->no_shadows = FALSE;
    opts->no_render = FALSE;
	opts->session[0] = 0;
	for (int i = 1; i<ac; i++) {
		if (av[i][0] == '-') {
			switch (av[i][1]) {
			case 'c':       // Interactive console
				opts->console = TRUE;
				break;
			case 'k':       // Interactive console on error
				opts->err_console = TRUE;
				break;
			case 'i':       // set resolution
				i++;
				opts->xres = atoi(av[i]);
				i++;
				opts->yres = atoi(av[i]);
				break;
			case 'l':       // set start & stop line
				i++;
				opts->start = atoi(av[i]);
				i++;
				opts->stop = atoi(av[i]);
				break;
			case 'd':       // set max recursion depth
				i++;
				opts->depth = atoi(av[i]);
				break;
			case 'b':       // set bunching factor
				i++;
				opts->bunch = atoi(av[i]);
				break;
			case 'a':       // set anti-alias mode
				i++;
				switch (av[i][0]) {
				case 'n':
					opts->amode = A_NONE;
					break;
				case 'c':
					opts->amode = A_CORNERS;
					break;
				case 'q':
					opts->amode = A_QUICK;
					break;
				case 'a':
					opts->amode = A_ADAPTIVE;
					break;
				default:
					fprintf(stderr, "Error, unkown antialiasing mode %s.\n", av[i]);
					usage(Progname);
					break;
				}
				break;
			case 'n':       // set no shadows mode
				opts->no_shadows = TRUE;
				break;
			case 'r':       // set resume mode
				opts->resume = TRUE;
				break;
			case 's':       // turn off statistics
				opts->verbose = FALSE;
				LogPrint("Statistics disabled");
				break;
			case 'p':       // turn off preprocessor
				LogPrint("Preprocessor disabled");
				opts->preprocess = FALSE;
				break;
            case 'o':       // turn off rendering
                LogPrint("Rendering disabled");
                opts->no_render = TRUE;
                opts->preprocess = TRUE;
                break;
			case '?':               // help mode
				usage(Progname);	// no return
				break;
			default:
				fprintf(stderr, "Error, unkown command line flag %s.\n", av[i]);
				usage(Progname);
				break;
			}
		}
		// Must be session name
		else {
			// Save session name
			strcpy(opts->session, av[i]);
		}
	}
}


/*
	Uses the command line options structure to modify application state.
*/

static void enact_command_options(PCOMMANDOPTIONS opts) {

	no_shadows = opts->no_shadows;
	resume = opts->resume;
	tickflag = opts->verbose;

	if (opts->xres > 0) {
		Xresolution = opts->xres;
        // ### IS THIS OK? ###
		if (stop_line == Yresolution)    // only change if not default
			stop_line = opts->yres;
		Yresolution = opts->yres;
	}

	if (opts->start != -1) {
		start_line = opts->start;
		stop_line = opts->stop;
	}

	if (opts->depth > 0) {
		maxlevel = opts->depth;
	}

	if (opts->bunch > 0) {
		bunching = opts->bunch;
	}

	if (opts->amode >= 0) {
		antialias = opts->amode;
	}

    console = opts->err_console;
}


/*
	main() -- Application entry point
*/

int main(int ac, char** av) {

	CommandOptions opts;

    // Open log
    LogOpen();

	// Display application version
	PrintAppVersion();

	// Initialize environment paths
	init_env();

	// Initialize some noise
	init_noise();

	// Clip path off of program name
	if ((Progname = rindex(av[0], '/')) == NULL) {
		Progname = av[0];
	}
	else {
		Progname++;
	}
	strcpy(g_exename, Progname);
	Progname = g_exename;
	// DOS: strlwr(Progname);

	// Initialize command line options
	init_command_options(&opts, ac, av);

	// No session name?
	if (opts.session[0] == 0) {
		enact_command_options(&opts);
		InteractiveConsole();
		last_call(0);
	}
	
	// Compose input and output filenames from session name
	strcpy(g_infilename, opts.session);
	strcpy(g_outfilename, g_infilename);
	strcat(g_infilename, EXT_INFILE);
	strcat(g_outfilename, EXT_OUTFILE);

	// Helps to catch no studio error
	stop_line = Yresolution;    // (-1);
	camera.aperture = -1.0;
	camera.focal_length = 0.0;
	Eye.view_aspect = -1.0;

	// Preprocess the read the scene file
	if (opts.preprocess) {
		preproc(g_infilename, PREPROC_FILE);
        if (opts.no_render) exit(0);
		ReadSceneFile(g_infilename, PREPROC_FILE);
	}

	// Read the scene file without preprocessing
	else {
		ReadSceneFile(g_infilename, g_infilename);
	}

	 // Do view angle fixup
     // ReadSceneFile can modify Eye
	if (Eye.view_aspect != -1.0) {
		Eye.view_angle_y = Eye.view_angle_x / Eye.view_aspect;
	}

	//
	// Deal with command line overrides
	//
	enact_command_options(&opts);

	// Build bounding slabs
	BuildBoundingSlabs();

	// Begin interactive mode
	if (opts.console) {
		InteractiveConsole();
	} else {
        ShootScene();
    }

	// Bye!!!
	return 0;

}

// Render the scene
void ShootScene(void) {
	// Render the scene
	Screen(&Eye, g_outfilename, Xresolution, Yresolution);
}

// Displays command line usage (internal)
static void usage(LPCSTR exename) {
	fprintf(stderr, "%s        %s        %s\n%s\n\n", _Program, _Version, _Date, _Copyright);
	PrintUsage();
	last_call(0);
}


// Displays command line usage (global)
void PrintUsage() {
	Print("");
	fprintf(stderr, "Usage:  %s [flags] <session>\n", Progname);
	Print("");
	fprintf(stderr, "       -c start interactive console.\n");
	fprintf(stderr, "       -e start interactive console on error.\n");
	fprintf(stderr, "       -s set silent mode.\n");
	fprintf(stderr, "       -r resume generating an interrupted image.\n");
	fprintf(stderr, "       -i change image size to xres by yres.\n");
	fprintf(stderr, "       -l change start and stop line.\n");
	fprintf(stderr, "       -n no shadows.\n");
	fprintf(stderr, "       -d limit recursion level to depth.\n");
	fprintf(stderr, "       -a change antialiasing to mode.  Valid modes are:\n");
	fprintf(stderr, "          none, corners, quick, and adaptive.\n");
	fprintf(stderr, "       -p run without the preprocessor.\n");
	fprintf(stderr, "       -o run only the preprocessor.\n");
	fprintf(stderr, "       -b set bunching factor.\n");
	Print("");
	fprintf(stderr, "Uses <session>.bob for I/P file and <session>.img for O/P file.\n");
	Print("");
}

