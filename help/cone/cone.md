<link rel="stylesheet" href="../assets/help.css"/>

[prim]: <../prim.html>

# Cone Structure

The cone is one of the five primitive objects. 

# Cone Kinds

For flexibility, the apex may or may not be a point. Rather you can set a radius for the apex, making it a hole just as the base is a hole.

When the apex and base radii are the same, the cone is in face a **pipe**. When the apex or base radius is zero, the cone is a **dunce cap** shape. In all other cases, the cone is a **funnel** shape.

## Degenerate Cases

You should always avoid degenerate cases in primitives. Basically, this is any case which would cause the volume (or area for some) to be zero.

For the cone, this can happen if you set both radii to zero. It can also happen if you set the base and apex points to be the same location.

# Usage

```
  cone {
    apex            x y z         // Where the apex point is located
    base            x y z         // Where the base point is located
    apex_radius     f             // Radius of the apex ring
    base_radius     f             // Radius of the base ring
    radius          f             // Radius of the base ring
    clip            {}            // One or more clip blocks
  }
```

# Cap Shape

For a dunce cap shape, set either `base_radius` or `apex_radius` to zero and the other to some non-zero real value.

```
cone {
base 0 0 1
apex 0 0 5
base_radius 3   // Wide base hole
apex_radius 0   // Np apex hole
}
```

<img src="../art/cone-cap.png" />

The figure above shows a dunce cap shaped cone.

# Funnel Shape

For a funnel shape (wide hole at one end, narrow hole at the other), set `apex_radius` to some
value other than the value of `base_radius`;

```
cone {
base 0 0 1
apex 0 0 5
base_radius 3   // Wide base hole
apex_radius 1   // Narrow apex hole
}
```

<img src="../art/cone-funnel.png" />

The figure above shows a funnel shaped cone.

# Pipe Shape

For a pipe with a uniform radius, set `radius` to some real value. Omit the base and apex radius values.

```
cone {
base 0 0 1
apex 0 0 5
radius 3    // apex_radius and base_radius
}
```

<img src="../art/cone-pipe.png" />

The figure above shows a pipe shaped cone.

# Primitive Shapes

There are five primitive objects altogether. These are discussed as a whole.

* See: [Primitive Objects][prim]

---
