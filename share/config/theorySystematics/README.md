Theoretical uncertainties
=========================

Theoretical uncertainties configs are used to provide the `theorysystematics.py` script with the information on which process-specific sources of uncertainties exist and how to evaluate them.


Usage for theorysystematics.py
--------------------

Please specify the config file when running (from `share/`), i.e.:
```bash
python ../tools/TheorySystematics/theorysystematics.py config/theorySystematics/config/VBF/ttbar-SM-VBF.cfg
```   
See `theorysystematics.py --help` for other arguments.

Explanation of common config items
--------------------

`process`: name of the physics progress (e.g. ggF)
`base`: indicate whether process' sampleFolder is signal (e.g. ggF => /sig/) or background (e.g. top => /bkg/)
`channels`:  added together (e.g. em,me => [em+me]).
`campaigns`: added together (e.g. c16a,c16d => [c16a+c16d]).
`sample`: nominal sample folder path after campaigns (e.g. /ggf/).
`actOnSample`: tag so assign process to the process-name used in fit configurations, e.g. Sample.ttbar\*jet for ggF ttbar processes.
`cuts`: filepath(s) to config specifying evaluated cut regions (see `data/cuts/README.md`).
`contolRegions`: name(s) of control region from which to evaluate extrapolation uncertainty in evaluated cut region, one for all cuts or each cut config.
`observables`: filepath(s) to config specifying evaluated observables, one for all cuts or each cut config (see `data/observables/README.md`).
`systematics`: list of systematics (specified furter by section) to evaluate.

Explanation of systematic config items
--------------------

`name`:  			name of the systematic to be called in SFramework.  
`folder`:     folder at which the variation is being applied (options: channel or sample).  
`nominal`:    name of nominal (default: empty).  
`variations`: list of variation names (e.g. alternate weights or samples).  
`mode`:       systematic uncertainty computing logic (options: single, updown, envelope, stddev, symhess, alphas)  
`symmetrize`: optional tag specifying whether uncertainty should be symmetrized, if applicable (e.g. up/down).  
`symmetrizeLarger`: optional tag specifying whether uncertainty should be symmetrized by choosing the larger variation (by norm difference from nominal) and inverting it (to be used e.g. for truth up/down variations).  

Making uncertainty tables
--------------------
Usage:
```bash
python ../tools/TheorySystematics/makeUncTables.py --input path/to/your/unc/file.txt --signalregions mySR_0jet,mySR_1jet
```
The `makeUncTables.py` script can be used to visualize the uncertainties in tables. Please specify the path to the theory systematic txt file with the `--input` argument. To produce tables for extrapolation uncertainties, please specify the SRs with the command line argument `--signalregions` or to point to the files Lists use `--signalregionsLists`. The table can be inverted with `--invertTable`. For further information please see `makeUncTables.py --help`.

Producing shape uncertainty plots
--------------------
Usage:
```bash
python ../tools/TheorySystematics/makeUncShapePlots.py --input path/to/your/unc/file.txt 
```
The `makeUncShapePlots.py` can be used to plot the shapes (nominal vs variation) of the histograms that were saved in the output txt files of the `theorysystematics.py` script. Please specify the path to the txt file with the `--input` argument. This script uses the TQHWWPlotter tags for which can be set in a separate config file. A default config file `plot-default.cfg` is provided; you can point to your own config with the `--config` tag and can define process specific tags inside a config file added with `--specificPlottingConfig`, see e.g. `plot-processes.cfg`. For further information please see `makeUncShapePlots.py --help`.

Note: There is currently another way to produce plots, via `--plots` flag in running `theorysystematics.py` script itself. This is useful for "debugging" plots of /individiual/ variations, rather than the final uncertainty (in which case the above plotting will be simpler and more insightful). Very much a work-in-progress to implement fine-tuneable options and functionality across all methods.
