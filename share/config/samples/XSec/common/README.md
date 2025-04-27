Cross-Section Files
==========================

Cross-section files list sample-specific information needed to determine the
overall normalization: cross sections, k-factors, filter efficiencies.

Usage
--------------------------
Cross-section files are tables in TSV (Tabulator Separated Values) or (recommended)
CSV (Comma Separated Values) format
The first line of the file defines the mapping of each columns. The first column
is always taken as the sample name (DSID), the other columns according to their
'title' in the first line. The default matching patterns for the column titles
are
* cross section: ```\*xsec\*```
* filter efficiency: ```\*filt\*eff\*```
* k-factor: ```\*k\*fac\*```
* process info string: ```\*proc\*``` (\*)
* generator info string: ```\*gen\*``` (\*)
* simulation info string: ```\*sim\*``` (\*)

The recognized column types indicated by (\*) have no effect on the analysis but
merly allow to drag along some additional information that is also stored in the
SampleFolder.

Additionally, if the column title of the cross-section column contains any of the
units ```mb,µb,nb,pb,fb,ab``` the values given in this column are assumed to be
given in the respective unit. The default unit is ```pb```.
