
# Bob Original
Bob Ray Tracer

See: `./source/README.md` for details.

# Registry Script

Each of the Bob repositories must be registered after they are cloned. Each
contains a script named `register` for that purpose.

Basically this script has these functions:

1. Register the local path for a repository
2. Report on the local path for a repository
3. Show a list of registered repositories

## Register Function

Registers the current working directory as the location of the repository
it contains.

## Report Function

Reports the current working folder for any of the four repositories that
have been locally cloned.

## Directory Function

Shows a list of the names of repositories that have already been registered.

# Why Register?

The ray tracer and all tools will work fine without registering any repos. The
registration process is to assist in some of the build scripts, in locating
stuff that might not be in an easy to find location. Or where you've forgotten
the location.

If you know the location of any of the repositories, you can open a terminal
for that folder and invoke the `register` script to check out the registry.

# Script Differences

Although each of the four repositories has a copy of `register`, they are
not identical.

Specifically, the `PROJECT` variable near the top is modified in each to
contain the simplified name of the repository, according to this table:

| Name | GitHub Repository | Notes |
|-|-|-|
| original | BOB-Original |
| runtime | BOB-Runtime |
| tools | BOB-Tools |
| help | BOB-Help |

---
