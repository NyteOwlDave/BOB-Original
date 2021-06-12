/*
	LANG-PARSE.C

	Language parser

	[x] Done
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "inc/bob.h"
#include "inc/tokens.h"

/*==========================================================================*/

// Function Prototypes

// Lower level functions (within blocks)
static void xx_studio_background(void);
static Texture* xx_texture(void);
static Bump* xx_bump(void);
static Texmap* xx_texmap(void);
static Turbulence* xx_turbulence(void);
static Wave* xx_wave(void);
static Clip* xx_clip(void);

// Studio (top level)
static void yy_studio(void);
static void yy_light(void);

// Material (top level)
static Surface* yy_surface(void);

// Transform (top level)
static void yy_transform(void);
static void yy_transform_pop(void);

// Global clip (top level)
static void yy_global_clip(void);
static void yy_clip_pop(void);

// Primitive shapes (top level)
static void yy_sphere(void);
static void yy_cone(void);
static void yy_ring(void);
static void yy_polygon(void);
static void yy_patch(void);


/*==========================================================================*/

// Local Data

// Temporary vector for get_vec() and read_vec()
static Vec tmp_vec;


/*==========================================================================*/

// Local Helper Functions

/*
	get_num() -- get a number.

	Tries to convert the last token text to a number
	cur_text --> cur_value

	Number is returned in cur_value.
*/
static inline void get_num(void) {
	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}
}

/*
	get_vec() -- get a vector.

	Tries to convert the last token text, plus two additional
	tokens to a vector.

	Vector is returned in tmp_vec.
*/
static void get_vec(void) {
	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}
	tmp_vec[0] = cur_value;
	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}
	tmp_vec[1] = cur_value;
	if (get_token() != NUMBER) {
		yyerror("Number expected but not found.");
	}
	tmp_vec[2] = cur_value;
}

// Fetches the next input token 
// Interprets it as a vector
static inline void read_vec(Flt* vec) {
	get_vec();
	vec[0] = tmp_vec[0];
	vec[1] = tmp_vec[1];
	vec[2] = tmp_vec[2];
}

// Fetches the next input token 
// Interprets it as a floating point value
static inline Flt read_float() {
	get_num();
	return (Flt)cur_value;
}

// Fetches the next input token 
// Interprets it as an integer
static inline int read_int() {
	get_num();
	return (int)cur_value;
}

// Fetches the next input token 
// Interprets it as a boolean flag
static inline BOOL read_flag() {
	get_num();
	return (BOOL)cur_value;
}

// Register new primitive shape
static void register_shape(Object* obj) {
	// Insert into list of primitive shapes
	obj->next = Root;
	Root = obj;
	nPrims++;
	yystats();
}


/*==========================================================================*/

// Parser Entry Point

/*
	yyparse() -- The main entry point into the parser.
	
	tokens:

		vv_newfile, vv_popfile
		studio, light, surface 
		transform, trans_pop 
		global_clip, clip_pop 
		sphere, cone, ring, polygon, patch
*/

int yyparse(void) {

    LogPrint("Parsing");

	while (get_token() != END_OF_FILE) {
		switch (cur_token) {
			case NEWFILE:
				// get file name
				get_token();
				// open file
				yy_newfile(cur_text);
				break;
			case POPFILE:
				// close file
				yy_popfile();
				break;
				// Studio type
			case STUDIO:		yy_studio();			break;
			case LIGHT:			yy_light();				break;
				// Material type
			case SURFACE:		yy_surface();			break;
				// Transform type
			case TRANSFORM:		yy_transform();			break;
			case TRANS_POP:		yy_transform_pop();		break;
				// Clip type
			case GLOBAL_CLIP:	yy_global_clip();		break;
			case CLIP_POP:		yy_clip_pop();			break;
				// Shape types
			case SPHERE:		yy_sphere();			break;
			case CONE:			yy_cone();				break;
			case RING:			yy_ring();				break;
			case POLYGON:		yy_polygon();			break;
			case PATCH:			yy_patch();				break;
				// Unknown token
			case UNKNOWN:
				fprintf(stderr, "\nError parsing, found token '%s'\n", cur_text);
				yyerror("Unknown token.\n");
				break;
				// Syntax error
			default:
				fprintf(stderr, "\nError parsing, found token %d '%s'\n", cur_token, cur_text);
				yyerror("Wrong context.\n");
				break;
		}
	}

    LogPrint("Parsing completed successfully");

	return 0;

}


/*==========================================================================*/

// STUDIO Structure

/*
	yy_studio() -- Parse the studio structure

	tokens:

		from, at, up, angle, width, resolution, start, stop
		aspect, bunching, ambient, bkg, aperture, focal_length
		samples, projection, antialias, jitter, threshold
		haze, depth, no_shadows, caustics, no_exp_trans

	default studio settings:

		aspect 1.0
		ambient 0, 0, 0
		background color 0 0 0
		background up 0 0 0
		camera aperture 0.0
		focal length 0.0
		samples F_SAMPLES
		projection flat
		haze density 0.0
		start line 0
		stop line -1
*/

// KEYWORD DECODERS

// static void xx_studio_[KEYWORD](void) {}

// Decode PROJECTION keyword
// Tokens: flat, ortho, fisheye, no_parallax
static void xx_studio_projection(void) {
	get_token();
	switch (cur_token) {
	case FLAT:
		// NOTE: Same effect as P_NO_PARALLAX
		camera.projection = P_FLAT;
		break;
	case ORTHO:
		// Plain ole perspective projection
		camera.projection = P_ORTHOGRAPHIC;
		break;
	case FISHEYE:
		// Weird roundy stuff
		camera.projection = P_FISHEYE;
		break;
	case NO_PARALLAX:
		// NOTE: Same effect as P_FLAT
		camera.projection = P_NO_PARALLAX;
		break;
	default:
		yyerror("Unknown projection type.");
		break;
	}
}

// Decode ANTIALIAS keword
// Tokens: none, corners, quick, adaptive
static void xx_studio_antialias(void) {
	get_token();
	switch (cur_token) {
	case NONE:
		antialias = A_NONE;
		break;
	case CORNERS:
		antialias = A_CORNERS;
		break;
	case QUICK:
		antialias = A_QUICK;
		break;
	case ADAPTIVE:
		antialias = A_ADAPTIVE;
		break;
	default:
		yyerror("Unknown antialias type.");
		break;
	}
}

// Decode AMBIENT keyword
static void xx_studio_ambient(void) {
	read_vec(Ambient);
}

// Decode AT keyword
static void xx_studio_at(void) {
	read_vec(Eye.view_at);
}

