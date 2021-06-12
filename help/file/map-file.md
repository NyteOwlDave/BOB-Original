<link rel="stylesheet" href="../assets/help.css"/>

[bc]: <bc-file.html>
[bkg]: <../bkg/bkg.html>

# Map Files

Map files define color palettes. They should contain exactly 256 entries.

# Entry Format

The format for each entry is:

```
r g b
```

Where:

**r** is red

**g** is green

**b** is blue

Each component is specified in the standard VGA range [0..255].

# BC Files

`Map` files are not to be confused with `BC` files.

* See: [BC File][bc]

# Backgrounds

Map files are exclusive to the `bkg` structure.

* See: [bkg][bkg]

---
