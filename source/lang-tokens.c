/*
	lang-tokens.c

	Converts input source code to tokens

*/

/*
	This is where the input file is read in and	broken up
	into tokens which are then fed to the parser as it asks
	for them.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "inc/bob.h"
#include "inc/tokens.h"

// #define NUM_TOKENS      (101)

// Local data
static BOOL token_pushed = FALSE;

// Global data
int		cur_token = UNKNOWN;			// current lexer token code
char	cur_text[256] = "";				// current text captured by lexer 
Flt		cur_value = 0;					// current number captured by lexer 
Vec		cur_vector = { 0, 0, 0 };		// current vector captured by lexer
int		yylinecount = 1;				// input file line count
FILE*	yyin = NULL;					// pointer to input file

// Token descriptor structure
typedef struct t_token_descriptor {
	LPCSTR	name;
	token_t value;
	LPCSTR  desc;
} TokenDesc, *PTOKENDESC;

// Token descriptor table
static TokenDesc token_list[] = {
	"vv_newfile",   NEWFILE,		"Includes another source file",
	"vv_popfile",   POPFILE,		"Closes the current source file",
	"studio",       STUDIO,			"Studio block",
	"from",         FROM,			"Viewpoint location",
	"at",           AT,				"The point to observe",
	"up",           UP,				"Viewpoint's upward direction",
	"angle",        ANGLE,			"Camera lens angle in degrees",
	"resolution",   RESOLUTION,		"Screen resolution",
	"res",			RESOLUTION,		"Synonym for resolution",
	"background",   BKG,			"Background block",
	"bkg",          BKG,			"Synonym for background",
	"start",        START,			"Start line for the session",
	"stop",         STOP,			"Stop line for the session",
	"aspect",       ASPECT,			"View aspect ratio",
	"ambient",      AMBIENT,		"Surface ambient material",
	"amb",		    AMBIENT,		"Synonym for ambient",
	"haze",         HAZE,			"Haze color",
	"aperture",     APERTURE,		"Camera aperture size",
	"focal_length", FOCAL_LENGTH,	"Camera focal length",
	"antialias",    ANTIALIAS,		"Antialias mode",
	"depth",        DEPTH,			"Maximum recursion depth",
	"no_shadows",   NO_SHADOWS,		"Turn off shadows",
	"samples",      SAMPLES,		"Sample count for non-pinhole lens",
	"jitter",       JITTER,			"Enable camera jitter",
	"threshold",    THRESHOLD,		"Adaptive threshold",
	"caustics",     CAUSTICS,		"Turn off fake caustics",
	"no_exp_trans", NO_EXP_TRANS,	"Turn off exponential transparency",
	"no_antialias", NO_ANTIALIAS,	"Turn off antialias for a shape",
	"none",         NONE,			"No antialias mode",
	"corners",      CORNERS,		"Antialias by shooting corners",
	"adaptive",     ADAPTIVE,		"Adaptive antialias",
	"quick",        QUICK,			"Quick antialias",
	"bunching",     BUNCHING,		"Composite shape bunch size",
	"projection",   PROJECTION,		"Projection mode setting",
	"flat",         FLAT,			"Flat projection mode",
	"orthogonal",   ORTHO,			"Orthogonal projection mode",
	"ortho",		ORTHO,			"Synonym for orthogonal",
	"width",        WIDTH,			"Width of frustum",
	"fisheye",      FISHEYE,		"Fisheye projection mode",
	"no_parallax",  NO_PARALLAX,	"Synonym for flat",
	"transform",    TRANSFORM,		"Push new transformation",
	"transform_pop",TRANS_POP,		"Pop topmost transformation",
	"rotate",       ROTATE,			"Rotation",
	"translate",    TRANSLATE,		"Translation",
	"scale",        SCALE,			"Scaling",
	"light",        LIGHT,			"Light source block",
	"type",         TYPE,			"Type of light source",
	"point",        POINT,			"Point light source",
	"spherical",    SPHERICAL,		"Spherical light source",
	"spot",         SPOT,			"Spot light source",
	"direction",    DIRECTION,		"(reserved for future use)",
	"directional",  DIRECTIONAL,	"Directional light source",
	"min_angle",    MIN_ANGLE,		"Minimum angle",
	"max_angle",    MAX_ANGLE,		"Maximum angle",
	"no_spec",      NOSPEC,			"Turn off specular shine",
	"color",        COLOR,			"Surface or light color",
	"falloff",      FALLOFF,		"Light falloff rate",
	"surface",      SURFACE,		"Surface block",
	"surf",			SURFACE,		"Synonym for surface",
	"diffuse",      DIFFUSE,		"Surface diffuse material",
	"diff",			DIFFUSE,		"Synonym for diffuse",
	"specular",     SPECULAR,		"Surface specular material",
	"spec",			SPECULAR,		"Synonym for specular",
	"transparent",  TRANSPARENT,	"Surface diffuse material",
	"xpar",			TRANSPARENT,	"Synonym for transparent",
	"shine",        SHINE,			"Specular shine color",
	"ior",          IOR,			"Surface index of refraction",
	"fuzz",         FUZZ,			"Surface fuzziness",
	"texture",      TEXTURE,		"Material texture map",
	"bump",         BUMP,			"Surface bump",
	"pattern",      PATTERN,		"Procedural texture block",
	"checker",      CHECKER,		"Checker texture",
	"noise",        NOISE,			"Noise texture",
	"turbulence",   TURBULENCE,		"Turbulence block",
	"turb",			TURBULENCE,		"Synonym for turbulence",
	"offset",       OFFSET,			"Wave offset",
	"terms",        TERMS,			"Turbulence terms",
	"amplitude",    AMP,			"Wave amplitude",
	"amp",			AMP,			"Synonym for amplitude",
	"wave",         WAVE,			"Wave block",
	"wavelength",   LENGTH,			"Wave length",
	"blend",        BLEND,			"(reserved for future use)",
	"image",        IMAGE,			"Material texture map",
	"across",       ACROSS,			"Direction across texture map",
	"damping",      DAMPING,		"Wave damping",
	"phase",        PHASE,			"Wave phase",
	"cone",         CONE,			"Cone primitive block",
	"sphere",       SPHERE,			"Sphere primitive block",
	"polygon",      POLYGON,		"Polygon primitive block",
	"poly",			POLYGON,		"Synonym for polygon",
	"patch",        PATCH,			"Patch primitive block",
	"tri",			PATCH,			"Synonym for patch",
	"ring",         RING,			"Ring primitive block",
	"vertex",       VERTEX,			"Vertex point",
	"vtx",			VERTEX,			"Synonym for vertex",
	"points",       POINTS,			"Number of vertex points",
	"center",       CENTER,			"Center of primitive shape or clip",
	"position",     CENTER,         "Synonym for center",
	"radius",       RADIUS,			"Primitive shape, clip or light radius",
	"min_radius",   MIN_RADIUS,		"Ring shape or cone light minimum radius",
	"max_radius",   MAX_RADIUS,		"Ring shape or cone light maximum radius",
	"base",         BASE,			"Cone base point",
	"apex",         APEX,			"Cone apex point",
	"base_radius",  BASE_RADIUS,	"Cone base radius",
	"apex_radius",  APEX_RADIUS,	"Cone apex radius",
	"normal",       NORMAL,			"Plane normal vector",
	"clip",         CLIP,			"Clip block",
	"inside",       INSIDE,			"Inside clip type",
	"outside",      OUTSIDE,		"Outside clip type",
	"global_clip",  GLOBAL_CLIP,	"Push global clip",
	"clip_pop",     CLIP_POP,		"Pop global clip",
	NULL,			UNKNOWN,		NULL
};


/*
	Gets the next token from the input stream and
	returns it's value.  If the previous token has been
	pushed back, then it is returned again.
*/