// Decode FROM keyword
static void xx_studio_from(void) {
	read_vec(Eye.view_from);
}

// Decode UP keyword
static void xx_studio_up(void) {
	read_vec(Eye.view_up);
}

// Decode ANGLE keyword
static void xx_studio_angle(void) {
	get_num(); Eye.view_angle_x = degtorad(cur_value / 2.0);
}

// Decode APERTURE keyword
static void xx_studio_aperture(void) {
	camera.aperture = read_float();
}

// Decode ASPECT keyword
static void xx_studio_aspect(void) {
	Eye.view_aspect = read_float();
}

// Decode FOCAL_LENGTH keyword
static void xx_studio_focal_length(void) {
	camera.focal_length = read_float();
}

// Decode HAZE keyword
static void xx_studio_haze(void) {
	HazeDensity = read_float();
}

// Decode WIDTH keyword
static void xx_studio_width(void) {
	Eye.view_angle_x = read_float();
}

// Decode BUNCHING keyword
static void xx_studio_bunching(void) {
	bunching = read_int();
}

// Decode DEPTH keyword
static void xx_studio_depth(void) {
	maxlevel = read_int();
}

// Decode SAMPLES keyword
static void xx_studio_samples(void) {
	camera.samples = read_int();
}

// Decode START keyword
static void xx_studio_start(void) {
	start_line = read_int();
}

// Decode STOP keyword
static void xx_studio_stop(void) {
	stop_line = read_int();
}

// Decode THRESHOLD keyword
static void xx_studio_threshold(void) {
	adapt_dist = read_int();
}

// Decode RESOLUTION keyword
static void xx_studio_resolution(void) {
	Xresolution = read_int();
	Yresolution = read_int();
}

// Decode JITTER keyword
static void xx_studio_jitter(void) {
	jitter = TRUE;
}

// Decode NO_SHADOWS keyword
static void xx_studio_no_shadows(void) {
	no_shadows = TRUE;
}

// Decode CAUSTICS keyword
static void xx_studio_caustics(void) {
	caustics = TRUE;
}

// Decode NO_EXP_TRANS keyword
static void xx_studio_no_exp_trans(void) {
	exp_trans = FALSE;
}


// Decode STUDIO structure
static void yy_studio(void) {

    // PREAMBLE

	Vec     tmp;

    LogPrint("yy_studio");

	// assign defaults
	Eye.view_aspect = 1.0;					// 90 degrees
	MakeVector(0, 0, 0, Ambient);			// no global illumination
	MakeVector(0, 0, 0, background.color);	// black background
	MakeVector(0, 0, 0, background.up);		// no background plane
	camera.aperture = 0.0;					// pinhole camera
	camera.focal_length = 0.0;				// fix later
	camera.samples = F_SAMPLES;				// for non pinhole camera
	camera.projection = P_FLAT;				// flat projection
	HazeDensity = 0.0;						// on a clear day...
	start_line = 0;							// session start line
	stop_line = (-1);						// render all lines

	// eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.\n");
	}

	// read all up through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case AMBIENT:		xx_studio_ambient();		break;	// vec
			case AT:			xx_studio_at();				break;	// vec
			case FROM:			xx_studio_from();			break;	// vec
			case UP:			xx_studio_up();				break;	// vec
			case ANGLE:			xx_studio_angle();			break;	// float
			case APERTURE:		xx_studio_aperture();		break;	// float
			case ASPECT:		xx_studio_aspect();			break;	// float
			case FOCAL_LENGTH:	xx_studio_focal_length();	break;	// float
			case HAZE:			xx_studio_haze();			break;	// float
			case WIDTH:			xx_studio_width();			break;	// float
			case BUNCHING:		xx_studio_bunching();		break;	// int
			case DEPTH:			xx_studio_depth();			break;	// int
			case SAMPLES:		xx_studio_samples();		break;	// int
			case START:			xx_studio_start();			break;	// int
			case STOP:			xx_studio_stop();			break;	// int
			case THRESHOLD:		xx_studio_threshold();		break;	// int
			case RESOLUTION:	xx_studio_resolution();		break;	// int
			case JITTER:		xx_studio_jitter();			break;	// flag
			case NO_SHADOWS:	xx_studio_no_shadows();		break;	// flag
			case CAUSTICS:		xx_studio_caustics();		break;	// flag
			case NO_EXP_TRANS:	xx_studio_no_exp_trans();	break;	// flag
			case PROJECTION:	xx_studio_projection();		break;	// keyword
			case ANTIALIAS:		xx_studio_antialias();		break;	// keyword
			case BKG:			xx_studio_background();		break;	// block
			default:
				// Syntax error
				yyerror("Unknown studio element.");
				break;
		}
	}

    // POST PROCESSING

	//
	// Clean up loose ends in studio structure
	//

	// Normalize UP vector
	VecNormalize(Eye.view_up);

	// If not pinhole camera and no focal length was
	// chosen, assume distance from eye to 'at' point
	if (camera.aperture>0.0 && camera.focal_length==0.0) {
		VecSub(Eye.view_from, Eye.view_at, tmp);
		camera.focal_length = VecLen(tmp);
	}

	// Do whole image
	// NOTE: If no studio was detected, stop_line at -1
	//		 forces an abort to occur!
	if (stop_line == (-1)) {
		stop_line = Yresolution;
	}

	// Haze color same as background color
	VecCopy(background.color, HazeColor);

	// If no background plane detected
	if (background.up[0]==0 && 
		background.up[1]==0 && 
		background.up[2]==0) {
		// Use viewpoint's up vector instead
		VecCopy(Eye.view_up, background.up);
	} 
	
	// Otherwise, normalize bgnd up vector
	else {
		VecNormalize(background.up);
	}

}


/*==========================================================================*/

// BACKGROUND Structure

/*
	xx_background() -- Parse a background structure.

	tokens:
	
		color, up

	NOTE:

		Also reads MAP color palette files
*/

// KEYWORD DECODERS

// static void xx_bgnd_(void) {}

// Decode COLOR keyword
static void xx_bgnd_color(void) {
	read_vec(background.color);
}

// Decode UP keyword
static void xx_bgnd_up(void) {
	read_vec(background.up);
}

// Read MAP file into background structure
static void xx_bgnd_map_file(void) {
	if (!ReadMapFile(cur_text, &background)) {
		last_call(1);
	}
}


