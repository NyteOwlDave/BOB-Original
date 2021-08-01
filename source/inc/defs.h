/*
	defs.h

	Definitions for everything under the sun

*/

#ifndef DEFS_DEFINED
#define DEFS_DEFINED


#include <stdio.h>
#include "config.h"

#define HUGE    (1.0e+8)
#define TINY	(1.0e-6)

#define FALSE	0
#define TRUE	1

#define PI 		(3.14159265358979323844)
#define D2R		(PI/180)
#define R2D		(180/PI)

#define PTR_CHECK(p) ptrchk(p,__FUNCTION__)


/*----------------------------------------------------------------------*/

// Math Macros

#define ABS(a)		(((a)<0)?(-(a)):(a))
#define FLOOR(a)	((a)>0?(int)(a):-(int)(a))
#define CEILING(a)	((a)==(int)(a)?(a):(a)>0?1+(int)(a):-(1+(int)(-a))
#define ROUND(a)	((a)>0?(int)((a)+0.5):-(int)(0.5-a))

#define MIN(a,b)	(((a)<(b))?(a):(b))
#define MAX(a,b)	(((a)>(b))?(a):(b))

// Random number [0.0 .. 1.0]
#define rnd()			(((double) rand())/RAND_MAX)

// Random number [0.0 .. s]
#define srnd(s)			((s)*rnd())

// Random number [b .. a+b]
#define abrnd(a,b)		(((a)*rnd())+(b))

// Convert degrees to radians
#define degtorad(x)     (((Flt)(x))*D2R)

// Convert radians to degrees
#define radtodeg(x)     (((Flt)(x))*R2D)


/*----------------------------------------------------------------------*/

// Simple Type Definitions

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long QWORD;
typedef char* LPSTR;
typedef const char* LPCSTR;

// Simple callback function for commands
// No arguments
typedef void(*pfnCmd)(void);

// Simple callback function for data operations
// First argument's data structure is context-sensitive
typedef void(*pfnVar)(void*);

// Simple callback function for string output
// Argument is some string to be sent as a message
typedef void(*pfnMsg)(LPCSTR);

// Simple callback function for string modifier
// First argument is the destination string buffer
// Second argument is the source string buffer
typedef int(*pfnStr)(LPSTR, LPCSTR);


// Math Type Definitions

typedef double Flt;
typedef Flt	Vec[3];
typedef Vec	Point;
typedef Vec	Color;
typedef Flt	Matrix[4][4];


/*----------------------------------------------------------------------*/

// Vector Macros

#define MakeVector(x, y, z, v)		\
	(v)[0]=(x);						\
	(v)[1]=(y);						\
	(v)[2]=(z)

#define VecNegate(a)	\
	(a)[0]=(-(a)[0]);	\
	(a)[1]=(-(a)[1]);	\
	(a)[2]=(-(a)[2])

#define VecDot(a,b)		\
	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

#define VecLen(a)		\
	(sqrt(VecDot(a,a)))

#define VecCopy(a,b)	\
	(b)[0]=(a)[0];		\
	(b)[1]=(a)[1];		\
	(b)[2]=(a)[2]

#define VecAdd(a,b,c)		\
	(c)[0]=(a)[0]+(b)[0];	\
	(c)[1]=(a)[1]+(b)[1];	\
	(c)[2]=(a)[2]+(b)[2]

#define VecSub(a,b,c)		\
	(c)[0]=(a)[0]-(b)[0];	\
	(c)[1]=(a)[1]-(b)[1];	\
	(c)[2]=(a)[2]-(b)[2]

#define VecComb(A,a,B,b,c)			\
	(c)[0]=(A)*(a)[0]+(B)*(b)[0];	\
	(c)[1]=(A)*(a)[1]+(B)*(b)[1];	\
	(c)[2]=(A)*(a)[2]+(B)*(b)[2]

