Reproducing Run 1 H->tautau results {#introrun1HTT}
=========================

Using all of the techniques described previously, both in terms of datacard production and usage of the RooMorphingPdf object for signal processes, code to reproduce many of the Run 1 H->tautau results is included in the package. For some analyses, development was first performed for the datacard production without morphing applied, and fully validated, before moving to using morphing for the signal process. Note that in the non-morphing version many of the analyses have an equivalent python version. Once the usage of RooMorphingPdf for signal was validated in several use cases some additional analyses were added making use of this method only. This section describes how to find the code for each of the legacy analyses and details any specifics beyond the previous examples. More detail on the validation which was made can be found in the analysis note AN-15-235. Note that to run all of the examples below, the shape files exist in /auxiliaries and are linked to the script correctly. For more information on running the statistical results with the produced datacards, see later sections.


Systematics for legacy H->tautau results {#run1HTTsystematics}
=========================

**Files** CombineTools/interface/HttSystematics.h, CombineTools/python/systematics

For analyses with a large number of systematic uncertainties, it is neater to detail these in a separate file from the main datacard production code. Files for the different analyses can be found at the paths above, either in c++ or python. 

Legacy SM H->tautau results {#run1HTTSM}
=========================

The previous example scripts for producing the legacy SM H->tautau datacards
have been retired and are no longer distributed with CombineHarvester. Refer to
the repository history if the original code is required for reference.


Run 1 H->hh->bbtautau and A->Zh->lltautau results {#run1HTTHhhAZh}
=========================

**Files** Run1BSMComb/bin/AZh.cpp, Run1BSMComb/bin/Hhh.cpp, CombineTools/scripts/HhhExample.py

**Files** Run1BSMComb/bin/MorphingAZh.cpp, Run1BSMComb/bin/MorphingHhh.cpp

The above scripts illustrate the datacard production for the H->hh and A->Zh analyses of HIG-14-034. The cards are very similar to those shown previously. The H->hh analysis makes use of the bin by bin merging functions exactly as described for the SM analysis. The A->Zh analysis makes use of one feature described for the SM cards- the ability to multiply signal by a constant factor. In this case the factor is 1000 to put the signal into femptobarns instead of picobarns.  

The validation of the produced cards as compared to the official cards can be found in the Analysis note.


MSSM update H->tautau results {#run1HTTMSSM}
=========================

**File** Run1BSMComb/bin/MorphingMSSMUpdate.cpp

The fit model for the MSSM update analysis is similarly complicated to the SM legacy analysis. There are a couple of unique features which are illustrated below:

1) Possibility to setup 3 Higgs bosons

In the MSSM there are three neutral Higgs bosons, and to set a limit on a particular MSSM model the signal model contains all three for the correct mass and ratios of cross section times branching ratios. For the model dependent limits therefore we need to have 6 separate signal processes in the datacards, one for each of the three neutral Higgs bosons, multiplied by two for the two different signal production mechanisms. This is done by mapping what exists in the shape files (two signal production processes generically named ggH and bbH, which can stand for any one of the three Higgs bosons) to the signal processes we want, using the following:

\snippet Run1BSMComb/bin/MorphingMSSMUpdate.cpp part1

When declaring the processes to be added to the CH instance, the full set of signal processes is included:

\snippet Run1BSMComb/bin/MorphingMSSMUpdate.cpp part2

Whereas when reading in the information from the shape file the usual names are used the individual ggH and bbH templates are used to fill the processes for all 3 Higgs bosons:

\snippet Run1BSMComb/bin/MorphingMSSMUpdate.cpp part3

When running model dependent limits, the 6 signal processes are used and manipulated in the required way to build a signal template for a given mA-tanb point. When running model independent limits, the datacards are simply setup with only one neutral Higgs boson, as appropriate for a single resonance search.

2) Tail fit uncertainties

During Run 1 for the MSSM analysis an analytic fit was performed to the high mass tail of the mass distribution for some of the backgrounds and associated systematics were included, in order to reduce problems with low statistics in the tails. For the initial implementation of the Run 1 cards, these tail fits are added 'by hand', i.e. directly included in the input shape file rather than the fits rerun. The systematics are then lifted from the shape file directly as shape uncertainties in the format:

\snippet CombineTools/src/HttSystematics_MSSMUpdate.cc part1

The validation of the MSSM update analysis has been performed in terms of the model independent and model dependent limits and is detailed in the Analysis note.
