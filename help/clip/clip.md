<link rel="stylesheet" href="../assets/help.css"/>

# Clip Structure

Clips all subtractive solid geometry logic. In other words, they allow you
to carve away all or part of a primitive.

## Clip Uage

```
  clip {
    apex            x y z         // Apex point's location (cone)
    base            x y z         // Base point's location (cone)
    center          x y z         // Center point's location (all)
    normal          x y z         // Normal vector (varies)
    apex_radius     f             // Radius of the apex ring (cone)
    base_radius     f             // Radius of the base ring (cone)
    radius          f             // Radius for spherical clips
    inside                        // Keep the inside
    outside                       // Keep the outside
  }
```

## Clip Kinds

There are three kinds of clips:

* Plane
* Cone
* Sphere

There is no way to explicitly specify the clip kind in the language. Rather Bob infers the kind, based on the values given inside the block.

The basic rule is that if an apex or base is given, it's a cone. Otherwise, if a radius is given, it's a sphere. If none of these is given, it's a plane.

## Inside/Outside

The default is to keep **outside** the clip region. Hence the `outside` keyword is superfluous. To keep the **inside** of the clip region, use the `inside` keyword.

## Plane Sample

```
sphere {
	center -1.5 0 .5
	radius .5
	clip {
		center -1.5 0 .375
		normal 0 0 -1   // only keep bottom part
	}
}
```

<img src="../art/planar-clip.png"/>

The figure above shows a sphere clipped by a plane.

## Cone Sample

```
global_clip {
	clip {
		base 0 0 -1
		apex 0 0 10
		base_radius 1.7
		apex_radius 5.2
	}
}
ring { 
	center 0 0 0
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 2 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 4 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 6 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 8 
	normal 0 0 1
	radius 10 
}
```

<img src="../art/cone-clip.png"/>

The figure above shows five rings clipped by a cone.

## Sphere Sample

```
global_clip {
	clip {
		center 0 0 4.75
		radius 4.7
	}
}
ring { 
	center 0 0 0
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 2 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 4 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 6 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 8 
	normal 0 0 1
	radius 10 
}
```

<img src="../art/sphere-clip.png"/>

The figure above shows five rings clipped by a sphere.

## Inside Sample

```
global_clip {
	clip {
		center 0 0 4.75
		radius 4.7
		inside
	}
}
ring { 
	center 0 0 0
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 2 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 4 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 6 
	normal 0 0 1
	radius 10 
}
ring { 
	center 0 0 8 
	normal 0 0 1
	radius 10 
}
```

<img src="../art/inside-clip.png"/>

The figure above shows five rings clipped by a cone. The sense is inverted so that the inside of the clip region is kept, rather than the outside.

# Global Clips

Global clips are used in the top-level context and apply to all subsequent
primitives.

## Global Clip Usage

```
  global_clip {
    clip            {}            // One or more clip blocks
  }
```

---
