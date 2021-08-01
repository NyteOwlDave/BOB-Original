
# BOB Original

This project is a rewrite of a rewrite of the original BOB ray tracer
app included as source code in the book _Photorealism and Ray Tracing in C_.

For more specific details about the book, see the `author.h` include file.

# Compatiblity

The project was built and tested on Ubuntu 18.04 LTS using GNU C.
The version of GCC was 7.5.0.

# Preparation

The app requires a specific file system arrangement. This can be
generated with the `make-rt` script in `BOB-Original`.

This command only builds the file folders needed for the runtime
system.

# Build

Building the project is easy. Just run the `build` script in
`BOB-Original`. This will create an executable named `bob` in the `rt` folder.

It wll also create a file named `yyz.txt` in both the `source` and
the `source\obj` folders. You can examine these to check for
compile or links errors, respectively.

---
