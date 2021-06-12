<link rel="stylesheet" href="../assets/help.css"/>

[bbs-file]: <bbs-file.html>
[bc-file]: <bc-file.html>
[bl-file]: <bl-file.html>
[bo-file]: <bo-file.html>
[img-file]: <img-file.html>
[map-file]: <map-file.html>
[studio-file]: <studio-file.html>

# File Formats

This folder contains documentation for the file formats used by Bob.

# Index

| Topic | Description | Notes |
| - | - | - |
| [BBS File][bbs-file] | Preprocessor output | 1 |
| [BC File][bc-file] | Color macro definitions | 2, 3 |
| [BL File][bl-file] | Light structures | 2, 3 |
| [BO File][bo-file] | Primitive structures | 2, 3 |
| [IMG File][img-file] | Output images, texture maps | 4 |
| [MAP File][map-file] | Background palette | 5 |
| [STUDIO File][studio-file] | Studio structures | 2, 3 |

## Notes

1. Cooked mode
2. Raw mode
3. Can include any source code
4. RLE compressed RGB pixel map
5. RGB palette


# Raw/Cooked Modes

You'll see throughout the text references to **raw** and **cooked** source code.

The **raw** code is what you design in. It's the fill language, including macros.

The **cooked** code has macros expanded, directives and line-comments stripped.

The raw mode is what is fed into the _preprocessor_ and cooked mode is what comes out.

The _interpreter_ **only** understands cooked mode.

---