// Decode BACKGROUND block
static void xx_studio_background(void) {

	// Look ahead
	get_token();
	
	// Background could be just a simple color vector
	if (cur_token == NUMBER) {
		push_token();
		read_vec(background.color);
		return;
	} 

	// Could also be a {} body
	if(cur_token == LEFT_BRACE) {
		while (get_token() != RIGHT_BRACE) {
			switch (cur_token) {
			case COLOR:		xx_bgnd_color();		break;	// vec
			case UP:		xx_bgnd_up();			break;	// vec
			case UNKNOWN:	xx_bgnd_map_file();		break;	// filename
			default:
				// TODO: specify which token was found...
				yyerror("Unexpected token in background structure.");
				break;
			}
		}
		return;
	} 

	// Bummer!
	yyerror("Invalid background definition.");
}


/*==========================================================================*/

// LIGHT Structure

/*
	yy_light() -- Parse a light.

	tokens:

		type, color, center, falloff, no_shadows, no_spec, direction
		radius, min_angle, max_angle, samples, at
*/

// KEYWORD DECODERS

// static void xx_light_[KEYWORD](void) {}

// Decode AT keyword
static void xx_light_at(void) {
	get_vec();
	if (TransTop) { trans_vector(TransTop->mat, tmp_vec, tmp_vec); }
	VecCopy(tmp_vec, light_head->dir);
	VecSub(light_head->position, light_head->dir, light_head->dir);
	VecNormalize(light_head->dir);
}

// Decode DIRECTION keyword
static void xx_light_direction(void) {
	get_vec();
	if (TransTop) { trans_normal(TransTop->mat, tmp_vec, tmp_vec); }
	VecCopy(tmp_vec, light_head->dir);
	VecS((-1.0), light_head->dir, light_head->dir);
	VecNormalize(light_head->dir);
}

// Decode MIN_ANGLE keyword
static void xx_light_min_angle(void) {
	get_num();
	light_head->min_angle = cos(degtorad(cur_value / 2.0));
}

// Decode MAX_ANGLE keyword
static void xx_light_max_angle(void) {
	get_num();
	light_head->max_angle = cos(degtorad(cur_value / 2.0));
}

// Decode the light TYPE keyword
// tokens: point, spherical, spot, directional
static void xx_light_type(void) {
	get_token();
	switch (cur_token) {
	case POINT:			light_head->type = L_POINT;			break;
	case SPHERICAL:		light_head->type = L_SPHERICAL;		break;
	case SPOT:			light_head->type = L_SPOT;			break;
	case DIRECTIONAL:	light_head->type = L_DIRECTIONAL;	break;
	default:
		yyerror("Unkown light type.");
		break;
	}
}

// Decode light COLOR keyword
static void xx_light_color(void) {
	read_vec(light_head->color);
}

// Decode light CENTER keyword
static void xx_light_center(void) {
	get_vec();
	if (TransTop) { trans_vector(TransTop->mat, tmp_vec, tmp_vec); }
	VecCopy(tmp_vec, light_head->position);
}

// Decode light FALLOFF keyword
static void xx_light_falloff(void) {
	light_head->illum = (short)read_int();
}

// Decode light NO_SHADOWS keyword
static void xx_light_no_shadows(void) {
	light_head->flag |= L_NOSHADOWS;
}

// Decode light NO_SPEC keyword
static void xx_light_no_spec(void) {
	light_head->flag |= L_NOSPEC;
}

// Decode light RADIUS keyword
static void xx_light_radius(void) {
	light_head->radius = read_float();
}

// Decode light SAMPLES keyword
static void xx_light_samples(void) {
	light_head->samples = read_int();
}

// Decode LIGHT structure
static void yy_light(void) {

	Light*	tmp;
	int     i;

    LogPrint("yy_light");

	//
	// Set up light defaults
	//

	// Allocate light structure
	tmp = (Light *)vmalloc(sizeof(Light));
	ptrchk(tmp, "light structure");

	// Add to head of list
	tmp->next = light_head;
	light_head = tmp;

	// Fill in default values
	light_head->type = L_POINT;
	light_head->illum = L_INFINITE;
	light_head->samples = L_SAMPLES;
	light_head->flag = 0;

	// Clear the object cache
	for (i=0; i<MAXLEVEL; i++) {
		light_head->light_obj_cache[i] = NULL;
	}

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// Decode all tokens up through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case TYPE:			xx_light_type();			break;	// keyword
			case AT:			xx_light_at();				break;	// vec
			case CENTER:		xx_light_center();			break;	// vec
			case COLOR:			xx_light_color();			break;	// vec
			case DIRECTION:		xx_light_direction();		break;	// vec
			case FALLOFF:		xx_light_falloff();			break;	// float
			case MAX_ANGLE:		xx_light_max_angle();		break;	// float
			case MIN_ANGLE:		xx_light_min_angle();		break;	// float
			case RADIUS:		xx_light_radius();			break;	// float
			case SAMPLES:		xx_light_samples();			break;	// int
			case NO_SHADOWS:	xx_light_no_shadows();		break;	// flag
			case NOSPEC:		xx_light_no_spec();			break;	// flag
			default:
				yyerror("Unkown light structure element.");
				break;
		}
	}

	// Bump light count
	nLights++;

	// Show statistics
	yystats();
}


/*==========================================================================*/

// SURFACE Structure

/*
	yy_surface() -- Parse a surface structure.

	tokens:

		diffuse, specular, transparent, ambient
		shine, ior, fuzz, no_antialias, texture, bump
*/

// KEYWORD DECODERS

// static void xx_surface_[KEYWORD](Surface* surf) {}

// Decode diffuse block
// texmap
// diffuse color
static void xx_surface_diffuse(Surface* surf) {
	get_token();
	if (cur_token == LEFT_BRACE) {
		push_token();
		surf->tm_diff = xx_texmap();
		surf->flags |= S_TM_DIFF;
	}
	else {
		push_token();
		get_vec();
		VecCopy(tmp_vec, surf->diff);
	}
}

// Decode ambient block
// texmap
// ambient color
static void xx_surface_ambient(Surface* surf) {
	get_token();    /* peek ahead */
	if (cur_token == LEFT_BRACE) {
		push_token();
		surf->tm_amb = xx_texmap();
		surf->flags |= S_TM_AMB;
	}
	else {
		push_token();
		get_vec();
		VecCopy(tmp_vec, surf->amb);
	}
}

// Decode specular block
// texmap
// specular color
static void xx_surface_spec(Surface* surf) {
	get_token();
	if (cur_token == LEFT_BRACE) {
		push_token();
		surf->tm_spec = xx_texmap();
		surf->flags |= S_TM_SPEC;
	}
	else {
		push_token();
		get_vec();
		VecCopy(tmp_vec, surf->spec);
		if (surf->shine != 0
			&& surf->cshine[0] != 0
			&& surf->cshine[1] != 0
			&& surf->cshine[2] != 0) {
			VecCopy(tmp_vec, surf->cshine);
		}
	}
}

