Processes (visualization)
=========================

These files are used to control which processes are considered when creating
cutflow tables and making plots. Each line of these files essentially 
corresponds to one column in the resulting cutflow tables and/or one legend
entry in plots.

Usage
--------------------

Each line in such a file defines one process that should appear in the cutflow
table(s) and plots. The order in which the processes are listed in this file
is also the order in which they appear in the cutflow tables (the order in 
which histogram contributions are stacked in plots is subject to some other
settings). Typical lines read:

.path="sig/$(channel)/$(campaign)/ggf", .name="ggf", .title="gluon-fusion H", .isSignal=true
.path="bkg/$(channel)/$(campaign)/Zjets", .name = "Zjets", .title="Z/DY", .isBackground=true

The '$(channel)' and '$(campaign)' placeholders will be replaced by segments defined for the 
respective channel and campaign, e.g., 'ee', 'mm' or '\[ee+mm\]' and 'c16a', 'c16d', 'c16e' 
or \[c16a+c16d\]. See e.g. the ['config/channels'](../../channels/README.md) README.

The various options that can be used here include:
* '.path': sets the path inside the SampleFolder that should be used for this
    process. As usual, summation over all subSampleFolders is implied.
* '.name': identifier of the process. The value should be unique within one 
    processes file. For plots (via TQPlotter/TQHWWPlotter) the internal histogram
    name will be prefixed with "hist\_", e.g., .name=Zjets -> hist\_Zjets .
* '.title': visible title in the first row of cutflow tables / plot legends.
    This defaults to the 'label' tag set on the (sub)SampleFolder corresponding
    to location given as '.path'.
* '.isSignal','.isBackground','.isData': controls various defaults for how the 
    process is displayed in plots, e.g., by default, backgrounds are stacked,
    the signal overlayed and data shown as individual points.
* 'color', 'histFillColor', 'histFillStyle', 'histLineColor', 'histLineWidth', 
    'histMarkerColor', 'histMarkerSize', 'histMarkerStyle': Explicitly sets various
    visualization settings for the process. Please note that this is often not
    needed as by default these settings are already stored in and read from the 
    SampleFolder.
* 'stack': Explicitly control if this process is added to the histogram stack
    Default depends on type of process (signal/bkg/data)
* 'drawOptions': options passed to TH1::Draw, for data defaults to "ep" ("same"
    is added internally). 

In order to make the tables more readable vertical lines can be inserted 
using

.path="|"

for a single line or 

.path="||"

for a double line. This will only be used for cutflow tables and ignored for
plots.

Advanced Features
--------------------

For cutflow tables special "processes" can be defined:
* '.special="ratio", .path1="path1", .path2="path2"' :
    Instead of showing yields of a certain SampleFolder path shows the ratio of
    yields of the two given paths (path1/path2). Example:
    '.name="DoverB", .title="data/total bkg", .special="ratio", .path1="data/$(channel)", .path2="bkg/$(channel)"'
* '.special="significance", .path1="path1", .path2="path2"' :
    Similar to the 'ratio' case  but shows the poisson significance of the 
    signal (path1) over the background (path2):
    sqrt\[ 2 x { (s+b) x log(1+s/b) - s  } \]
* '.special="pull", .path1="path1", .path2="path2"':
    Shows the pull between the yields of path1 and path2:
    (p1-p2)/sqrt( sp1 x sp1 + sp2 x sp2)
    where p1,p2 are the yields corresponding to path1/2 and sp1/2 their stat.
    uncertainties.
* '.special="acceptance", .denominator="someCut", .path="sig/$(channel)" :
    Shows the acceptance of each cut w.r.t. some other cut 'someCut' for the
    yields corresponding to the given SampleFolder path. Unless 
    '.isEfficiency=false' is explicitly set this will assume that the events
    in the numerator are a subset of the events in the denominator when 
    calculating the resulting statistical uncertainty (otherwise the two yields
    involved are assumed to be statistically independent).
    



Tips & Tricks
--------------------

The '.titles' option can make use of some basic ROOT style LaTeX. Please note
that the comment character '#' need to be escaped by a leading '\':
.path = "bkg/$(channel)/Vgamma", .name = "Vgamma", .title = "$V+\#gamma$", .isBackground = true


