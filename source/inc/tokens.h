/*
	tokens.h

	Lexer token definitions

*/

#ifndef TOKENS_DEFINED
#define TOKENS_DEFINED


/*--------------------------------------------------------------------------*/

// Miscellaneous

#define END_OF_FILE		(-1)
#define UNKNOWN			(0)
#define NEWFILE			(1)
#define POPFILE			(2)
#define NUMBER			(3)


/*--------------------------------------------------------------------------*/

// Punctuation

#define LEFT_BRACE		(10)
#define RIGHT_BRACE		(11)
#define LEFT_PAREN		(12)
#define RIGHT_PAREN		(13)
#define PLUS			(14)
#define MINUS			(15)
#define TIMES			(16)
#define DIVIDE			(17)


/*--------------------------------------------------------------------------*/

// Studio

#define STUDIO			(20)
#define FROM			(21)
#define AT				(22)
#define UP				(23)
#define ANGLE			(24)
#define RESOLUTION		(25)
#define BKG				(26)
#define START			(27)
#define STOP			(28)
#define ASPECT			(29)
#define AMBIENT			(30)
#define HAZE			(31)
#define APERTURE		(32)
#define FOCAL_LENGTH	(33)
#define ANTIALIAS		(34)
#define DEPTH			(35)
#define NO_SHADOWS		(36)
#define SAMPLES			(37)
#define JITTER			(38)
#define THRESHOLD		(39)
#define CAUSTICS		(40)
#define NO_EXP_TRANS	(41)
#define NO_ANTIALIAS	(42)
#define NONE			(43)
#define CORNERS			(44)
#define ADAPTIVE		(45)
#define QUICK			(46)
#define BUNCHING		(47)
#define PROJECTION		(48)
#define FLAT			(49)
#define ORTHO			(50)
#define WIDTH			(51)
#define FISHEYE			(52)
#define NO_PARALLAX		(53)


/*--------------------------------------------------------------------------*/

// Transformations

#define TRANSFORM		(70)
#define TRANS_POP		(71)
#define ROTATE			(72)
#define TRANSLATE		(73)
#define SCALE			(74)


/*--------------------------------------------------------------------------*/

// Lights

#define LIGHT			(80)
#define TYPE			(81)
#define POINT			(82)
#define SPHERICAL		(83)
#define SPOT			(84)
#define DIRECTIONAL		(85)
#define MIN_ANGLE		(86)
#define MAX_ANGLE		(87)
#define NOSPEC			(88)
#define COLOR			(89)
#define FALLOFF			(90)
#define DIRECTION		(91)


/*--------------------------------------------------------------------------*/

// Surfaces

#define SURFACE			(100)
#define DIFFUSE			(101)
#define SPECULAR		(102)
#define TRANSPARENT		(103)
#define SHINE			(104)
#define IOR				(105)
#define FUZZ			(106)
#define TEXTURE			(107)
#define BUMP			(108)
#define PATTERN			(109)
#define CHECKER			(110)
#define NOISE			(111)
#define TURBULENCE		(112)
#define OFFSET			(113)
#define TERMS			(114)
#define AMP				(115)
#define WAVE			(116)
#define LENGTH			(117)
#define BLEND			(118)
#define IMAGE			(119)
#define ACROSS			(120)
#define DAMPING			(121)
#define PHASE			(122)


/*--------------------------------------------------------------------------*/

// Primitive Shapes

#define CONE			(130)
#define SPHERE			(131)
#define POLYGON			(132)
#define PATCH			(133)
#define RING			(134)
#define VERTEX			(135)
#define POINTS			(136)
#define CENTER			(137)
#define RADIUS			(138)
#define MIN_RADIUS		(139)
#define MAX_RADIUS		(140)
#define NORMAL			(141)
#define BASE			(142)
#define APEX			(143)
#define BASE_RADIUS		(144)
#define APEX_RADIUS		(145)


/*--------------------------------------------------------------------------*/

// Clipping

#define CLIP			(160)
#define INSIDE			(161)
#define OUTSIDE			(162)
#define GLOBAL_CLIP		(163)
#define CLIP_POP		(164)


/*--------------------------------------------------------------------------*/

// Token code type definition
typedef int token_t;

// Global data
extern int 				yylinecount;		// interpreter line count
extern FILE*			yyin;				// ptr to interpreter input file stream
extern token_t			cur_token;			// current token code captured by lexer
extern char				cur_text[];			// current text captured by lexer
extern Flt				cur_value;			// current number captured by lexer
extern Vec				cur_vector;			// current vector captured by lexer

// Push last token
extern token_t push_token(void);

// Match last token
extern token_t match_token(void);

// Match token custom
extern token_t match_token_custom(LPCSTR name);

// Read next token from input file
extern token_t get_token(void);

// Reads current value into current number
extern void get_number(void);

// Reads current value and two more values into current vector
extern void get_vector(void);

// Prints all token codes and their names
extern void PrintTokens();

// Show extended help for a token
extern void TokenHelp(LPCSTR token);

// Lookup name for a token
extern LPCSTR lookup_token_name(token_t token);


#endif // !TOKENS_DEFINED