#define VecS(A,a,b)		\
	(b)[0]=(A)*(a)[0];	\
	(b)[1]=(A)*(a)[1];	\
	(b)[2]=(A)*(a)[2]

#define VecAddS(A,a,b,c)		\
	(c)[0]=(A)*(a)[0]+(b)[0];	\
	(c)[1]=(A)*(a)[1]+(b)[1];	\
	(c)[2]=(A)*(a)[2]+(b)[2]

#define VecMul(a,b,c)		\
	(c)[0]=(a)[0]*(b)[0];	\
	(c)[1]=(a)[1]*(b)[1];	\
	(c)[2]=(a)[2]*(b)[2]

#define VecCross(a,b,c)						\
	(c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1];		\
	(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2];		\
	(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0]

#define VecZero(v)		\
	(v)[0]=0.0;			\
	(v)[1]=0.0;			\
	(v)[2]=0.0

#define VecPrint(msg,v)		\
	fprintf(stderr,			\
	"%s: %g %g %g\n", msg,	\
	(v)[0],(v)[1],(v)[2])


/*----------------------------------------------------------------------*/

// Ray

// Ray structure
typedef struct Ray {
	Point P;	// Point of origin
	Point D;	// Direction vector
} Ray, *PRAY;

// Ray projection
#define RayPoint(ray,t,point)	\
	VecAddS(t,(ray)->D,(ray)->P,point)


/*----------------------------------------------------------------------*/

// Texture Map

//
// Texture maps use three separate color planes. Each plane consists
// of an array of scanline pointers. Each scanline is an array of
// byte values [0 .. 255] for the texel color value in that channel. 
//

// Color plane type
typedef BYTE** PTEXPLANE;

// Texture map structure
typedef struct t_texmap {
	Vec			position;       // upper left hand corner of image
	Vec			normal;         // same as projection direction
	Vec			across;         // across top of image to upper right
	Vec			down;           // down to lower left
	Flt			scale;          // defaults to 1.0, #units across full image
	int			xres;			// image width
	int			yres;			// image height
	PTEXPLANE	red;			// red color plane
	PTEXPLANE	grn;			// green color plane
	PTEXPLANE 	blu;			// blue color plane
} Texmap, *PTEXMAP;


/*----------------------------------------------------------------------*/

// Surface

// Surface structure
typedef struct t_surface {
	Color				cshine;		// spec spot color
	Color				amb;		// ambient
	Color				diff;		// diffuse
	Color				spec;		// specular (reflected)
	Color				trans;		// transparency
	Flt					shine;		// specular spot exponent
	Flt					ior;		// index of refraction
	Flt					fuzz;		// surface fuzz
	int					flags;		// is this surface valid for shadow caching
	Texmap*				tm_amb;		// ambient texture map (overrides amb color)
	Texmap*				tm_diff;	// diffuse texture map (overrides diff color)
	Texmap*				tm_spec;	// specular texture map (overrides spec color)
	Texmap*				tm_trans;	// transparency texture map (overrides trans color)
	struct t_texture*	tex;		// procedural texture
	struct t_bump*		bump;		// surface normal texture
	Matrix				matrix;		// transformation matrix
} Surface, *PSURFACE;

// Surface flags
#define S_CACHE         (0x0001)	// Surface is cacheable
#define S_NO_ANTIALIAS  (0x0002)	// Don't anti-alias
#define S_TRANSFORM     (0x0004)	// Transform provided
#define S_TM_DIFF       (0x0008)	// Has diffuse component
#define S_TM_SPEC       (0x0010)	// Has specular component
#define S_TM_TRANS      (0x0020)	// Has transparent component
#define S_TM_AMB        (0x0040)	// Has ambient component
#define S_TM_MAPPING    (0x0078)    // All bits for mapping


/*----------------------------------------------------------------------*/

// Turbulence

// Turbulence structure
typedef struct t_turbulence {
	int		terms;          // # of terms in the series
	Vec		trans;          // pre-process number
	Vec		scale;          // pre-process number
	Flt     amp;            // post turbulence amplification
} Turbulence, *PTURBULENCE;