// Decode transparent block
// texmap
// transparent color
static void xx_surface_trans(Surface* surf) {
	get_token();    /* peek ahead */
	if (cur_token == LEFT_BRACE) {
		push_token();
		surf->tm_trans = xx_texmap();
		surf->flags |= S_TM_TRANS;
	}
	else {
		push_token();
		get_vec();
		VecCopy(tmp_vec, surf->trans);
	}
	surf->flags &= ~S_CACHE;
}

// Decode shine block
// Shine coefficient and color 
// (explicit vector or copies specular color in no args)
static void xx_surface_shine(Surface* surf) {
	get_num();
	surf->shine = cur_value;
	get_token();
	push_token();
	if (cur_token == NUMBER) {
		get_vec();
		VecCopy(tmp_vec, surf->cshine);
	}
}

// Decode IOR keyword
static void xx_surface_ior(Surface* surf) {
	surf->ior = read_float();
}

// Decode FUZZ keyword
static void xx_surface_fuzz(Surface* surf) {
	surf->fuzz = read_float();
}

// Decode NO_ANTIALIAS keyword
static void xx_surface_no_antialias(Surface* surf) {
	surf->flags |= S_NO_ANTIALIAS;
}

// Decode TEXTURE keyword
static void xx_surface_texture(Surface* surf) {
	surf->tex = xx_texture();
}

// Decode BUMP keyword
static void xx_surface_bump(Surface* surf) {
	surf->bump = xx_bump();
}


// Decode SURFACE structure
static Surface* yy_surface(void) {

	Surface* surf;

    LogPrint("yy_surface");

	// Allocate surface and fill in default values
	surf = (Surface *)vmalloc (sizeof(Surface));
	ptrchk(surf, "surface");

	//
	// Assign defaults
	//
	MakeVector(0, 0, 0, surf->amb);
	MakeVector(0, 0, 0, surf->diff);
	MakeVector(0, 0, 0, surf->spec);
	MakeVector(0, 0, 0, surf->trans);
	MakeVector(-1, -1, -1, surf->cshine);		// -1 signals not explicitly set
	surf->fuzz = 0.0;
	surf->ior = 1.0;
	surf->shine = 0.0;
	surf->flags = S_CACHE;
	surf->tm_diff = NULL;
	surf->tm_spec = NULL;
	surf->tm_trans = NULL;
	surf->tm_amb = NULL;
	surf->bump = NULL;
	surf->tex = NULL;

	// If a transformation exists, set matrix to inverse
	// of current matrix at TransTop
	if (TransTop) {
		surf->flags |= S_TRANSFORM;
		matrix_inverse(TransTop->mat, surf->matrix);
	}

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// decode all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case AMBIENT:		xx_surface_ambient(surf);		break;	// vec or block
			case DIFFUSE:		xx_surface_diffuse(surf);		break;	// vec or block
			case SPECULAR:		xx_surface_spec(surf);			break;	// vec or block
			case TRANSPARENT:	xx_surface_trans(surf);			break;	// vec or block
			case FUZZ:			xx_surface_fuzz(surf);			break;	// float
			case IOR:			xx_surface_ior(surf);			break;	// float
			case SHINE:			xx_surface_shine(surf);			break;	// float (plus optional vec)
			case NO_ANTIALIAS:	xx_surface_no_antialias(surf);	break;	// flag
			case TEXTURE:		xx_surface_texture(surf);		break;	// block
			case BUMP:			xx_surface_bump(surf);			break;	// block
			default:
				yyerror("Unknown surface element.");
				break;
		}
	}

	// Fixup shine color
	if (surf->cshine[0] < 0) {
		VecCopy(surf->spec, surf->cshine);
	}

	// Save most recent surface globally, so primitives can find it
	CurrentSurface = surf;

	// return surface ptr
	return surf;
}


/*==========================================================================*/

// TEXTURE Structure

/*
	xx_texture() -- Parse procedural texture definition.

	tokens:

		surface pattern scale translate blend
		fuzz terms radius turbulence wave
*/

// Decode SURFACE keyword
// NOTE: Up to 2 surfaces allowed
static void xx_texture_surface(Texture* tex) {
	if (tex->surf[0] == NULL) {
		tex->surf[0] = yy_surface();
	}
	else if (tex->surf[1] == NULL) {
		tex->surf[1] = yy_surface();
	}
	else {
		yyerror("Sorry, only two surfaces per texture allowed.\n");
	}
}

// Decode PATTERN keyword
// Tokens: checker spherical noise
static void xx_texture_pattern(Texture* tex) {
	get_token();
	switch (cur_token) {
	case CHECKER:	tex->func = tex_checker;		break;
	case SPHERICAL:	tex->func = tex_spherical;		break;
	case NOISE:		tex->func = tex_noise;			break;
	default:
		yyerror("Unknown texture type.");
		break;
	}
}

// Decode RADIUS keyword
// NOTE: Up to 2 radii allowed
static void xx_texture_radius(Texture* tex) {
	get_num();
	if (tex->r1 < 0) {
		tex->r1 = cur_value;
	}
	else if (tex->r2 < 0) {
		tex->r2 = cur_value;
	}
	else {
		yyerror("Thanks anyway but only two radii are needed.");
	}
}

// Decode WAVE keyword
static void xx_texture_wave(Texture* tex) {
	// Get new wave structure and add to head of list
	Wave* tmp_wave = xx_wave();
	tmp_wave->next = tex->waves;
	tex->waves = tmp_wave;
}

// Decode SCALE keyword
static void xx_texture_scale(Texture* tex) {
	tex->scale[0] = read_float();
	get_token();
	if (cur_token != NUMBER) {
		push_token();
		tex->scale[2] = tex->scale[1] = tex->scale[0];
		return;
	}
    push_token();
	tex->scale[1] = read_float();
	get_token();
	if (cur_token != NUMBER) {
		yyerror("Expected 1 or 3 numbers to follow the scale keyword.");
		return;
	}
    push_token();
	tex->scale[2] = read_float();
}

// Decode TRANSLATE keyword
static void xx_texture_translate(Texture* tex) {
	read_vec(tex->trans);
}

// Decode BLEND keyword (BLUR)
static void xx_texture_blend(Texture* tex) {
	tex->blur = read_float();
}

// Decode FUZZ keyword
static void xx_texture_fuzz(Texture* tex) {
	tex->fuzz = read_float();
}

// Decode TERMS keyword
static void xx_texture_terms(Texture* tex) {
	tex->terms = read_int();
}

// Decode TURBULENCE keyword
static void xx_texture_turbulence(Texture* tex) {
	tex->turbulence = xx_turbulence();
}

