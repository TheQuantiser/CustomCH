MSSM Limits with RooMorphingPdf {#MSSMWithMorphing}
===================================================
**This example demonstrates the use of the RooMorphingPdf and other RooFit objects to build a complete MSSM signal model within a single datacard.** From this datacard we will build a workspace in which \f$m_{A}\f$ and \f$\tan\beta\f$ are free model parameters which can be scanned or fixed as needed, in exactly the same way as for other physics models, e.g. the \f$\kappa_{f}\f$ vs \f$\kappa_{v}\f$ model in the SM analysis. However, unlike the \f$\kappa_{f}\f$, \f$\kappa_{v}\f$ model where the parameters simply scale the signal processes with simple formulae, the \f$m_{A}\f$,\f$\tan\beta\f$ model must fix the masses, cross sections and branching ratios for each of the \f$h\f$, \f$H\f$ and \f$A\f$ bosons using the 2D scans provided by the LHC Higgs XS working group.

Getting Started
===============
Install the project as described in
[`StandaloneInstallation.md`](StandaloneInstallation.md). The
[HiggsToTauTau](https://github.com/cms-analysis/HiggsAnalysis-HiggsToTauTau)
and
[auxiliaries](https://github.com/roger-wolf/HiggsAnalysis-HiggsToTauTau-auxiliaries)
repositories are also required and should be checked out alongside
CombineHarvester.

The RooMorphingPdf and other tools are built automatically as part of
the CMake build. We will build the datacard using the program
`bin/MorphingMSSM`. Take a look through the source code in
`test/MorphingMSSM.cpp`.