/*----------------------------------------------------------------------*/

// Wave

// Wave structure
typedef struct t_wave {
	Vec		center;			// Center point
	Flt		wavelength;		// Wavelength
	Flt		amp;			// Amplitude - should be about 0 to 0.6
	Flt		damp;			// damping per wavelength
	Flt		phase;			// wavelength offset
	struct t_wave* next;	// next wave in list
} Wave, *PWAVE;


/*----------------------------------------------------------------------*/

// Procedural Texture

// Texture structure
typedef struct t_texture {
	Flt			(*func)();		// returns 0.0..1.0
	Flt			blur;			// % blending between layers
	Flt			fuzz;			// white noise blending
	Flt			r1, r2;			// just because
	int			terms;			// for tex_noise()
	Vec			trans;			// translation
	Vec			scale;			// scale
	Surface*	surf[2];		// (?)
	Turbulence*	turbulence;		// turbulence
	Wave*		waves;			// waves list
} Texture, *PTEXTURE;


/*----------------------------------------------------------------------*/

// Bump

//
// Bumps are used to tweak surface normals
//

// Bump structure
typedef struct t_bump {
	Vec				trans;			// apply to resulting normal
	Vec				scale;			// (?)
	Turbulence*		turbulence;		// turbulence
	Wave*			waves;			// wave list
} Bump, *PBUMP;


/*----------------------------------------------------------------------*/

// Stack

//
// General purpose stack
// Can store data of any kind
// Employs a linked list structure
//

// Stack structure
typedef struct t_stack {
	void*			what;	// Data for stack entry
	struct t_stack*	prev;	// Pointer to previous stack entry
} Stack, *PSTACK;


/*----------------------------------------------------------------------*/

// Input file

// Input file structure (lexer)
typedef struct t_infile {
	char*				file_name;		// Filename
	FILE*				fp;				// File stream pointer
	long				line;			// Current line number
	struct t_infile*	next;			// Next file in linked list
} Infile, *PINFILE;


/*----------------------------------------------------------------------*/

// Light

// Light type constants
#define L_DIRECTIONAL	(1)		// Direction only (no location)
#define L_POINT			(2)		// Location only (no direction)
#define L_SPHERICAL		(3)		// No umbra/penumbra
#define L_SPOT			(4)		// Has umbra/penumbra

// Light illumination constants
#define L_INFINITE		(0)		// No falloff
#define L_R				(1)		// Linear falloff
#define L_R_SQUARED		(2)		// Exponential falloff

// Light flags
#define L_NOSHADOWS		(1)		// Disable shadows
#define L_NOSPEC		(2)		// Disable specular dot

typedef struct t_light {
	Vec					position;					// where light is located in world space
	Vec					dir;						// unit direction vector
	Color				color;						// radiant color
	Flt					radius;						// radius/spherical, max_angle/spotlight
	Flt					min_angle;					// minimum angle for spot lights
	Flt					max_angle;					// maximum angle for spot lights
	short				type;						// what type is this?
	short				illum;						// how does the light fall off?
	short				flag;						// noshadows? nospec?
	short				samples;					// num samples for spherical light
	struct t_object*	light_obj_cache[MAXLEVEL];	// cached object light has hit
	struct t_light*		next;						// next light in list
} Light, *PLIGHT;


/*----------------------------------------------------------------------*/

// Viewpoint/Eye

//
// The system defines a single viewpoint for the scene.
// The view contains the following information:
//
// Point in world space where the eye is located.
// Point in world space that the eye is looking at.
// Unit up vector.
// Two fov (field of view angles).
// Aspect ratio.
//

