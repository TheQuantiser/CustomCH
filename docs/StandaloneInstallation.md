# Standalone Installation

This guide describes how to build and run CombineHarvester as a standalone project.

## Prerequisites

The following external libraries must be available and discoverable by CMake:

* [ROOT](https://root.cern) with the RooFit and RooStats components
* [Boost](https://www.boost.org/)
* [libxml2](http://xmlsoft.org/)
* [vdt](https://gitlab.cern.ch/vdt/vdt)
* [HistFactory](https://root.cern.ch/doc/master/group__HistFactory.html)

ROOT often bundles vdt and HistFactory.  Set `ROOTSYS` to the location of your
ROOT installation if required.

## Repository setup

```bash
git clone https://github.com/TheQuantiser/CustomCH.git
cd CustomCH
git submodule update --init
```

The CMake build will automatically download the
[HiggsAnalysis/CombinedLimit](https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit)
`main` branch sources if they are not already present.

If you have `HiggsAnalysis/CombinedLimit` available as a separate installation,
skip the download by configuring CombineHarvester with

```bash
cmake -S . -B build -DUSE_SYSTEM_COMBINEDLIMIT=ON \
      -DCMAKE_PREFIX_PATH=/path/to/combinedlimit
```

`find_package(HiggsAnalysisCombinedLimit)` searches the locations listed in
`CMAKE_PREFIX_PATH` for an installation that provides
`HiggsAnalysisCombinedLimitConfig.cmake`.

When working from a CombinedLimit source tree built with the provided
`Makefile` (for example after running `make CONDA=1 -j8` in the
`HiggsAnalysis/CombinedLimit` directory), point CMake to the checkout with

```bash
cmake -S . -B build -DUSE_SYSTEM_COMBINEDLIMIT=ON \
      -DHiggsAnalysisCombinedLimit_ROOT=/path/to/HiggsAnalysis/CombinedLimit
```

so that the headers and library are picked up correctly. In all cases no
download occurs and the build links against the existing library.

If a build directory for `HiggsAnalysis/CombinedLimit` is present alongside the
CombineHarvester source tree (for example `../HiggsAnalysis/CombinedLimit/build`),
it will be detected automatically when configuring with
`-DUSE_SYSTEM_COMBINEDLIMIT=ON`.

## Building with CMake

```bash
cmake -S . -B build
cmake --build build --target install
```

Without additional options the installation step writes into the active Conda
environment (`$CONDA_PREFIX`) when defined, or the first path listed in
`CMAKE_PREFIX_PATH`.  Specify `-DCMAKE_INSTALL_PREFIX=/desired/path` during
configuration to override this behaviour.

## Environment setup

```bash
export CH_BASE=$(pwd)
source build/setup.sh
```

## Smoke test

Run one of the example programs to verify the installation:

```bash
$CH_BASE/build/bin/Example1
```

This should print a short message and exit without error.
