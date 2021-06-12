/*
	general-data.c

	Global variables

*/

#include <stdio.h>
#include <string.h>
#include "inc/bob.h"
#include "inc/tokens.h"


// Ray
Flt				rayeps = TINY;					// some tiny number
Flt				minweight = MINWEIGHT;			// minimum weight

// Screen
int 			Xresolution = 320;				// screen width
int 			Yresolution = 200;				// screen height

// Session
int				start_line = 0;					// session start line
int				stop_line = 0;				    // session stop line
BOOL		    resume = 0;						// resume prior session
BOOL            console = 0;                    // enter console mode on error
BOOL			tickflag = TRUE;				// enable statistics reporting

// Studio
Viewpoint		Eye = { 0 };					// camera orientation
Camera			camera = { 0 };					// camera configuration

int				bunching = BUNCHINGFACTOR;		// composite bunch size
int				antialias = 0;					// default to none
int				jitter = 0;						// none
int				adapt_dist = 8;					// threshold for adaptive sampling
int				maxlevel = MAXLEVEL;			// maximum recursion level allowed

// Lights
Light*			light_head = NULL;				// ptr to head of linker list if lights
int				nLights = 0;					// it's a dark world out there
BOOL			no_shadows = 0;					// shadows by default
BOOL			caustics = 0;					// no fake caustics
BOOL			exp_trans = TRUE;				// enable exponential transparency
BOOL			fuzzy_ray = 0;                  // Ray fuzziness

// Scene
Background		background = { 0 };				// background configuration
Color			Ambient = { 0, 0, 0 };			// global ambient light
Color			HazeColor = { 0, 0, 0 };		// color of haze
Flt				HazeDensity = 0;				// exponential factor

// Surfaces
Surface*		CurrentSurface = NULL;			// surface at current top of stack
Stack*			SurfTop = NULL;					// points to top of surface stack

// Transforms
Transform*		TransTop = NULL;				// top of transformation stack

// Clipping
Clip*			ClipTop = NULL;					// current clipping list
GlobalClip*		GlobalClipTop = NULL;			// current global clip list

// Primitive objects (shapes)
Object*			Root = NULL;					// ptr to head of linked list of primitive shapes
int	    		nPrims = 0;						// total primitive shapes

// Statistics
int				deepest = 0;					// deepest recursion level reached
DWORD			nRays = 0;						// total number of rays traced
DWORD			nShadows = 0;					// total number of shadow feeler rays
DWORD			nReflected = 0;					// total number of reflected rays
DWORD			nRefracted = 0;					// total number of refracted rays
DWORD			maxQueueSize = 0;				// largest queue size
DWORD			totalQueues = 0;				// total number of queues
DWORD			totalQueueResets = 0;			// total number of queue resets
DWORD			nChecked = 0;					// total number of primitives checked for enqueue
DWORD			nEnqueued = 0;					// total number of primitives enqueued
DWORD			nShadowCacheHits = 0;			// total number of shadow cache hits
DWORD			MemAllocated = 0;				// total memory allocated by vmalloc
char*			Progname = NULL;				// ptr to executable filename

// Interpeter
char			Infilename[MAX_PATH] = "";		// input filename buffer
Stack*			InfileTop = NULL;				// points to top of input file stack

// Bounding slabs
Vec	Slab[] = {									// bounding slabs
	{1.0, 0.0, 0.0},							// x-axis
	{0.0, 1.0, 0.0},							// y-axis
	{0.0, 0.0, 1.0},							// z-axis
	{1.0, 1.0, 0.0},							// x-y axes (unused?)
	{1.0, 0.0, 1.0},							// x-z axes (unused?)
	{0.0, 1.0, 1.0}								// y-z axes (unused?)
};


/*--------------------------------------------------------------------------*/

// Local Data
static int g_num_prims = 0;             // Number of primitives
static int g_num_lights = 0;            // Number of lights
static int g_num_comps = 0;             // Number of composites
static int g_num_clips = 0;             // Number of clips
static int g_num_surfs = 0;             // Number of surfaces
static int g_num_texs = 0;              // Number of textures


/*--------------------------------------------------------------------------*/

// Local routines