// Decode TEXTURE structure
static Texture* xx_texture(void) { 

	Texture*	tex;

	// Allocate texture structure
	tex = (Texture *)vmalloc(sizeof(Texture));
	ptrchk(tex, "procedural texture structure");

	//
	// Assign defaults
	//
	MakeVector(1, 1, 1, tex->scale);
	MakeVector(0, 0, 0, tex->trans);
	tex->turbulence = NULL;
	tex->waves = NULL;
	tex->blur = 0.0;
	tex->fuzz = 0.0;
	tex->r1 = (-1.0);
	tex->r2 = (-1.0);
	tex->func = NULL;
	tex->surf[0] = NULL;
	tex->surf[1] = NULL;
	tex->terms = 0;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case PATTERN:		xx_texture_pattern(tex);	break;	// keyword
			case SCALE:			xx_texture_scale(tex);		break;	// vec or float
			case TRANSLATE:		xx_texture_translate(tex);	break;	// vec
			case BLEND:			xx_texture_blend(tex);		break;	// float
			case FUZZ:			xx_texture_fuzz(tex);		break;	// float
			case RADIUS:		xx_texture_radius(tex);		break;	// float
			case TERMS:			xx_texture_terms(tex);		break;	// int
			case SURFACE:		xx_texture_surface(tex);	break;	// block
			case TURBULENCE:	xx_texture_turbulence(tex);	break;	// block
			case WAVE: 			xx_texture_wave(tex);		break;	// block
			default:
				yyerror("Unknown texture element.");
				break;
		}
	}

	//
	// clean up loose ends
	//

	// Make sure patteern was explicitly provided
	if (tex->func == NULL) {
		yyerror("No pattern declared for texture.");
	}

	// Deal with spherical radii
	if (tex->func == tex_spherical) {
		if(tex->r2 < 0) {
			tex->r2 = tex->r1;
		}
	}

	// Return new texture pointer
	return tex;

}


/*==========================================================================*/

// BUMP Structure

/*
	xx_bump() -- Parse bump map structure definition.

	tokens:

		scale translate turbulence wave
*/

// KEYWORD DECODERS

// static void xx_bump_[KEYWORD](Bump* bump) {}

// Decode WAVE keyword
static void xx_bump_wave(Bump* bump) {
	// Get new wave structure and add to head of list
	Wave* wave = xx_wave();
	wave->next = bump->waves;
	bump->waves = wave;
}

// Decode SCALE keyword
static void xx_bump_scale(Bump* bump) {
	bump->scale[0] = read_float();
	get_token();
	if (cur_token != NUMBER) {
		push_token();
		bump->scale[2] = bump->scale[1] = bump->scale[0];
		return;
	}
	bump->scale[1] = read_float();
	get_token();
	if (cur_token != NUMBER) {
		push_token();
		yyerror("Expected 1 or 3 numbers to follow the scale keyword.");
		return;
	}
	bump->scale[2] = read_float();
}

// Decode TRANSLATE keyword
static void xx_bump_translate(Bump* bump) {
	read_vec(bump->trans);
}

// Decode TURBULENCE keyword
static void xx_bump_turbulence(Bump* bump) {
	bump->turbulence = xx_turbulence();
}

// Decode BUMP structure
static Bump* xx_bump(void) {

	Bump* bump;

	// Allocate new bump structure
	bump = (Bump *)vmalloc(sizeof(Bump));
	ptrchk(bump, "surface normal bump structure");

	// Fill in defaults
	MakeVector(1, 1, 1, bump->scale);
	MakeVector(0, 0, 0, bump->trans);
	bump->turbulence = NULL;
	bump->waves = NULL;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace }
	while(get_token() != RIGHT_BRACE) {
		switch(cur_token) {
			case SCALE:			xx_bump_scale(bump);			break;	// vec or float
			case TRANSLATE:		xx_bump_translate(bump);		break;	// vec
			case TURBULENCE:	xx_bump_turbulence(bump);		break;	// block
			case WAVE:			xx_bump_wave(bump);				break;	// block
			default:
				yyerror("Unknown bump map element.");
				break;
		}
	}

	// Return the new bump
	return bump;

}


/*==========================================================================*/

// TURBULENCE Structure

/*
xx_turbulence() -- Parse a turbulence structure.

tokens:

terms amp scale translate
*/

// Decode TURBULENCE structure
static Turbulence* xx_turbulence(void) {

	Turbulence* cur_turb;

	// Allocatenew turbulence structure
	cur_turb = (Turbulence *)vmalloc(sizeof(Turbulence));
	ptrchk(cur_turb, "turbulence structure");

	//
	// Fill in defaults
	//

	cur_turb->terms = 1;
	cur_turb->amp = 1.0;
	MakeVector(1, 1, 1, cur_turb->scale);
	MakeVector(0, 0, 0, cur_turb->trans);

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			// tokens:
			// terms amp scale translate
		case TERMS: cur_turb->terms = read_int(); break;
		case AMP: cur_turb->amp = read_float(); break;
		case SCALE: read_vec(cur_turb->scale); break;
        case OFFSET:
            // Fall through
		case TRANSLATE: read_vec(cur_turb->trans); break;
		default:
			yyerror("Unknown turbulence structure element.");
			break;
		}
	}

	// Return turbulence structure
	return cur_turb;

}


/*==========================================================================*/

// WAVE Structure

/*
	xx_wave() -- Parse a wave structure.

	tokens:
	
		center wavelength amplitude damping phase
*/

// KEYWORD DECODERS

// static void xx_[BLOCK]_[KEYWORD](void) {}

// Decode CENTER keyword
static void xx_wave_center(Wave* wave) {
	read_vec(wave->center);
}

// Decode AMPLITUDE keyword
static void xx_wave_amplitude(Wave* wave) {
	wave->amp = read_float();
}

// Decode DAMPING keyword
static void xx_wave_damping(Wave* wave) {
	wave->damp = read_float();
}

// Decode PHASE keyword
static void xx_wave_phase(Wave* wave) {
	wave->phase = read_float();
}

// Decode WAVELENGTH keyword
static void xx_wave_wavelength(Wave* wave) {
	wave->wavelength = read_float();
}

// Decode WAVE structure
static Wave* xx_wave(void) {

	Wave* wave;

	// Allocate new wave structure
	wave = (Wave *)vmalloc(sizeof(Wave));
	ptrchk(wave, "wave structure");

	//
	// Fill in defaults
	//

	MakeVector(0, 0, 0, wave->center);
	wave->wavelength = 1.0;
	wave->amp = 0.1;
	wave->damp = 1.0;
	wave->phase = 0.0;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
		case CENTER:	xx_wave_center(wave);			break; // vec
		case AMP:		xx_wave_amplitude(wave);		break; // float
		case DAMPING:	xx_wave_damping(wave);			break; // float
		case PHASE:		xx_wave_phase(wave);			break; // float
		case LENGTH:	xx_wave_wavelength(wave);		break; // float
		default:
			yyerror("Unknown wave structure element.");
			break;
		}
	}

	// Return wave structure
	return wave;

}


