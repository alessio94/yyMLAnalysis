systematicVariations
=========================

To book systematic variations (p4 and scale-factor variations) the txt files contained in this directory can be added to the analyze step. Four-vector variations are added with the tag `xAODsfSystematicsList`, scale-factor systematics with `xAODp4SystematicsList`. Variations dedicated to a certain process (mainly MC weight variations) can be added with tags `xAODProcessSpecificsfSystematicsPaths` and `xAODProcessSpecificsfSystematicsLists`, i.e. for example to book ggf uncertainties:

    xAODProcessSpecificsfSystematicsPaths: sig/?/?/ggf, ...
    xAODProcessSpecificsfSystematicsLists: auxData/systematicVariations/ggF/signal-variations.txt, ...

See also the [VBF default config](https://gitlab.cern.ch/atlas-physics/higp/nresmultileptons/hww/HWWAnalysisCode/blob/master/share/config/master/VBF/analyze-VBF-default-sys.cfg#L21-29) file as an example.

## Treatment of MC weights
In the PAODs, the MC weights are stored in the `EventInfoAuxDyn.mcEventWeights[]` vector. Some of the weights (e.g. pdf weight variations) are also added as decorations to the `EventInfo` object directly and have names that start with `EventInfoAuxDyn.weight_*`. A distinction between the two cases is necessary on a technical level and is achieved by specifying a `!EventInfoDecoration:` in front of weights stored as decorations. See e.g. the file `auxData/systematicVariations/common/top-variations.txt`. Variations in `EventInfoAuxDyn.mcEventWeights[]` can be accessed with the `PMGTruthToolWeight` workaround suggested [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgEventWeights#Interim_solution_for_use_in_case). Please, specify the list of DSIDs you want to access truth weights for with the tag `accessTruthWeightsForDSIDs`. Weightnames with special characters (like equality signs, comma, ...) need to be placed in quotes `""`. 
    
