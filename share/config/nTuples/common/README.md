nTuples
=========================

You can also dump mini nTuples from any sort of input, be it xAODs or
other nTuples.

Usage
--------------------

The syntax of these files is similar for those for event lists and histograms, namely, you can write 

    ntup: int runNumber << EventInfo.runNumber(), int eventNumber << EventInfo.eventNumber(), float mjj << $(Mjj). , float dyjj << $(DYjj);

 to define a set of ntuples named `ntup` with branches `runNumber` and
 `eventNumber` of integer type and branches `mjj` and `dyjj` of
 `float` type, which are filled according to your definition.

You can then book these nTuples at one or more cut stages with a line like

    @Cut_2jet: ntup >> data/ntup/$(name).root:HWWTree_$(channel);

here, the nTuple set named `ntup` is filled at the cut stage
`Cut_2jet` and saved in the directory `data/ntup/$(name).root`, where
`$(name)` will be replaced by the name of the individual sample
(dataset id) such that you will obtain a fully-featured and
fully-split ntuple directory for further processing (assuming such a
tag with key `name` is present on the respective `TQSample`). The tree
written will be named `HWWTree_$(channel)` in this example, where
`$(channel)` will be replaced by your channel name as indiciated by
the tags in your sample folder hierarchy.

By default, an additional branch named weight will be created for you
which contains the fully computed and correctly normalized weight of
each event at the cut where the ntuples are filled.

In the analyze configuration a path has to be provided for the ntuple configuration and printing of the ntuples has to be switched on:

   ntuples: config/nTuples/Offshell/ml-ntuple-1j.txt
   printNTuples: true

The actual ntuples will be either dumped into your $CAFOUTPUTDIR/ntup (if you use ntup >> above) and in case you run locally, or if you submit on the batch the ntup directory will appear at the $PWD


Currently supported data types for branches are `int`, `float`,
`double`, `unsigned long long` and `vector<T>` , where `T` is one of
the other supported (scalar) data types. In order to create a vector
branch a vector observable needs to be assigned to it, it's number of
possible evaluations can be arbitrary and different for each event. If
vector observables are assigned to scalar type branches one entry in
the output tree is created for each evaluation of the vector
observable. In this case all vector observables assigned to scalar
branches are enforced to have the a consistent number of evaluations
(if for some event one of these vector observables has a different
number of evaluations than the others a runtime error is
thrown). Scalar observables are internally 'expanded' to match the
length of vector observables assigned to scalar branches. (For the
advanced reader: in this context vector observables assigned to vector
branches are treated like scalar observables, i.e., the corresponding
vector might be written multiple times.)

Tips & Tricks
--------------------

Dumping nTuples on a batch system requires some more thought as many
processes will typically attempt to write out nTuples at the same
time. Hence, each job should use a unique output file name for its
nTuples. 