/*==========================================================================*/

// AMBIENT, DIFFUSE, SPECULAR, TRANSPARENT Structures

/*
	xx_texmap() -- Parse a texure map structure.

		This function is called to decode each of the four kinds of
		texture map blocks: AMBIENT, DIFFUSE, SPECULAR and TRANSPARENT.
		These are called in the context of a SURFACE block.

	tokens:

		center normal across scale image
*/

// KEYWORD DECODERS

// static void xx_texmap_[KEYWORD](void) {}

// Decode ACROSS keyword
static void xx_texmap_across(Texmap* texmap) {
	read_vec(texmap->across);
	if (TransTop) { 
		trans_normal(TransTop->mat, texmap->across, texmap->across); 
	}
}

// Decode CENTER keyword
static void xx_texmap_center(Texmap* texmap) {
	read_vec(texmap->position);
	if (TransTop) { 
		trans_vector(TransTop->mat, texmap->position, texmap->position); 
	}
}

// Decode NORMAL keyword
static void xx_texmap_normal(Texmap* texmap) {
	read_vec(texmap->normal);
	if (TransTop) { 
		trans_normal(TransTop->mat, texmap->normal, texmap->normal); 
	}
}

// Decode SCALE keyword
static void xx_texmap_scale(Texmap* texmap) {
	texmap->scale *= read_float();
}

// Decode IMAGE keyword
static void xx_texmap_image(Texmap* texmap) {
	get_token();
	// NOTE: This should be in GENERAL-FILE.C
	tex_read_img(cur_text, texmap);
}

// Decode AMBIENT, DIFFUSE, SPECULAR or TRANSPARENT structure 
// These are all texture maps
static Texmap* xx_texmap(void) {

	Texmap* texmap;

	// Allocate new texture map structure
	texmap = (Texmap *)vmalloc(sizeof(Texmap));
	ptrchk(texmap, "texture map structure");
	
	//
	// Fill in default values
	//

	MakeVector(0.0,  0.0, 0.0, texmap->position);
	MakeVector(0.0,  0.0, 1.0, texmap->normal);
	MakeVector(1.0,  0.0, 0.0, texmap->across);
	MakeVector(0.0, -1.0, 0.0, texmap->down);
	
	if (TransTop) {
		texmap->scale = 1.0/(TransTop->mat[3][3]);
	} 
	else {
		texmap->scale = 1.0;
	}

	texmap->red = NULL;
	texmap->grn = NULL;
	texmap->blu = NULL;

	texmap->xres = 0;
	texmap->yres = 0;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case ACROSS:	xx_texmap_across(texmap);		break;	// vec
			case CENTER:	xx_texmap_center(texmap);		break;	// vec
			case NORMAL:	xx_texmap_normal(texmap);		break;	// vec
			case SCALE:		xx_texmap_scale(texmap);		break;	// float
			case IMAGE:		xx_texmap_image(texmap);		break;	// filename
			default:
				yyerror("Unkown texture map structure element.");
				break;
		}
	}

	//
	// Make sure everything is normalized and perpendicular
	//

	VecNormalize(texmap->normal);
	VecNormalize(texmap->across);
	VecCross(texmap->across, texmap->normal, texmap->down);
	VecCross(texmap->normal, texmap->down,   texmap->across);

	// Return texture map
	return texmap;

}


/*==========================================================================*/

// TRANSFORM Structure

/*
	yy_transform() -- Parse a transformation.

	tokens:

		rotate translate scale
*/

// KEYWORD DECODERS

// static void xx_transform__[KEYWORD](void) {}

// Decode ROTATE keyword
static void xx_transform_rotate(void) {
	Matrix	m;
	Flt		theta;
	get_vec();
	// do x axis rotation
	identity(m);
	theta = degtorad(tmp_vec[0]);
	m[1][1] = cos(theta);
	m[2][1] = -sin(theta);
	m[1][2] = sin(theta);
	m[2][2] = cos(theta);
	matrix_cat(TransTop->mat, m, TransTop->mat);
	// do y axis rotation
	identity(m);
	theta = degtorad(tmp_vec[1]);
	m[0][0] = cos(theta);
	m[2][0] = sin(theta);
	m[0][2] = -sin(theta);
	m[2][2] = cos(theta);
	matrix_cat(TransTop->mat, m, TransTop->mat);
	// do z axis rotation
	identity(m);
	theta = degtorad(tmp_vec[2]);
	m[0][0] = cos(theta);
	m[1][0] = -sin(theta);
	m[0][1] = sin(theta);
	m[1][1] = cos(theta);
	matrix_cat(TransTop->mat, m, TransTop->mat);
}

// Decode TRANSLATE keyword
static void xx_transform_translate(void) {
	Matrix          m;
	get_vec();
	identity(m);
	m[3][0] = tmp_vec[0];
	m[3][1] = tmp_vec[1];
	m[3][2] = tmp_vec[2];
	matrix_cat(TransTop->mat, m, TransTop->mat);
}

// Decode SCALE keyword
static void xx_transform_scale(void) {
	Matrix m;
	Flt tmp;
	get_num();
	tmp = cur_value;
	get_token();
	if (cur_token == NUMBER) {
		identity(m);
		m[0][0] = tmp;
		m[1][1] = cur_value;
		get_num();
		m[2][2] = cur_value;
		matrix_cat(TransTop->mat, m, TransTop->mat);
	}
	else {
		push_token();
		TransTop->mat[3][3] /= tmp;
	}
}


// Decode TRANSFORM structure
static void yy_transform(void) {

	Transform* cur_trans;

    LogPrint("yy_transform");

	// Allocate new transform structure
	cur_trans = (Transform *)vmalloc(sizeof(Transform));
	ptrchk(cur_trans, "transform structure");
	
	// Push onto transform stack
	cur_trans->next = TransTop;
	TransTop = cur_trans;

	// Set identity matrix
	identity(cur_trans->mat);

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

#pragma endregion

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case ROTATE:		xx_transform_rotate();		break;	// vec	
			case SCALE:			xx_transform_scale();		break;	// vec or float
			case OFFSET:		// Fall through...
			case TRANSLATE:		xx_transform_translate();	break;	// vec
			default:
				yyerror("Unknown transformation element.");
				break;
		}
	}

	// Concatenate new transformation onto previous ones
	if (TransTop->next) {
		matrix_cat(TransTop->mat, TransTop->next->mat, TransTop->mat);
	}

}