token_t get_token(void) {

	int     c, i;
	char    *cptr, *punctuation="{}()+*/";

	if (token_pushed) {
		token_pushed = 0;
		return cur_token;
	}

	// no token waiting so we have to build one

	cur_text[0] = 0;
	i = 0;

	// get rid of any whitespace
	while (isspace(c = fgetc(yyin))) {
		if (c == '\n') {
			yylinecount++;
		}
	}

	// check for end of file
	if (c == EOF) {
		cur_token = END_OF_FILE;
		return cur_token;
	}

	// check for punctuation or math symbols
	if ((cptr = strchr(punctuation, c))
		!= NULL) {
		switch (*cptr) {
			case '{' : cur_token = LEFT_BRACE; break;
			case '}' : cur_token = RIGHT_BRACE; break;
			case '(' : cur_token = LEFT_PAREN; break;
			case ')' : cur_token = RIGHT_PAREN; break;
			case '+' : cur_token = PLUS; break;
			case '*' : cur_token = TIMES; break;
			case '/' : cur_token = DIVIDE; break;
			default :
				fprintf(stderr, "Bogus punctuation token %c aka 0x%02x\n", c, c);
				last_call(1);
				break;
		}       // end of punctuation switch
		return cur_token;
	}

	// handle minu sign oddly because of unary minus problems
	if (c == '-') {
		c = fgetc(yyin);
		if (isspace(c)) {       // treat as a minus
			cur_token = MINUS;
			return cur_token;
		} else {
			ungetc(c, yyin);        // push it back and let the number routine handle it
			c = '-';
			// note that this falls through
		}
	}

	// check for a quoted string
	if (c== '\"') {   // start of quoted string, most likely a file name
		while ((c = fgetc(yyin)) != '\"') {
			cur_text[i++] = c;
		}
		cur_text[i] = 0;        // null terminate the string
		cur_token = UNKNOWN;
		return cur_token;
	}

	// check for keyword
	if (isalpha(c)) {         // must be a keyword
		do {
			cur_text[i++] = c;
			c = fgetc(yyin);
		} while (isalnum(c) || c=='_' || c=='.' || c=='\\' || c==':');
		ungetc(c, yyin);        // push back the character that doesn't belong

		cur_text[i] = 0;        // null terminate

		cur_token = match_token();
		return cur_token;
	}

	// last chance, try for a number
	if (isdigit(c) || c=='.' || c=='-') {
		do {
			cur_text[i++] = c;
			c = fgetc(yyin);
			if (c=='e' || c=='E') {
				cur_text[i++] = c;
				c = fgetc(yyin);        // should be +/-
				cur_text[i++] = c;
				c = fgetc(yyin);
			}
		} while (isdigit(c) || c=='.');
		ungetc(c, yyin);        // push back the character that doesn't belong

		cur_text[i] = 0;
		cur_value = atof(cur_text);
		cur_token = NUMBER;

		// printf("\"%s\" = %g\n", cur_text, cur_value);

		return cur_token;
	}

	// if we get down here something is really wrong

	fprintf(stderr, "\nError parsing.  Found the character '%c' aka 0x%02x and\n", c, c);
	fprintf(stderr, "I don't know what to do with it.\n");
	
	// Never returns
	yyerror("");

	return UNKNOWN;		// Keep lint/compilers quiet
}


