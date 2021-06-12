<link rel="stylesheet" href="../assets/help.css"/>

[map]: <map-file.html>

# BC Files

It's often handy to define a set of macros for common colors. This is the purpose
of the `bc` file format (a raw mode file).

Typically, it just contains a set of `#define` preprocessor directives.

# Sample

```
#define alice_blue 0.9411765 0.9725491 1
#define antique_white 0.9803922 0.9215687 0.8431373
#define aqua 0 1 1
...
```

# Additional Comments

This is **not** a palette, which uses the `map` format instead.

* See: [Map Files][map]

---

