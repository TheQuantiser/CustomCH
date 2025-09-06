Standalone Installation
=======================

This guide describes how to build *CombineHarvester* without relying on a
CMSSW release area.

## Dependencies

The project requires a C++17 compiler together with a number of external
packages:

* [CMake](https://cmake.org/) 3.12 or newer
* [ROOT](https://root.cern/) with the *RooFit* and *RooStats* components
* Boost (`program_options`, `filesystem`, `system`)
* LibXml2
* VDT
* HistFactory
* [HiggsAnalysis-CombinedLimit](https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit)

The `HiggsAnalysis-CombinedLimit` dependency is required because
`CombineHarvester` links against the `libHiggsAnalysisCombinedLimit`
library.  During configuration CMake will attempt to download this
package automatically using `FetchContent`.  If the download is not
possible (for example on networks without direct internet access), clone
the dependency by hand before running CMake:

```bash
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
```

Alternatively an already installed version can be used by enabling the
`USE_SYSTEM_COMBINEDLIMIT` option and pointing CMake to its location via
`-DHiggsAnalysisCombinedLimit_DIR=/path/to/lib/cmake/HiggsAnalysisCombinedLimit`.

These packages can be provided by the host system or through a Conda
environment (see below).

## Optional Conda environment

A convenient way to obtain the dependencies is via
[Conda](https://conda.io/) or [Mambaforge](https://github.com/conda-forge/miniforge).
The following commands create and activate an environment that contains
the required tools:

```bash
mamba create -n ch cmake root boost libxml2 vdt histfactory python
conda activate ch
```

## Building with CMake

Clone the repository and build the code in a separate directory:

```bash
git clone https://github.com/cms-analysis/CombineHarvester.git
cd CombineHarvester
cmake -S . -B build
cmake --build build -j$(nproc)
cmake --install build
```

By default the installation prefix is taken from the active Conda
environment or the first entry in `CMAKE_PREFIX_PATH`.  Adjust
`CMAKE_INSTALL_PREFIX` when running `cmake` if a different location is
desired.

After installation the libraries and Python bindings are available from
the chosen prefix and `CombineHarvester` can be used like any other
installed package.

Command-line tools such as `ChronoSpectra` are installed into the `bin`
directory of that prefix, which is typically added to the `PATH` by
Conda environments.