/*==========================================================================*/

// TRANS_POP Keyword

/*
	yy_transform_pop() -- Pop the current transformation.
*/

static void yy_transform_pop(void) {

    LogPrint("yy_transform_pop");

	trans_pop();

}


/*==========================================================================*/

// GLOBAL_CLIP Structure

/*
	yy_global_clip() -- Parse a global clip.

	tokens:

		clip
*/

// KEYWORD DECODERS

// static void xx_global_clip_[KEYWORD](GlobalClip* ptr) {}

// Decode GLOBAL_CLIP structure
static void yy_global_clip(void) {

	GlobalClip* ptr;

    LogPrint("yy_global_clip");

	// Allocate new global clip structure
	ptr = (GlobalClip *)vmalloc(sizeof(GlobalClip));
	ptrchk(ptr, "global clipping linked list structure");
	
	// Fill and defaults and push to stack top
	ptr->next       = GlobalClipTop;
	ptr->clip       = NULL;
	GlobalClipTop   = ptr;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		if (cur_token == CLIP) {
			xx_clip();
		} 
		else {
			yyerror("Clipping structure expected.");
		}
	}

	// Point to top of clip stack
	ptr->clip = ClipTop;

}


/*==========================================================================*/

// CLIP_POP Keyword

/*
	yy_clip_pop() -- Pop the current clip.
*/

static void yy_clip_pop(void) {

    LogPrint("yy_clip_pop");

	// Pop global clip from global clips list
	GlobalClipTop = GlobalClipTop->next;

	// Pop clip from clip list
	ClipTop = GlobalClipTop->clip;
}


/*==========================================================================*/

// CLIP Structure

/*
	xx_clip() -- Parse a clipping structure.

	tokens:

		normal center apex base
		radius apex_radius base_radius
		inside outside
*/

// TYPE MODIFIERS

// static void cc_clip_[KEYWORD](Clip* clip) {}

// Make clip into a plane
static void cc_clip_plane(Clip* clip) {
	clip->type |= C_PLANE;
	clip->type &= (~(C_SPHERE | C_CONE));
}

// Make clip into a cone
static void cc_clip_cone(Clip* clip) {
	// Make clip into a cone
	clip->type |= C_CONE;
	clip->type &= (~(C_SPHERE | C_PLANE));
}

// Make clip into a sphere
static void cc_clip_sphere(Clip* clip) {
	// Make clip into a sphere
	clip->type |= C_SPHERE;
	clip->type &= (~(C_PLANE | C_CONE));
}

// KEYWORD DECODERS

// static void xx_clip_[KEYWORD](Clip* clip) {}

// Decode CENTER keyword
static void xx_clip_center(Clip* clip) {
	// Init clip center point
	read_vec(clip->center);
	if (TransTop) { trans_vector(TransTop->mat, clip->center, clip->center); }
	if (!(clip->type & C_PLANE)) { clip->type |= C_SPHERE; }
}

// Decode NORMAL keyword
static void xx_clip_normal(Clip* clip) {
	// Init clip normal vector
	read_vec(clip->normal);
	if (TransTop) { trans_normal(TransTop->mat, clip->normal, clip->normal); }
	cc_clip_plane(clip);
}

// Decode APEX keyword
static void xx_clip_apex(Clip* clip) {
	// Init clip apex point
	read_vec(clip->apex);
	if (TransTop) { trans_vector(TransTop->mat, clip->apex, clip->apex); }
	cc_clip_cone(clip);
}

// Decode BASE keyword
static void xx_clip_base(Clip* clip) {
	// Init clip base point
	read_vec(clip->base);
	if (TransTop) { trans_vector(TransTop->mat, clip->base, clip->base); }
	cc_clip_cone(clip);
}

// Decode RADIUS keyword
static void xx_clip_radius(Clip* clip) {
	// Init clip radius
	clip->radius1 = clip->radius2 = read_float();
}

// Decode APEX_RADIUS keyword
static void xx_clip_apex_radius(Clip* clip) {
	// Init clip apex radius (for cones)
	clip->radius1 = read_float();
	cc_clip_cone(clip);
}

// Decode BASE_RADIUS keyword
static void xx_clip_base_radius(Clip* clip) {
	// Init clip base radius (for cones)
	clip->radius2 = read_float();
	cc_clip_cone(clip);
}

// Decode INSIDE keyword
static void xx_clip_inside(Clip* clip) {
	// Set clip inside flag
	clip->type |= C_INSIDE;
}

// Decode OUTSIDE keyword
static void xx_clip_outside(Clip* clip) {
	// Clear clip inside flag (make outside instead)
	clip->type &= (~C_INSIDE);
}

// Decode CLIP structure
static Clip* xx_clip(void) {

	Clip* clip;

	// Allocate new clip structure
	clip = (Clip *)vmalloc(sizeof(Clip));
	ptrchk(clip, "clipping structure");

	//
	// Fill in defaults
	//
	MakeVector(0, 0, 0, clip->center);
	MakeVector(0, 0, 1, clip->normal);
	MakeVector(0, 0, 0, clip->apex);
	MakeVector(0, 0, 0, clip->base);
	clip->radius1 = 0.0;
	clip->radius2 = 0.0;
	clip->length  = 0.0;
	clip->type    = 0;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

#pragma endregion

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case APEX:			xx_clip_apex(clip);			break;	// vec
			case BASE:			xx_clip_base(clip);			break;	// vec
			case CENTER:		xx_clip_center(clip);		break;	// vec
			case NORMAL:		xx_clip_normal(clip);		break;	// vec
			case APEX_RADIUS:	xx_clip_apex_radius(clip);	break;	// float
			case BASE_RADIUS:	xx_clip_base_radius(clip);	break;	// float
			case RADIUS:		xx_clip_radius(clip);		break;	// float
			case INSIDE:		xx_clip_inside(clip);		break;	// flag
			case OUTSIDE:		xx_clip_outside(clip);		break;	// flag
			default:
				yyerror("Unknown clipping structure element.");
				break;
		}
	}

	// Fixup cone details
	if (clip->type & C_CONE) {
		VecSub(clip->base, clip->apex, clip->normal);
		clip->length = VecNormalize(clip->normal);
	}

	// Fixup spherical details
	if (clip->type & C_SPHERE) {
		// For spheres, radius1 is squared radius
		clip->radius1 = clip->radius1 * clip->radius1;
	}

	// Insert as new head of current clip list
	clip->next = ClipTop;
	ClipTop = clip;

	// Return clip
	return clip;
}


