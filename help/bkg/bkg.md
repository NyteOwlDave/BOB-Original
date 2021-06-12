<link rel="stylesheet" href="../assets/help.css"/>

[studio]: <../studio/studio.html>
[map]: <../file/map-file.html>

# Bkg Structure

The `bkg` structure is used to create a default background for rays that
do not intersect any primitives.

There are two kinds of background:


* Simple
* Paletted

## Simple Background

The simple background is just a single solid color. The default is black.

## Paletted Background

The paletted background employs a palette, a color and a vector. The vector determines
the "up" direction for linear interpolation into the palettes.

For best results, the palette itself should be interpolated between to colors.
Generally, this is black or white and some other color.

The color is provided in cases where the ray is orthogonal to or points away from
the background. This operates like the single color mode.

# Usage

## Simple Background

```
  background      r g b           // Solid background color
```

## Paletted Background

```
  background {
    color           r g b         // Background color
    up              x y z         // Up direction
    filename.map                  // RGB color map (256 entries)
  }
```

# Synonym

The `bkg` keyword is a synonym for `background`.

# Map File

Palettes use the `map` file format.

* See: [Map File][map]

# Studio Structure

The `bkg` structure can only be used in the context of the `studio` structure.

* See: [studio][studio]

---
