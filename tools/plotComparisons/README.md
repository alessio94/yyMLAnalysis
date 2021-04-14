# Comparing distributions for different processes, channels or at different cut stages

The `plotComparison.py` script is a convenient tool to produce fast but nice looking comparison plots. It uses the general concepts of patches and tags in CAF including a config file for steering this script.
A new plot comparison can be done with the following:
   - Adapt the configuration file 'plotComparisons.cfg' (or write a new one and add its path with the `--config` argument) to your needs
   - Adapt the style file 'style-plotComparisons.txt' (or write a new one and specify it in your .cfg file) to your needs
   - Run this script with:
       ```python plotComparisons.py <plotCategories>```
       where `<plotCategories>` specifies the a list of sections you want to produce plots for and available in your config file.

## Configuration options
In the configuration file, each section corresponds to one comparison plot that can be created. You can also specify options in a `[common]` section that will be accounted in all other sections (with possibility of overwriting options in the individual sections). The following options/tags are available:

#### Main options
* `input`: path to sample folder with inputs
* `patches`: path to patch file to be used to specify some style settings
* `compare`: mode of comparison, can be:
  * 'processes' for comparing distributions at fixed cut for different processes
  * 'cuts' for comparing distributions at different cuts for fixed processes
  * 'variables' for comparing different distributions/variables at fixed cut and for fixed process
* `cuts`: list of cuts at which plots should be produced
* `observables`: list of observables/variables to be plotted
* `samplePaths`: list of paths to samples to be plotted
* `basePath`: prepended to samplePaths in case of common base paths
* `outputFolder`: output folder where plots should be saved

#### Special plots/options
* `drawRatio`: bool whether or not ratio should be drawn
* `setLegNameToTrailingFolder`: bool whether or not to set the legend names according to the trailing folder specified in samplePaths
* `specialPlot.StatUncertainties`: bool whether or not the statistical uncertainty should be plotted as a histogram instead of the nominal yields
* `specialPlot.StatUncertaintyDataProcess`: If set to a sample path (e.g. data/em/c16a) the square root of this nominal yield is plotted as an additional histogram
* `setDistinguishableLineColors`: bool whether or not to use a set of predefined colors instead of the colors specified in the style file

#### Style options
* `rebin.VARNAME`: integer to enable rebinning of histogram VARNAME
* `xrange.VARNAME`: 2 comma separated floats setting a custom x-axis range for histogram VARNAME
* `yrange.VARNAME`: 2 comma separated floats setting a custom y-axis range for histogram VARNAME
* `xlabel.VARNAME`: x-axis label for histogram VARNAME
* `ylabel.VARNAME`: y-axis label for histogram VARNAME
* `normed`: bool whether or not to normalized distributions
* `ratio.indexOfNumerator`: integer which plot in the list should be taken as the numerator
* `ratio.yrange`: 2 comma separated floats setting the custom y-axis range for the ratio plots
* `ratio.ylabel`: y-axis label for the ratio plot
* `label`: possible text that appears below the ATLAS label
* `plotFormat`: format of plots (pdf, png, eps)
* `plotOptions`: string specifying the option given to TH1::Draw()
* `logX`: bool whether or not to plot the x-axis logarithmically 
* `logY`: bool whether or not to plot the y-axis logarithmically
* `legend.textSize`: text size of legend (default = 0.04)
* `geometry.legend.xlow`: lower x-position of legend (default = 0.52)
* `geometry.legend.xhigh`: upper x-position of legend (default = 0.85)
* `geometry.legend.ylow`: lower y-position of legend (default 0.7)
* `geometry.legend.yhigh`: upper y-position of legend (adjusted to top margin per default)