static void print_object(POBJECT pObj);
static void print_composite(PCOMPOSITEDATA pComp);
static void print_waves(PWAVE pWave);
static void print_bump(PBUMP pBump);
static void print_turb(PTURBULENCE pTurb);
static void print_surf(PSURFACE pSurface);
static void print_tex(PTEXTURE pTexture);
static void print_clip(PCLIP pClip);


/*--------------------------------------------------------------------------*/

// Global routines

// Print background variables
void print_bgnd(void) {
    PrintVector("background.color", background.color);
    PrintVector("background.up", background.up);
    for (int n=0; n<256; n++) {
        Vec v;
        char buffer[MAX_TOKEN];
        v[0] = background.pal[n][0];
        v[1] = background.pal[n][1];
        v[2] = background.pal[n][2];
        sprintf(buffer, "background.pal[%u]", n);
        PrintVector(buffer, v);
    }
}


// Print ray variables
void print_ray_vars(void) {
    Print("\nRay Variables");
    printf("rayeps = %f\n", rayeps);
    printf("minweight = %f\n", minweight);
}


// Print screen variables
void print_screen_vars(void) {
    // Xresolution, Yresolution
    Print("\nScreen Variables");
    PrintInteger("Xresoultion", Xresolution);
    PrintInteger("Yresolution", Yresolution);
}


// Print session variables
void print_session_vars(void) {
    Print("\nSession Variables");
    PrintInteger("start_line", start_line);
    PrintInteger("stop_line", stop_line);
    PrintFlag("resume", resume);
}


// Print studio variables
void print_studio_vars(void) {
    static LPCSTR aa_modes[] = {
        "none", "corners", "quick", "adaptive"
    };
    static LPCSTR proj_modes[] = {
        "flat", "orthographic", "fisheye", "no parallax"
    };
    LPCSTR err_mode = "error";
    LPCSTR mode;
    Print("\nStudio Variables");
    switch (antialias) {
        case A_NONE:        mode = aa_modes[0];    break;
        case A_CORNERS:     mode = aa_modes[1];    break;
        case A_QUICK:       mode = aa_modes[2];    break;
        case A_ADAPTIVE:    mode = aa_modes[3];    break;
        default:            mode = err_mode;       break;
    }
    PrintText("antialias", mode);
    PrintInteger("bunching", bunching);
    PrintInteger("jitter", jitter);
    PrintInteger("adapt_dist", adapt_dist);
    PrintInteger("maxlevel", maxlevel);
    PrintVector("Eye.view_from", Eye.view_from);
    PrintVector("Eye.view_at", Eye.view_at);
    PrintVector("Eye.view_up", Eye.view_up);
    PrintFloat("Eye.view_angle_x", Eye.view_angle_x);
    PrintFloat("Eye.view_angle_y", Eye.view_angle_y);
    PrintFloat("Eye.view_aspect", Eye.view_aspect);
    switch (camera.projection) {
        case P_FLAT:            mode = proj_modes[0];   break;
        case P_ORTHOGRAPHIC:    mode = proj_modes[1];   break;
        case P_FISHEYE:         mode = proj_modes[2];   break;
        case P_NO_PARALLAX:     mode = proj_modes[3];   break;
        default: ;              mode = err_mode;        break;
    }
    PrintText("camera.projection", mode);
    PrintVector("camera.lens_i", camera.lens_i);
    PrintVector("camera.lens_j", camera.lens_j);
    PrintFloat("camera.aperture", camera.aperture);
    PrintFloat("camera.focal_length", camera.focal_length);
    PrintInteger("camera.samples", (int)camera.samples);
}


