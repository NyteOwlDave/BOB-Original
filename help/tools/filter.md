<link rel="stylesheet" href="../assets/help.css"/>

# Filters

Filters are used to anti-alias images. They use a
weighted averaging algorithm, where the weights
and the divisor are all programmable.

The `filter` tool **only** accepts images that are stored in the Bob IMG File Format.

## Filter Layout

```
[a][b][c]
[d][e][f]
[g][h][i]
[divisor]
```

The filter is a 3x3 matrix containing relative weights.
This is a moving window, where [e] represents the weight of the
pixel at the center of the window. The remainder of the weights
are laid out according to the relative positions of all
pixels that are adjacent (including diagonals).

The value of each pixel's RG and B channels is multiplied
by the corresponding matrix weight. These are summed up for
all 9 pixels. The resulting pixel's RG and B channels is then
divided by the divisor.

Note that, for the math to work out properly, the divisor
should be:

```
divisor >= a+b+c+d+e+f+g+h+i
```

## Example

For example, let's look at a filter:

```
[a][b][c]
[d][e][f]
[g][h][i]
[divisor]
```
Here's our pixel window (within the image file):

```
[m][n][o]
[p][q][r]
[s][t][u]
```

Here's how we calculate the revised center pixel [q"].

```
m' = m*a
n' = n*b
o' = o*c
p' = p*d
q' = q*e
r' = r*f
s' = s*g
t' = t*h
u' = u*i
q" = q' / divisor
```

Now bear in mind that each of these values is an RGB pixel
mulitplied by an integer, so for each multiply above, three actual multiplications will take place. The same goes for the division at 
the end. In other words, [m] through[u] are all pixels. So are [m'] though [u']. And so is [q"].

## Filter Types

* Average
* Diagonal Laplace
* Horizontal
* Laplace
* LMR
* Unity
* Vertical

# Average

Simple average.

```
1 1 1
1 1 1
1 1 1
9
```

# Diagonal Laplace

Diagonal Laplace.

```
-1 0 -1
 0 4  0
-1 0 -1
 1
```

# Horizontal

Horizontal Laplace.

```
0 -1 0
0  2 0
0 -1 0
1
```

# Laplace

Laplace.

```
 0 -1  0
-1  4 -1
 0 -1  0
 1
```

# LMR

Laplace M-R.

```
-1 -1 -1
-1  9 -1
-1 -1 -1
1
```

# Unity

Unity (no change).

This is primarily for trouble-shooting purposes.

```
0 0 0
0 1 0
0 0 0
1
```

# Vertical

Vertical Laplace.

```
 0 0  0
-1 2 -1
 0 0  0
 1
```

---
