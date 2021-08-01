/*
	extern.h

	Global variables

*/

#ifndef EXTERN_DEFINED
#define EXTERN_DEFINED


#include <stdlib.h>
#include <stdio.h>
#include "proto.h"


// Screen
extern int 				Xresolution;		// screen width
extern int 				Yresolution;		// screen height

// Ray
extern Flt				rayeps;				// some tiny number
extern Flt				minweight;			// minimum weight

// Session
extern int				start_line;			// session start line
extern int				stop_line;			// session stop line

// Studio
extern Viewpoint 		Eye;				// location of camera in world space
extern Camera			camera;				// camera configuration
extern int				bunching;			// composite bunch size
extern int				antialias;			// antialias mode
extern int				adapt_dist;			// adaptive distance
extern BOOL				jitter;				// jitter flag

// Lights
extern Light*			light_head;			// ptr to head of linked list of lights
extern int				nLights;			// number of lights in list

// Environment
extern BOOL				no_shadows;			// disable shadows
extern BOOL				caustics;			// no fake caustics
extern BOOL				exp_trans;			// enable exponential transparency
extern BOOL				fuzzy_ray;			// enable fuzzy rays

// Scene
extern Background		background;			// background configuration
extern Color			Ambient;			// ambient color
extern Color			HazeColor;			// haze color
extern Flt				HazeDensity;		// haze density

// Surfaces
extern Surface*			CurrentSurface;		// ptr to current surface (during parsing)
extern Stack*			SurfTop;			// ptr to top of surface stack

// Transforms
extern Transform*		TransTop;			// ptr to head of linked list of transforms

// Slabs
extern Vec				Slab[];				// bounding slabs

// Primitive objects
extern ObjectProcs		NullProcs;			// composite bunch function v-table
extern Object*			Root;				// head of linked list of primitive objects
extern int	    		nPrims;				// number of primitive objects (shapes)

// Clips
extern Clip*			ClipTop;			// ptr to head of linked list of clips
extern GlobalClip*		GlobalClipTop;		// ptr to head of linked list of global clips

// Job
extern char*			Progname;			// ptr to name of executable file
extern char			    Infilename[];	    // input filename buffer
extern Stack*			InfileTop;			// ptr to top of input file stack

// Stats
extern int				maxlevel;			// maximum allowable trace recursion depth
extern int				deepest;			// deepest trace recursion depth reached
extern DWORD			nRays;				// number of rays traced
extern DWORD			nShadows;			// number of shadow feeler rays traced
extern DWORD			nReflected;			// number of reflected rays traced
extern DWORD			nRefracted;			// number of refracted rays traced
extern DWORD			MemAllocated;		// total memory allocated by vmalloc
extern DWORD			maxQueueSize;		// maximum priority queue size
extern DWORD			totalQueues;		// total number of priority queue items
extern DWORD			totalQueueResets;	// total number of priority queue resets
extern DWORD			nChecked;			// number of shapes checked for possible queue
extern DWORD			nEnqueued;			// number of shapes in queue
extern DWORD			nShadowCacheHits;	// number of shadow cache hits

// Modes
extern BOOL				tickflag;			// enable statistics reporting
extern BOOL				resume;				// resume a prior session
extern BOOL             console;            // Enter console mode on error

// Copyright
extern LPCSTR			_Copyright;			// copyright notice
extern LPCSTR			_Program;			// program name
extern LPCSTR			_Version;			// program version
extern LPCSTR			_Date;				// program release date


#endif // !EXTERN_DEFINED