// Print light variables
void print_light_vars(void) {
    // nLights, no_shadows, caustics, exp_trans, fuzzy_ray, light_head
    Print("\nLight Variables");
    PrintFlag("no_shadows", no_shadows);
    PrintFlag("caustics", caustics);
    PrintFlag("exp_trans", exp_trans);
    PrintFlag("fuzzy_ray", fuzzy_ray);
    PrintInteger("nLights", nLights);
    PLIGHT pLight = light_head;
    int nLight=0;
    if (pLight) {
        while (pLight) {
            char buffer[MAX_TOKEN];
            PrintInteger("Light #", ++nLight);
            PrintVector("pLight->position", pLight->position);
            PrintVector("pLight->dir", pLight->dir);
            PrintVector("pLight->color", pLight->color);
            PrintFloat("pLight->radius", pLight->radius);
            PrintFloat("pLight->min_angle", pLight->min_angle);
            PrintFloat("pLight->max_angle", pLight->max_angle);
            PrintInteger("pLight->samples", (int)(pLight->samples));
            switch (pLight->type) {
            case L_DIRECTIONAL:     strcpy(buffer, "directional");  break;
            case L_POINT:           strcpy(buffer, "point");        break;
            case L_SPHERICAL:       strcpy(buffer, "spherical");    break;
            case L_SPOT:            strcpy(buffer, "spot");         break;
            default:                strcpy(buffer, "error");        break;
            }
            PrintText("pLight->type", buffer);
            switch (pLight->illum) {
            case L_INFINITE:        strcpy(buffer, "infinite");     break;
            case L_R:               strcpy(buffer, "r");            break;
            case L_R_SQUARED:       strcpy(buffer, "r squared");    break;
            default:                strcpy(buffer, "error");        break;
            }
            PrintText("pLight->illum", buffer);
            buffer[0] = 0;
            if (pLight->flag & L_NOSHADOWS) {
                strcat(buffer, "no-shadows ");
            }
            if (pLight->flag & L_NOSPEC) {
                strcat(buffer, "no-spec ");
            }
            PrintText("pLight->flags", buffer);
            pLight = pLight->next;
        }
    }
    else {
        PrintText("light_head", "NULL");
    }
}


// Print scene variables
void print_scene_vars(void) {
    Print("\nScene Variables");
    PrintVector("Ambient", Ambient);
    PrintVector("HazeColor", HazeColor);
    printf("HazeDensity = %f\n", HazeDensity);
    PrintInteger("nPrims", nPrims);
    POBJECT pObject = Root;
    g_num_prims = 0;
    g_num_comps = 0;
    if (pObject) {
        while (pObject) {
            print_object(pObject);
            pObject = pObject->next;
        }
    } else {
        PrintText("Root", "NULL");
    }
}


// Print surface variables
void print_surface_vars(void) {
    char value[MAX_TOKEN];
    char buffer[MAX_TOKEN];
    // CurrentSurface, SurfTop
    Print("\nSurface Variables");
    g_num_surfs = 0;
    g_num_texs = 0;
    PrintAddress("CurrentSurface", CurrentSurface);
    PSTACK pStack = SurfTop;
    if (pStack) {
        while (pStack) {
            PSURFACE pSurf = (PSURFACE)(pStack->what);
            print_surf(pSurf);
            pStack = pStack->prev;
        }
    }
    else {
        PrintText("SurfTop", "NULL");
    }
}


// Print transform variables
void print_transform_vars(void) {
    Print("\nTransform Variables");
    PTRANSFORM pTrans = TransTop;
    int nTrans=0;
    if (pTrans) {
        while (pTrans) {
            PrintInteger("Transform #", ++nTrans);
            PrintMatrix("pTrans->mat", pTrans->mat);
            pTrans = pTrans->next;
        }
    }
    else {
        PrintText("TransTop", "NULL");
    }
}


// Print clip variables
void print_clip_vars(void) {
    Print("\nClip Variables");
    PrintAddress("ClipTop", ClipTop);
    g_num_clips = 0;
    PGLOBALCLIP pNext = GlobalClipTop;
    if (pNext) {
        while (pNext) {
            PCLIP pClip = pNext->clip; 
            print_clip(pClip);
            pNext = pNext->next;
        }
    }
    else {
        PrintText("GlobalClipTop", "NULL");
    }
}


// Print slab variables
void print_slab_vars(void) {
    Print("\nSlab Variables");
    for (int n=0; n<NSLABS; n++) {
        char buffer[MAX_TOKEN];
        sprintf(buffer, "Slab[%i]", n);
        PrintVector(buffer, Slab[n]);
    }
}


