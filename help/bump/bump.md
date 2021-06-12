<link rel="stylesheet" href="../assets/help.css"/>

[wave]: <../wave/wave.html">
[turb]: <../turb/turb.html">
[surf]: <../surf/surf.html">

# Bump Structure

The bump structure is used to modify surface normals to create rough surfaces.

The bump structure contains `turbulence` and/or `wave` structures. These produce
noise in the surface normals and texturing.


# Usage

```
  bump {
    scale           x y z         // Scale bump space (discrete)
    translate       x y z         // Traslate bump space
    scale           f             // Scale bump space (uniform)
    turbulence      {}            // Turbulence block
    wave            {}            // Wave block
  }
```

# Transforms

The `scale` and `translate` keywords allow the noise to be modified relative
to the location of the related primitive(s).

The `translate` keywords always takes three arguments (x, y anx z).

The `scale` keyword may take either three (x, y and z) or one (k). When one
argument is specified, it is used for all three dimensions.

# Turbulence Structure

The `bump` structure may optionally contain a `turb` structure.

* See: [Turbulence](turb)

# Wave Structure

The `bump` structure may optionally contain a `wave` structure.

* See: [Wave][wave]

# Surface Structure

Bump is used in **only** in the context of the `surface` structure.

* See: [Surface][surf]

---