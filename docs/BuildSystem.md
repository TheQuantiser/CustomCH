Build System {#build}
=====================

CombineHarvester uses [CMake](https://cmake.org/) as its build system.
The repository is organised into packages, each contained within a
subdirectory that produces shared libraries and executable programs. The
core package `CombineHarvester/CombineTools` contains the main framework
and example programs. Additional analysis-specific packages can be added
in the future.

To configure and build the project run:

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
cmake --install build
```

The libraries and executables are placed in the build directory.  The
`cmake --install` step installs headers and libraries into the chosen
prefix (by default the active Conda environment or the first entry in
`CMAKE_PREFIX_PATH`).

Each package follows a conventional layout:

    interface/    : header files
    src/          : source code (.cc) compiled into the library
    bin/          : small example and helper programs

Executables can be built individually with `cmake --build build --target <name>`
and are written to the `build/<package>` directory.