/*
	Sets a flag which indicates that the previous
	token was not used by the parser and should therefore
	be returned by the next call to get_token.
*/

token_t push_token(void) {

	token_pushed = 1;

	return cur_token;

}


/*
	Compares the string in cur_test with the list
	of valid tokens.  If a match is found then the token's
	value is returned.  If no match is found then UNKNOWN
	is returned.  This could be sped up quite a bit by sorting
	the list of tokens alphabetically and doing a binary search.
	Note that this only compares the first "length" characters.
	This allows the parser to recognize abbreviations such
	as "surf" for "surface".  This feature can cause some
	problems eg the token "transform" could be mistaken for
	an abbreviation for the token "transform_pop".  Therefore,
	"transform" must come in the list before "transform_pop".
	Since a binary search will not ensure the order of the
	tokens being compared (with a LOT of work) the ability to
	recognize abbreviations would have to be eliminated before
	a binary search was used.  Speed vs flexibility.  Classic.
	Of course a dozen other ways to get both speed and
	flexibilty should be quite easy to come up with.
*/

token_t match_token(void) {

	return match_token_custom(cur_text);

}


// Match token custom
token_t match_token_custom(LPCSTR name) {

	int length = strlen(name);

	for (int i = 0; token_list[i].name; i++) {
		if (!strncmp(name, token_list[i].name, length)) {
			cur_token = token_list[i].value;
			return cur_token;
		}
	}

	return UNKNOWN;
}


// Read a number
void get_number(void) {

	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}

}


// Read a vector
void get_vector(void) {

	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}
	cur_vector[0] = cur_value;
	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}
	cur_vector[1] = cur_value;
	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}
	cur_vector[2] = cur_value;

}


// Lookup name for a token
LPCSTR lookup_token_name(token_t token) {

	for (int i = 0; token_list[i].name; i++) {
		if (token_list[i].value == token) {
			return token_list[i].name;
		}
	}
    return "UNKNOWN";
}


// Print all token codes and their names
void PrintTokens() {

	fprintf(stderr, "Token Codes:\n");

	for (int i = 0; token_list[i].name; i++) {
		fprintf(stderr, "%4u\t %-20s %s\n",
			token_list[i].value,
			token_list[i].name,
			token_list[i].desc
		);
	}

}

// Show extended help for a token
void TokenHelp(LPCSTR token) {
	token_t tok = match_token_custom(token);
	if (tok == UNKNOWN) {
		fprintf(stderr, "Unknown token: '%s'\n", token);
		return;
	}
	OpenHelpFile(token);
}


