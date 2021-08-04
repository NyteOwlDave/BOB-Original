/*
	config.h

	Configuration settings

*/


#ifndef CONFIG_DEFINED
#define CONFIG_DEFINED

#define STABLE_VERSION  "v1.0.2"        // Stable version number
#define STABLE_DATE     "Aug 4, 2021"   // Stable version date

#define	NSLABS			(3)			// number of bounding slab axes
#define	BUNCHINGFACTOR	(4)			// composite bunch size
#define PQSIZE          (1000)		// Priority queue size

#define L_SAMPLES       (8)			// default # samples for spherical lights
#define	MIN_LIGHT		(0.005)		// min spotlight size

#define F_SAMPLES       (8)			// default # samples for depth of field

#define MAXLEVEL        (20)		// max recursion level, start at 0
#define MINWEIGHT       (0.0001)	// min weight for a ray to be considered

#define	MAX_TOKEN		(80)		// max token length
#define MAX_PARAMS		(10)		// max number of parameters for file
#define MAX_PATH		(260)		// Maximum pathname length

#define MAX_STRING_LEN	(1<<24)		// Maximum string length

#define ADAPT_DIST      (8)         // lens adaptive distance

#endif // !CONFIG_DEFINED