/*==========================================================================*/

// CONE Structure

/*
	yy_cone() -- Parse a cone.

	tokens:
	
		apex base apex_radius base_radius radius clip
*/

// Decode CONE structure
static void yy_cone(void) {

	Vec     apex, base;
	Flt     arad, brad;

    LogPrint("yy_cone");

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case APEX:			read_vec(apex);					break;	// vec
			case BASE:			read_vec(base);					break;	// vec
			case APEX_RADIUS:	arad = read_float();			break;	// float
			case BASE_RADIUS:	brad = read_float();			break;	// float
			case RADIUS:		arad = brad = read_float();		break;	// float
			case CLIP:			xx_clip();						break;	// block
			default:
				yyerror("Unknown cone element.");
				break;
		}
	}

	// Deal with transforms
	if (TransTop) {
		trans_vector(TransTop->mat, apex, apex);
		trans_vector(TransTop->mat, base, base);
		arad /= TransTop->mat[3][3];
		brad /= TransTop->mat[3][3];
	}

	// Create and init new cone structure
	register_shape(MakeCone(apex, arad, base, brad));
}


/*==========================================================================*/

// PATCH Structure

/*
	yy_patch() -- Parse a triangular patch.

	tokens:

		vertex normal clip
*/

// Decode PATCH structure
static void yy_patch(void) {

	Vec     data[6];
	int     v, n;

    LogPrint("yy_patch");

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// Clear indices
	v = n = 0;

	// Decode all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
			case VERTEX: 
				if (v >= 3) {
					yyerror("Too many patch vertices defined.");
				}
				read_vec(data[v*2]); 
				++v; 
				break;
			case NORMAL: 
				if (n >= 3) {
					yyerror("Too many patch normals defined.");
				}
				read_vec(data[n*2+1]);
				++n; 
				break;
			case CLIP: 
				xx_clip(); 
				break;
			default:
				yyerror("Unknown triangular patch element.");
				break;
		}
	}

// POST PROCESSING

	// Make sure we got enough vertices
	if (v < 3) {
		yyerror("Not enough patch vertices defined.");
	}

	// Make sure we got enough normals
	if (n < 3) {
		yyerror("Not enough patch normals defined.");
	}

	// Deal with transformations
	if (TransTop) {
		// Transform vertices
		trans_vector(TransTop->mat, data[0], data[0]);
		trans_vector(TransTop->mat, data[2], data[2]);
		trans_vector(TransTop->mat, data[4], data[4]);
		// Transform normals
		trans_normal(TransTop->mat, data[1], data[1]);
		trans_normal(TransTop->mat, data[3], data[3]);
		trans_normal(TransTop->mat, data[5], data[5]);
	}

	// Create and init new patch structure
	register_shape(MakeTri(data));
}


/*==========================================================================*/

// POLYGON Structure

/*
	yy_polygon() -- Parse a polygon.

	tokens:

		points vertex clip
*/

// Decode POLYGON structure
static void yy_polygon(void) {

	int     num_pnts, i;
	Vec*	vlist;

    LogPrint("yy_polygon");

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	//
	// Get number of points
	//
	get_token();
	if (cur_token != POINTS) {
		yyerror("Number of points in polygon needed.");
	}
	get_num();
	num_pnts = (int)cur_value;

	// Allocate new vertex array
	vlist = (Vec *)vmalloc(num_pnts * sizeof(Vec));
	ptrchk(vlist, "polygon vertex list");

	// Set vertex index to 0
	i = 0;

	// Decode all up through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
		case VERTEX:
			if (i >= num_pnts) {
				yyerror("Too many polygon vertices were defined.");
			}
			read_vec(vlist[i]);
			if (TransTop) { trans_vector(TransTop->mat, vlist[i], vlist[i]); }
			i++;
			break;
		case CLIP:
			xx_clip();
			break;
		default:
			yyerror("Unknown polygon element.  Vertex expected.");
			break;
		}
	}

	// Make sure we got right # of points
	if (i < num_pnts) {
		yyerror("Too few polygon vertices were defined.");
	}

	// Create and init new polygon structure
	register_shape(MakePoly(num_pnts, vlist));

}


/*==========================================================================*/

// RING Structure

/*
	yy_ring() -- Parse a ring.

	tokens:

		center normal radius max_radius min_radius clip
*/

// Decode RING structure
static void yy_ring(void) {

	Vec     center, normal;
	Flt     min_rad, max_rad;

    LogPrint("yy_ring");

	min_rad = 0.0;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace }
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
		case CENTER:		read_vec(center);			break;	// vec
		case NORMAL:		read_vec(normal);			break;	// vec
		case RADIUS:		// fall through...
		case MAX_RADIUS:	max_rad = read_float();		break;	// float
		case MIN_RADIUS:	min_rad = read_float();		break;	// float
		case CLIP:			xx_clip();					break;	// block
		default:
			yyerror("Unknown ring element.");
			break;
		}
	}

	// Deal with transformations
	if (TransTop) {
		trans_vector(TransTop->mat, center, center);
		trans_normal(TransTop->mat, normal, normal);
		min_rad /= TransTop->mat[3][3];
		max_rad /= TransTop->mat[3][3];
	}

	// Create and init new ring structure
    Object* tmp = MakeRing(center, normal, min_rad, max_rad);
	LogPrint("Register ring");
    register_shape(tmp);
}


/*==========================================================================*/

// SPHERE Structure

/*
	yy_sphere() -- Parse a sphere

	tokens:

		center radius fuzz clip
*/

// Decode SPHERE structure
static void yy_sphere(void) {

	Vec center;
	Flt radius;
	Flt fuzz;

    LogPrint("yy_sphere");

	// Set fuzz to zero
	fuzz = 0.0;

	// Eat left brace {
	if (get_token() != LEFT_BRACE) {
		yyerror("Left brace expected.");
	}

	// And all through right brace
	while (get_token() != RIGHT_BRACE) {
		switch (cur_token) {
		case CENTER:	read_vec(center);		break;	// vec
		case RADIUS:	radius = read_float();	break;	// float
		case FUZZ:		fuzz = read_float();	break;	// float
		case CLIP:		xx_clip();				break;	// block
		default:
			yyerror("Unknown sphere element.");
			break;
		}
	}

	// Deal with transformations
	if (TransTop) {
		// Reposition in world
		trans_vector(TransTop->mat, center, center);
		radius /= TransTop->mat[3][3];	// Distance scaling
		fuzz /= TransTop->mat[3][3];	// Distance scaling
	}

	// Register new primitive shape
	register_shape(MakeSphere(center, radius, fuzz));
}

