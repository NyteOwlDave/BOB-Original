<link rel="stylesheet" href="../assets/help.css"/>

[surf]: <../surf/surf.html>

# IMG File Format

This is the image file format. It uses RLE compression
on a per scanline basis.

Bob generates output images in this format.

# Layout

**NOTE**: All numeric values are BIG-ENDIAN.

| Offset | Type | Contents |
|--------|------|----------|
| 0 | WORD | Image width  (1-65535) |
| 2 | WORD | Image height (1-65535) |
| 4 | WORD | Starting scanline (0-65535) |
| 6 | WORD | Ending scanline (0-65535) |
| 8 | WORD | Bits-per-pixel (Always 24) |
| 10 | RLE | Scanline(s) in RLE 24-bit RGB format |

# Comments

Each RGB structure (color triplet) is 24-bits. One byte for each color
channel in B, G, R order (from LSB to MSB).

Each RLE run begins with a count (1-254), followed by a color triplet.

No RLE run can span multiple scanlines. In other words, the compression
algorithm only considers a single scanline at a time.

Scanlines may contain as little as one run or as many as 65535 runs.

If the file is malformed (due to, for example, interruption during
rendering), an encoded scanline may not contain the correct number of
runs. For underruns, the extra pixels on a scanline will contain
garbage. For overruns, the excess runs wrap to the next scanline.

If too few runs exist to read the required number of scanlines, the
file can't be read. If too many runs exist, the excess is ignored.

# Texture Maps

Bob accepts **texture maps** in this format. Texture maps are
applicable only to the context of the `surface` structure.

* See: [surface][surf]

---