// Print miscellaneous variables
void print_misc_vars(void) {
    Print("\nMiscellaneous Variables");
    PrintFlag("console", console);
    PrintFlag("tickflag", tickflag);
    PrintText("Infilename", Infilename);
    PrintText("Progname", Progname);
}


/*--------------------------------------------------------------------------*/

// Helpers

// Print an object
static void print_object(POBJECT pObject) {
    static LPCSTR types[] = {
        "composite", "sphere", "poly", "cone", "ring", "tri", "fuzzy"
    };
    if (pObject) {
        char value[MAX_TOKEN];
        char buffer[MAX_TOKEN];
        PrintInteger("Primitive #", ++g_num_prims);
        LPCSTR type;
        switch (pObject->o_type) {
        case T_COMPOSITE:   type = types[0];    break;
        case T_SPHERE:      type = types[1];    break;
        case T_POLY:        type = types[2];    break;
        case T_CONE:        type = types[3];    break;
        case T_RING:        type = types[4];    break;
        case T_TRI:         type = types[5];    break;
        case T_FUZZY:       type = types[6];    break;
        default:            type = "error";     break;
        }
        PrintText("pObject->type", type);
        PrintAddress("pObject->o_procs", pObject->o_procs);
        PrintAddress("pObject->o_data", pObject->o_data);

        // Print surface structure
        Print("pObject->o_surf");
        print_surf(pObject->o_surf);

        // Delve into clips list
        g_num_clips = 0;
        PrintAddress("pObject->clips", pObject->clips);
        PCLIP pClip = pObject->clips;
        while (pClip) {
            print_clip(pClip);
            pClip = pClip->next;
        }

        strcpy(buffer, "pObject->o_dmax = ");
        for (int i=0; i<NSLABS; i++) {
            sprintf(value, "%f ", pObject->o_dmax[i]);
            strcat(buffer, value);
        }
        Print(buffer);
        strcpy(buffer, "pObject->o_dmin = ");
        for (int i=0; i<NSLABS; i++) {
            sprintf(value, "%f ", pObject->o_dmin[i]);
            strcat(buffer, value);
        }
        Print(buffer);

        // Delve into composite list
        if (pObject->o_type == T_COMPOSITE) {
            PCOMPOSITEDATA data = (PCOMPOSITEDATA)(pObject->o_data);
            print_composite(data);
        }
    }
    else {
        PrintText("pObject", "NULL");
    }
}


// Print a composite object
static void print_composite(PCOMPOSITEDATA pComp) {
    if (pComp) {
        PrintInteger("pComp->size", pComp->size);
        int nChild = 0;
        POBJECT pObject = pComp->children;
        while (pObject) {
            PrintInteger("Child Object #", ++nChild);
            print_object(pObject);
            pObject = pObject->next;
        }
    }
    else {
        PrintText("pComp", "NULL");
    }
}


// Print a list of wave structures
static void print_waves(PWAVE pWave) {
    if (pWave) {
        int nWaves = 0;
        while (pWave) {
            PrintInteger("Wave #", ++nWaves);
            PrintVector("pWave->center", pWave->center);
            PrintFloat("pWave->wavelength", pWave->wavelength);
            PrintFloat("pWave->amp", pWave->amp);
            PrintFloat("pWave->damp", pWave->damp);
            PrintFloat("pWave->phase", pWave->phase);
            pWave = pWave->next;
        }
    }
    else {
        PrintText("pWave", "NULL");
    }
}


// Print a turbuulence structure
static void print_turb(PTURBULENCE pTurb) {
    if (pTurb) {
        PrintInteger("pTurb->terms", pTurb->terms);
        PrintFloat("pTurb->amp", pTurb->amp);
        PrintVector("pTurb->trans", pTurb->trans);
        PrintVector("pTurb->scale", pTurb->scale);
    }
    else {
        PrintText("pTurb", "NULL");
    }
}


// Print a bump structure
static void print_bump(PBUMP pBump) {
    if (pBump) {
        PrintVector("pBump->trans", pBump->trans);
        PrintVector("pBump->scale", pBump->scale);
        print_turb(pBump->turbulence);
        print_waves(pBump->waves);
    }
    else {
        PrintText("pBump", "NULL");
    }
}


