Build System {#build}
====================

CombineHarvester is built with [CMake](https://cmake.org/) and no CMSSW
environment is required.  The repository contains two main packages,
`CombineTools` and `CombinePdfs`, which are configured and compiled in a
single CMake project.

See the [standalone installation instructions](../README.md#installation)
for the initial checkout of the repository.  A typical build workflow is:

```
cmake -S . -B build
cmake --build build -j4
```

Libraries and executables are placed in `build/lib` and `build/bin`.  The
project can optionally be installed to a custom prefix:

```
cmake --install build --prefix /path/to/prefix
```

Set the `CH_BASE` environment variable to the repository location (or the
installation prefix) so that the helper tools can locate auxiliary
resources:

```
export CH_BASE=$(pwd)
```

CMake handles dependency tracking automatically, so incremental builds only
recompile files that have changed.  To clean the build simply delete the
`build` directory.
