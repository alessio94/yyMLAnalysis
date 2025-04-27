Joblists
=========================

Joblist files are used to instruct the submit script how to split your jobs on a batch system.


Usage for analyze.py
--------------------

A joblist file for analyse.py is simply a text file with a list of paths

    /bkg/?/Bkg1
    /bkg/?/Bkg2
    /sig/?/Higgs
    /data/?/2017

Please note that if you use pooling with the `.aj.pool.histograms` tag
or similar, you probably want to make sure that your job splitting
does not have a finer granularity than the placement of the pooling
tags in your sample folder - otherwise, you might end up in confusing
situations where you are unsure if a certain histogram was merged
correctly when collecting the results from the batch system.


Advanced Features
--------------------

You can put wildcards like `?` to instruct the code to process all
parts of the respective path together. This is very useful in case you
have a certain hierarchy level that corresponds to 'channels' that are
saved in the same `TTree`, allowing you to process all the channels
from that file in the same job. This is especially useful when you are
using additional features intended to increase the performance like
the `TQMultiChannelAnalysisVisitor`, which is able to process multiple
channels in parallel.