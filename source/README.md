
# BOB Original

This project is a rewrite of a rewrite of the original BOB ray tracer
app included as source code in the book _Photorealism and Ray Tracing in C_.

For more specific details about the book, see the `author.h` include file.

# Compatibility

The project was built and tested on Ubuntu 18.04 LTS using GNU C.
The version of GCC was 7.5.0.

# Preparation

The app requires a specific file system arrangement. This can be
generated with the `make-rt` script in `BOB-Original`.

This command only builds the file folders needed for the runtime
system.

__NOTE__: `build` calls `make-rt`.

# Build

Building the project is easy. Just run the `build` script in
`BOB-Original`. This will create an executable named `bob` in the `rt` folder.

It will also create a file named `yyz.txt` in the 
the `source\obj` folder. You can examine this to check for
compile or links errors, respectively.

__NOTE__: `build` calls `make-rt`.

# Clean

The `clean` script in the `source/obj` folder will remove object files and
the temporary report file `yyz.txt` mentioned above.

# Cloning

You can use the `clone-repo` script to clone any of the four Bob repositories.

Just type one of:

```
./clone-repo original
./clone-repo runtime
./clone-repo tools
./clone-repo help
```

Note that this is the original repo, so that option is included only for
completeness.

You can use the `clone-repo` script comments for better understanding.

---