// Print a surface structure
static void print_surf(PSURFACE pSurf) {
    PrintInteger("Surface #", ++g_num_surfs);
    if (pSurf) {
        char buffer[MAX_TOKEN];

        // Vectors
        PrintVector("pSurf->cshine", pSurf->cshine);
        PrintVector("pSurf->amb", pSurf->amb);
        PrintVector("pSurf->diff", pSurf->diff);
        PrintVector("pSurf->spec", pSurf->spec);
        PrintVector("pSurf->trans", pSurf->trans);

        // Floats
        PrintFloat("pSurf->fuzz", pSurf->fuzz);
        PrintFloat("pSurf->ior", pSurf->ior);
        PrintFloat("pSurf->shine", pSurf->shine);

        // Flags
        buffer[0]=0;
        if (pSurf->flags & S_CACHE)             strcat(buffer, "cache ");
        if (pSurf->flags & S_NO_ANTIALIAS)      strcat(buffer, "no-antialias ");
        if (pSurf->flags & S_TRANSFORM)         strcat(buffer, "transform ");
        if (pSurf->flags & S_TM_DIFF)           strcat(buffer, "tm_diff ");
        if (pSurf->flags & S_TM_SPEC)           strcat(buffer, "tm_spec ");
        if (pSurf->flags & S_TM_TRANS)          strcat(buffer, "tm_trans ");
        if (pSurf->flags & S_TM_AMB)            strcat(buffer, "tm_amb ");
        PrintText("pSurf->flags", buffer);

        // Bump map
        PrintAddress("pSurf->bump", pSurf->bump);
        print_bump(pSurf->bump);

        // Procedural texture
        PrintAddress("pSurf->tex", pSurf->tex);
        print_tex(pSurf->tex);

        // Texture maps
        PrintAddress("pSurf->tm_amb", pSurf->tm_amb);
        PrintAddress("pSurf->tm_diff", pSurf->tm_diff);
        PrintAddress("pSurf->tm_spec", pSurf->tm_spec);
        PrintAddress("pSurf->tm_trans", pSurf->tm_trans);

        // Matrix
        PrintMatrix("pSurf->matrix", pSurf->matrix);
    }
    else {
        PrintText("pSurf", "NULL");
    }
}


// Print a texture structure
static void print_tex(PTEXTURE pTexture) {
    if (pTexture) {
        PrintInteger("Texture #", ++g_num_texs);
        PrintAddress("pTexture->func", pTexture->func);
        PrintFloat("pTexture->blur", pTexture->blur);
        PrintFloat("pTexture->fuzz", pTexture->fuzz);
        PrintFloat("pTexture->r1", pTexture->r1);
        PrintFloat("pTexture->r2", pTexture->r2);
        PrintInteger("pTexture->terms", pTexture->terms);
        PrintVector("pTexture->trans", pTexture->trans);
        PrintVector("pTexture->scale", pTexture->scale);
        print_surf(pTexture->surf[0]);
        print_surf(pTexture->surf[1]);
        print_turb(pTexture->turbulence);
        print_waves(pTexture->waves);
    }
    else {
        PrintText("pTexture", "NULL");
    }
}


// Print a clip structure
static void print_clip(PCLIP pClip) {
    PrintInteger("Clip #", ++g_num_clips);
    if (pClip) {
        char buffer[MAX_TOKEN];
        PrintVector("pClip->center", pClip->center);
        PrintVector("pClip->normal", pClip->normal);
        PrintVector("pClip->apex", pClip->apex);
        PrintVector("pClip->base", pClip->base);
        PrintFloat("pClip->radius1", pClip->radius1);
        PrintFloat("pClip->radius2", pClip->radius2);
        PrintFloat("pClip->length", pClip->length);
        pClip->type;
        buffer[0] = 0;
        if (pClip->type & C_PLANE) strcat(buffer, "plane ");
        if (pClip->type & C_SPHERE) strcat(buffer, "sphere ");
        if (pClip->type & C_CONE) strcat(buffer, "cone ");
        if (pClip->type & C_INSIDE) strcat(buffer, "inside ");
        PrintText("pClip->type", buffer);
        PrintAddress("pClip->next", pClip->next);
    }
    else {
        PrintText("pClip", "NULL");
    }
}



