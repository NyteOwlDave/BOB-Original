<link rel="stylesheet" href="../assets/help.css"/>

# BBS File

These are the output files from the _preprocesser_.

The preperocessor takes raw mode files and converts then to cooked mode.

Cooked mode strips out preprocessor directives and line-style comments. It also
expands macros and combines raw mode files into a single source module.

All other file formats are either special case (like images) or raw mode.

# Default

The preprocessor always writes output to a file named `yyz.bbs`. If you want
to keep the preprocessed file for later, you should rename it and save it to
a subfolder in the BOB environment.

## VV Extensions

The `#include` directive is replaced by the `vv_newfile` command. This precedes
the included content in the cooked file.

At the end of the included content, the preprocessor places a `vv_popfile` command.

Together, these allow the processor to report error locations in the proper raw
file. The line numbers for error reporting will still be related to the cooked file.

---