// Viewpoint structure
typedef struct t_viewpoint {
	Vec		view_from;			// location in world space
	Vec		view_at;			// point we're looking at
	Vec		view_up;			// unit up direction
	Flt		view_angle_x;		// horizontal fov angle
	Flt		view_angle_y;		// vertical fov angle
	Flt		view_aspect;		// aspect ratio
} Viewpoint, *PVIEWPOINT;


/*----------------------------------------------------------------------*/

// Camera

// Camera structure
typedef struct t_camera {
	short   projection;		// flat, orthographic, fisheye, no parallax
	Vec		lens_i;			// horizontal vector across lens
	Vec		lens_j;			// vertical vector across lens
	Flt		aperture;		// radius of lens
	Flt		focal_length;   // how far away are we focussed
	short   samples;        // number of samples for non-pinhole camera
} Camera, *PCAMERA;

// Camera projections types
#define		P_FLAT			(0)
#define		P_ORTHOGRAPHIC	(1)
#define		P_FISHEYE		(2)
#define		P_NO_PARALLAX	(3)


/*----------------------------------------------------------------------*/

// Clipping

// Clip flags
#define C_PLANE         (0x01)
#define C_SPHERE        (0x02)
#define C_CONE          (0x04)
#define C_INSIDE        (0x10)

// Clip structure
typedef struct t_clip {
	Vec				center;		// location in world space
	Vec				normal;		// unit normal
	Vec				apex;		// apex point (cone)
	Vec				base;		// base point (cone)
	Flt				radius1;	// radius #1
	Flt				radius2;	// radius #2
	Flt				length;		// length (cone)
	int				type;		// clip type
	struct t_clip*	next;		// ptr to next clip in linked list
} Clip, *PCLIP;

// Global clip structure
typedef struct t_global_clip {
	Clip*					clip;	// ptr to linked list of clips
	struct t_global_clip*	next;	// ptr to next global clip in linked list
} GlobalClip, *PGLOBALCLIP;


/*----------------------------------------------------------------------*/

// Primitive Object

// Object procedures LUT
typedef struct t_objectprocs {			// function v-table
    int		(*intersect) ();			// calculate intersection 
    void	(*normal) ();				// calculate surface normal
} ObjectProcs;							// ptr to function v-table
typedef ObjectProcs* POBJECTPROCS;

// Primitive object structure
typedef struct t_object {
	WORD 				o_type;				// cone, ring, tri, etc.
	void*				o_data;				// ptr to type specific data
	Flt					o_dmin[NSLABS];		// minimum distance (bounding slabs)
	Flt					o_dmax[NSLABS];		// maximum distance (bounding slabs)
	Surface*			o_surf;				// ptr to surface
	Clip*				clips;				// ptr to list of clips
	POBJECTPROCS        o_procs;			// ptr to function v-table
	struct t_object*	next;				// next primitive object in linked list
} Object, *POBJECT;

// Intersection v-table function pointer
// typedef int(*pfnIntersect)(Object*, Ray*, Isect*);

// Normal v-table function pointer
// typedef int(*pfnNormal)(Object*, Isect*, Point, Vec);

typedef struct t_objectprocs ObjectProcs;

// Primitive object types
#define		T_COMPOSITE	(0)
#define		T_SPHERE	(1)
#define		T_POLY		(2)
#define		T_CONE		(3)
#define		T_RING		(4)
#define		T_TRI		(5)
#define		T_FUZZY		(6)


/*----------------------------------------------------------------------*/

// Composite Object Data

//
// A composite object is a linked list of primitives.
// Generally these primitives are lumped together in a "bunch" which
// is a group of shapes in close proximity in world space.
// Grouping in bunches is used by the bounding slabs algorithm to
// improve efficiency (by reducing intersection detection time).
//

// Composite object data structure
typedef struct t_compositedata {
	DWORD		size;		// number of primitives in bunch
	Object*		children;	// ptr to linked list of primitives
} CompositeData, *PCOMPOSITEDATA;


/*----------------------------------------------------------------------*/

// Intersection

//
// Intersection of a ray with a shape (primitive object).
//
 
