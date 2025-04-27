Cuts (visualization)
=========================

These files are used to control which cuts are considered when creating
cutflow tables. Each line of these files essentially corresponds to one
row in the resulting cutflow tables.

Usage
--------------------

Each line in such a file lists one cut that should appear in the cutflow
table(s). The order in which the cut names are listed in this file is also
the order in which they appear in the cutflow tables. A typical line reads:

.name="CutChannels", .title="Channel Selection"

The '.title' part is optional and can be used to have a different title
shown in the first column of the cutflow table. If this is not specified here
it will default to the '.title' tag set in the cut definition file during the
analyze.py step.

In order to make the tables more readable horizontal lines can be inserted 
using

.name="|"

for a single line or 

.name="||"

for a double line.

Advanced Features
--------------------

In addition to the .name and the optional .title the option '.sfPolicy' can
be used to control if applied NormalizationFactors (from NF calculators) are
shown in a line above the actual cutflow entry. Possible values are 0 (never),
1 (only if different from 1.0) and 2 (always).

The CutflowPrinter supports a set of special "cuts". Appart from the special
'.name' cases listed above a "ratio" entry is supported. The '.name' option
in this case is repalced by '.special'. For example:

.special="ratio", .numerator="someCut", .denominator="otherCut"

if '.numerator' and '.denominator' are not specified the code falls back to 
using the *process* options '.path1' (->numerator) and '.path2' (->denominator).



Tips & Tricks
--------------------

Lines starting with a '#' are considered as comments and simply ignored.


