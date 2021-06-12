<link rel="stylesheet" href="../assets/help.css"/>

# Bob

This is the ray tracer app. Many of the default settings can be
overiden from the command line. These are shown in usage, below.

# Usage

```
bob [flags] <session> 

-c  start interactive console
-e  start interactive console on error
-s  set silent mode
-r  resume generating an interrupted image
-i  change image size to xres by yres
-l  change start and stop line
-n  no shadows
-d  limit recursion level to depth
-a  change antialiasing to mode.
-p  run without the preprocessor
-o  run only the preprocessor
-b  set bunching factor
```

The session name should have no extension. Bob will apply two extensions to this name for input and output files.

The input is `<session>.bob`.

The output is `<session>.img`.


---

# Console (-c and -e)

The `-c` flag tells Bob to start the interactive
console immediately after rendering the scene.

The `-e` flag tells Bob to start the interactive
console if there is a fatal error.

NOTE: If the session name is omitted, the `-c` flag is assumed. This starts the
console.

---

# Silent Mode (-s)

Normally, Bob show statistics when rendering, as well as various other
status messages. This behavior may not be desirable for batch mode. You
can inhibit these messages with the silent mode.

---

# Resume Mode (-r)

Bob is designed to to jobs in total or in part. Resume mode picks up
where a partial job was performed.

YOu can also use the Start/Stop Line Mode (see below).

---

# Change Image Size (-i)

The default image size is 320 by 240 pixels. This can be modified in the
`studio` block. It can also be overridden on the command line.

```
bob -i <w> <h>
```

Where `w` is width and `h` is height of the output image.

---

# Change Start/Stop Lines (-l)

By default, the start line is zero and the stop line is the output
image's height minus 1. You can override this to perform a partial
rendering.

```
bob -l <start> <stop>
```

NOTE: This overrides any start/stop lines specified in the `studio` block.

---

# No Shadows Mode (-n)

Turns off shadows. This speeds up rendering at the expense of realism.

NOTE: This works with no_shadows specified in the `studio` block. You
can turn off shadows in either place (but not on, which is the default).

---

# Limit Recursion Depth (-d)

Recursion depth is one of the most important factors for rendering speed.
By default, the maximum is 20. You can modify this.

```
bob -r <depth>
```

NOTE: This overrides any depth specified in the `studio` block.

---

# Change Antialiasing Mode (-a)

Bob has four available antialiasing modes:

| Mode | Description |
| - | - |
| none | No antialiasing (fastest) |
| corners | Shoot the four corners (fast) |
| quick | Shoot a moving window (slow) |
| adaptive | Adaptive (slowest) |

Generally, the slower a mode is the more realistic the results.

Use any of the four modes as follows:

```
bob -a <mode>
```

The default is no antialiasing.

NOTE: This overrides any antialiasing specified in the `studio` block.

---

# Preprocessing (-o and -p)

Bob contains a build-in preprocessor. The primary purpose of this
subsystem is macro expansion. But it also removes commends and
combines all source files into a single module. This facilitates
the operation of the interpreter subsystem.

If you don't need the preprocessor for a job, you can omit it:

```
bob -p
```

If you want to only preprocess of a file (skip rendering), you
can run it with:

```
bob -o
```

---

# Set Bunching Factor (-b)

The manual states that the bunching factor is a constant, but this
turns out to be false. Apparently the original coders decided it
was worthwhile to make it programmable.

The bunching factor is used with the median cut algorithm for
bounding boxes. It determines the maximum number of primitives
permitted in a composite object.

```
bob -b <bunching>
```

---

# Studio Overlap

If you've read closely this document, you will have noticed that
several of the command line flags override similar settings in
the `studio` block.

For convenient and completeness, these are summarized here:

| Flag | Keyword in Studio |
| - | - |
| -a | antialias |
| -b | bunching |
| -d | depth |
| -i | resolution |
| -l | start stop |
| -n | no_shadows |


---