// Intersection structure
typedef struct t_isect {
	Flt 		isect_t;		// distance to intersection
	int 		isect_enter;	// entering? ie hit front?
	Object*		isect_prim;		// object we hit
	Surface*	isect_surf;		// surface def of hit object
	Object*		isect_self;		// pointer to self for queue elimination
} Isect, *PISECT;


/*----------------------------------------------------------------------*/

// Transform

// Transform structure
typedef struct t_transform {
	Matrix                  mat;	// matrix 4x4
	struct t_transform*		next;	// ptr to next transform in linked list
} Transform, *PTRANSFORM;


/*----------------------------------------------------------------------*/

// Pixel

// Pixel structure
#pragma pack(1)
typedef struct t_pixel {
	BYTE r;		// red channel
	BYTE g;		// green channel
	BYTE b;		// blue channel
	BYTE q;		// misc. channel
} Pixel, *PPIXEL;
#pragma pack()

#define COLOR_TO_PIXEL(color,pixel)			\
	(pixel).r = (BYTE)(255*(color)[0]);		\
	(pixel).g = (BYTE)(255*(color)[1]);		\
	(pixel).b = (BYTE)(255*(color)[2])

#define PIXEL_TO_COLOR(color,pixel)			\
	(color)[0] = (Flt)((pixel).r) / 255;	\
	(color)[1] = (Flt)((pixel).g) / 255;	\
	(color)[2] = (Flt)((pixel).b) / 255


/*----------------------------------------------------------------------*/

// Palette

//
// Palettes are color arrays.
// Each color is an RGB triplet.
// There are 256 color in a palette.
//

// Palette type definition
typedef BYTE Palette[256][3];
typedef Palette* PPALETTE;

//
// RGBColor and RGBpalette definitions are provided to make
// the above Palette structure more easily understandable.
//

// RGBColor Definition
#pragma pack(1)
typedef struct t_rgb_color {
	BYTE	r;
	BYTE	g;
	BYTE	b;
} RGBColor, *PRGBCOLOR;
#pragma pack()

// RGBPalette Definition
typedef struct t_rgb_palette {
	RGBColor color[256];
} RGBPalette, *PRGBPALETTE;

// Macro to cast Palette to RGBPalette pointer
#define MakeRGBPalette(pal) ((PRGBPALETTE)(pal))


/*----------------------------------------------------------------------*/

// Background

//
// The background determines the color for any ray which
// has not intersected a shape. This may be a simple color
// or it may be a shaded color, as with striking a plane
// from a specific direction.
//

// Background structure
typedef struct t_background {
	Color		color;		// ambient color
	Vec			up;			// surface normal
	Palette		pal;		// palette for shading
} Background, *PBACKGROUND;


/*----------------------------------------------------------------------*/

// Preprocessor Macro

// Preprocessor macro structure
typedef struct t_macro {
	LPSTR				macro;		// define'd word
	int					mlen;		// length of macro
	LPSTR				text;		// text to replace macro with
	int					tlen;		// length of text
	struct t_macro*		next;		// ptr to next macro in linked list
} Macro, *PMACRO;


/*----------------------------------------------------------------------*/

// Anti-alias Modes

// Anti-alias Modes
#define		A_NONE			(0)
#define		A_CORNERS		(1)
#define		A_QUICK			(2)
#define		A_ADAPTIVE		(3)


/*----------------------------------------------------------------------*/

// Dynamic String

// Dynamic string structure
typedef struct t_string {
	LPCSTR data;		// String dynamic data (ASCIIZ)
	int len;			// String length
} String, *PSTRING;


/*----------------------------------------------------------------------*/

// Extended Macro

// Extended macro structure
typedef struct t_macro_ex {
	PSTRING name;	// Macro name
	PSTRING text;	// Macro text
	struct t_macro_ex* next;
} MacroEx, *PMACROEX;


#endif // !DEFS_DEFINED

