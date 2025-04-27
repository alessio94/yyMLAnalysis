Histograms
=========================

Histograms can be defined and booked in one or more config files.

Usage
--------------------

In order to define histograms, you can use lines like the following.

This is a one-dimensional histogram.

    TH1F('Mjj', '', 40, 0., 800.) << (Mjj/1000. : '\#it{m}_{jj} [GeV]'); 

 The value "Mjj/1000" is filled into the histogram, and the label of
the x-axis will be '\#it{m}_{jj} [GeV]'. Please note that the
'#'-symbol is escaped by a backslash. If the backslash is omitted, the
parser will interpret anything following the '#' as a comment and
ignore it, resulting in a syntax error while parsing this histogram.

This is a one-dimensional histogram with variable binning.

    TH1F('Mjj2', '', {0,200,600,800,1200,2000}) << (Mjj/1000. : '\#it{m}_{jj} [GeV]'); A

This is a two-dimensional histogram with labels for the x- and y-axis.

    TH2F('Mjj_DYjj', '', 40, 0., 800., 30, 0., 5.) << (Mjj/1000. : '\#it{m}_{jj} [GeV]', DYjj : '\#it{\#Delta Y}_{jj}'); 

Please note that the parser will also understand definitions of `TH[1/2/3][D/F]` as well as `TProfile` and `TProfile2D`. 
For each axis another triplet `..., nBins, lowerLimit, upperLimit` or a set of bin boundaries, e.g., `... , {0., 1., 4., 9.}`
needs to be added in the `THxF(...)` part.

Once you have defined your histograms, you can book them at one or several cut stages.
With this line, two of the above histograms will be attached to the Cut named "Cut_2jetincl" and all descendant cuts. 

    @Cut_2jetincl/*: Mjj_DYjj,Mjj2; 
    
In order to attach histograms only to one or a few cuts one can simply use a comma separated list:

    @Cut_2jetincl,Cut_2jetexcl: Mjj_DYjj,Mjj2; 

You can have as many lines defining and booking histograms as you
want (and your memory limitations permit), and they can be in any order. However, every histogram
definition must have a unique name.


Additional Features
--------------------

Additional option for individual histograms can be specified as in the
following example:

    TH1F('Mjj', '', 40, 0., 800.) << (Mjj/1000. : '\#it{m}_{jj} [GeV]') << (fillRaw=true, someOtherOption="someSetting"); 

The content between the last pair of parentheses is read as a list of
tags (see `TQTaggable`:importTags`). Supported options are then
applied to all booking instances (i.e. for all different cuts) of the
particular histogram. Currently supported options include:

 * Ignore event weights when filling this histogram/profile with the option `fillRaw=true`.

 * You can steer the behavior of vector-type observables with the
   option `fillSynchronized=true`, used for profiles and
   multidimensional histograms, i.e., everything but `TH1x`. When
   using vector type observables (observables with possibly more than
   one evaluation per event), by default, all combinations of values
   provided by the observables corresponding to the individual axes
   are filled. By specifying this option the behavior is changed such
   that only combinations of values are filled where their indices are
   equal. This then also enforces all non-scalar observables for the
   histogram/profile to have the same number of evaluations (same
   'length' of the vector/list of values they represent). Scalar type
   observables are implicitly expanded to match the size of the vector
   observables as if it would return the same value for every index.

 * You can apply specific weights when filling individual histograms
   with the option `weightExpression="myFancyWeight"`. This is similar
   to weight expression used in a cut but only applied to the
   particular histogram. If `myFancyWeight` corresponds to a vector
   type observable, its number of evaluations must be equal to the
   number of evaluations of the regular observables used for this
   histogram/profile. For multidimensional histograms and profiles the
   use of a vector valued weight is only supported in combination with
   `fillSynchronized=true` and enforces the weight observable to be of
   equal length as the regular (non-scalar) observables.

