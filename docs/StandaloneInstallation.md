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
git clone https://github.com/cms-analysis/CombineHarvester.git
cd CombineHarvester
git submodule update --init
```

## Building with CMake

```bash
cmake -S . -B build
cmake --build build --target install
```

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
