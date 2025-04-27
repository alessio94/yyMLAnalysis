Observables
=========================

An observable is an object that will redirect some string (for
example, a name or expression given in a cut definition file or via a
histogram definition) to some class that is able to evaluate it and
retrieve the corresponding value from the your input tree.


Preface
--------------------

In the end, all data-processing classes in the CAF have to somehow
extract their data from the tree. This data extraction is handled by
the `TQObservable` class - always. Independently of whether you are
applying a `TQCut`, or if you want to fill a histogram with a
`TQHistoMakerAnalysisJob`, or if you are making an event list with a
`TQEventlistAnalysisJob`, or if you are using a `TQMVA` - the numerical
values of variables will always be retrieved via an instance of
`TQObservable`.

All instances of `TQObservable` are handled globally by static functions
of the `TQObservable` class. However, the `TQObservable` class itself is
merely an interface, from which other classes can inherit, most
prominently the `TQTreeFormulaObservable`. When you specify an
arithmetic expression using some branch names in a cut definition
file, in a histogram definition file, in an event list definition file
or somewhere in the code, ultimately, `TQObservable::getObservable` will
be called on your expression and check if another observable with the
same expression already exists - in which case it will return a
pointer to that one - or if it needs to instantiate a new
`TQObservable`. In the latter case, it will usually instantiate a
`TQTreeFormulaObservable`, which internerally uses a `TTreeFormula` to
retrieve its data from the tree, but there are other observable
classes as well.

Please note the distinction between the abstract `TQObservable` class,
which only provides an interface for observables along with same basic
helper functions without specifying any actual method of data
retrieval, and the more specialized classes `TQTreeObservable`, which
relies on the presence of a TTree, as well as the added
`TQEventObservable`, which uses the `xAOD::TEvent` class and can only be
used in conjuction with xAODs. The default observable type is the
`TQTreeFormulaObservable`, a specialized version of `TQTreeObservable`
which employs an instance of `TTreeFormula` for data retrieval.

Types of observables
--------------------

The default type of observable that is used by the framework most of
the time is the `TQTreeFormulaObservable`. If you have ever made a
typo in one of your definition files, you probably already encountered
error messages from this class.

For example, if you provide an expression that contains no branch
names and only constant numerical expressions, the framework will
decide to use a `TQConstObservable` instead, which will only be
evaluated once for the entire sample and then simply return the cached
value for each event for performance reasons.

Also other, more fance types of `TQObservable` exist:

   * `TQMVAObservable`, which will be constructed for you if you
     provide an expression that looks like this:
     MVA:/path/to/my_mva.xml. The observable will then automatically
     parse the XML file you provided and construct a TMVA::Reader from
     it, which will then be evaluated on each individual event it is
     called on. This allows you to calculate MVA outputs on the fly
     and use them in your cut-based analysis, to either cut on them or
     fill them into histograms, event lists, or anything else (like
     other MVAs).

   * `TQMultiObservable` is a sort of meta-observable that will act
     upon other observables and combine their respective
     output. Creation of such an observable can be triggered by
     prepending Multi: to any expression, or by using brackets [...]
     in such a way that they enclose an unquoted string. Each segment
     enclosed in such brackets will be interpreted as the name of some
     other observable, which will be evaluated and inserted in this
     expression at runtime. Please not that `TQMultiObservable` does not
     provide any direct data access, which means that you shall not
     use any branch names in this expression directly, but only other
     observable names.

   * `TQEventIndexObservable` is a quite specialized type of
     observable that only returns the index of the current event in
     the tree. In case you are wondering how this might come in
     useful, you might want to study the section on MVA integration.

   * `TQCPDecorationObservable` is a `xAOD::TEvent` based observable
     (inheriting from `TQEventObservable`) that is capable of reading
     auxdata members of `xAOD::CompositeParticle` and or any part of
     such. You may use these observables with expressions like
     `CPFD:ContainerName[0].part(1).part(2).part(3):memberName`, where
     you may go to arbitrary depth in dereferencing levels by calling
     parts - also zero occurences are allowed, allowed to directly
     read decorations of the composite particle in the container.

   * `TQHistogramObservable` is an observable intended for using
     binned weights from histograms. It can be used with `TH1`, `TH2` and
     `TH3` histograms. It provides the bin content of the histogram
     based on the values of other observables. The number of other
     observables is equal to the dimension of the histogram, i.e. each
     axis of the histogram corresponds to one observable. It can be
     used with expressions like
     `TH1Map:myFile.root:myHist([observable1])` or
     `TH3Map:myFile.root:myHist([observable1],[observable2],[observable3])`. This
     loads a histogram named `myHist` from a file 'myFile.root' (it is
     technically reloaded for every sample, so sample dependent
     expressions using the usual `$(myTag)` syntax are possible) and
     maps the value(s) returned by the observable(s) `observable1`
     (`observable2` and `observable3`) to the corresponding bin
     content obtained from the histogram 'myHist'. The observables are
     assigned to the x-,y- and z-axis in the order they are
     specified. It is up to the user to ensure that this axis
     assignment is correct. The user should also make sure that the
     units of the axes and the corresponding observables match. The
     bins are found using `TH1::FindBin`. Please remember to pay
     attention to under/overflow bins! If these are not set, zero will
     be returned in case one of the sub-observables returns a value,
     which is outside of the histogram range.


   * `TQFilterObservable` is an observable intended for rejecting
     certain values returned by one observable based on the response
     of a second observable. It can be used with expressions like
     `Filter(firstObservable,secondObservable)`. The `TQFilterObservable`
     is a vector observable, i.e., it can return zero or more
     values. A value from the first ('value') observable is suppressed
     if the second ('cut') observable evaluates to zero (think of the
     boolean 'false'). It supports three configurations of (value,
     cut) observables: (scalar, scalar), (vector,scalar), and
     (vector,vector). The first case was essentially just explained. A
     fairly straight forward generalization is given by the second
     case where either the entire value vector passes or no value
     passes. A particularity is given by the third case (vector,
     vector): This variant enforces the number of evaluations of the
     two observables to be equal, each entry of the value vector is
     forwarded if the corresponding entry of the cut vector evaluates
     to non-zero.

   * `TQVectorAuxObservable` is an observable intended for reducing
     vector type observables to single, 'scalar' quantities through a
     multitude of available operations. It is invoked by prefixing a
     vector type observable expression (enclosed in parentheses) with
     the keyword corresponding to the operation to be applied, e.g.:
     `VecAND(myVectorObservable)` In this example the
     `TQVectorAuxObservable` returns `1` if all evaluations of
     myVectorObservable are non-zero (and zero otherwise). Available
     prefixes are (each to be followed by a colon character ':') :
     `VecAND`, `VecOR`, `VecSUMABS`, `VecSUM`, `VecAVG`, `VecLEN`,
     `VecMIN`, `VecMAX`, `VecMINABS`, `VecMAXABS`, `VecNORM`,
     `VecPROD`, `VecNTRUE`, `VecNFALSE` . In addition there is the
     prefix `VecAT` for which the full syntax reads
     `VecAt(myVectorObs,myIndex)`. This will only return the entry of
     the vector observable at the index given by `myIndex` (please
     note that the can itself be a (scalar) observable expression and
     the returned value will simply be cast to an integer internally)

For users of vector observables (you can likely skip this on your
first read), please take note of the behavior when combining multiple
observables (e.g. via TQMultiObservable or TQHistogramObservable in
the 2/3D histogram case). Unless an observable states otherwise
vectors are combined element wise. This means that the combining
observable combines its subobservables element wise. As an example,
please consider the product of two vector observables via the
TQMultiObservable with an expression `[vec1] * [vec2]` which will
behave as a vector observable with the same length as the
subobservables and values obtained as the element wise product of the
values of `vec1` and `vec2`. As a consequence, it is enforced that the
length of vector observables combined in such a way are equal (an
error is raised otherwise, i.e., your analysis will crash). Scalar
observables are implicitly expanded to match the length of the vector
observables used alongside. (This behavior was chosen with
object-related studies in mind; to calculate the standard scalar
product of two vector observables one should use an expression like
`VecSUM( [vec1] * [vec2] )`.)

Writing your own observable and using it
--------------------

All of this magic works because the observables are simply identified
by the expressions you provide. After an observable is constructed,
the framework does not care anymore what type of `TQObservable` it
is handling. This especially allows anybody using the framework to
implement custom observables in regular `C++` and inject them into the
framework. To achieve, all you need to do is the following:

  *  Implement a new class that inherits from TQObservable. Don't forget to overwrite all the virtual functions you need, that is,
      *  `double getValue() const` Return the value of your observable. May contain any code that interacts with the tree fTree or the sample fSample, or any other members your class has.
      *  `double initializeSelf()` Initialize your class. Do anything that needs to be done once you get a new sample and tree. When this function is called, your sample and tree have already been setup for you.
      *  `double finalizeSelf()` The counterpart of `initializeSelf()`. The tree and sample will be finalized later. If you have allocated any memory inside `initializeSelf()`, for example by using the keyword new, now is the time to free it and delete your object.
      *  `TObjArray* getBranches()` This function is supposed to return a TObjArray containing a list of `TObjStrings` that correspond to the branches that your observable requires to be enabled in the tree. If your observable is based on an arithmetic expression, it's probably sufficient to call `TQUtils::getBranchNames()` on that expression. If you have any hard-coded branches, you should add their names to the list. 

Once your class is implemented and compiles fine along with the other
observable classes, you can write a small python script that
instantiates your observable class and adds it to the observable
database. This can look like this:
```python
    from QFramework import TQObservable, INFO
    import ROOT
    from CAFExample import MyNewFancyObservableClass
    
    def addObservables():
      myObs = MyNewFancyObservableClass ( ... )
      if not TQObservable.addObservable(myObs,"FancyObs"):
        INFO("failed to add FancyObs Observable!")
        return False
    return True
```
Then, all you need to do is list the path to your script file under
the config option addObservables to have the framework execute your
code and add your observable to the database.

In this example, your observable is known to the framework under the
name of `FancyObs`. This especially means that you can use `FancyObs`
everywhere you want to refer to your observable. For example, you can
fill it into a histogram. However, if you want to use it as a cut
value, some additional precautions are required: Usually, any
arithmetic expression like `FancyObs > 1` will be handed over to a
`TTreeFormula`, which will not understand the name of your variable
and complain about a missing branch with this name. The solution to
this dilemma is the `TQMultiObservable`, which was designed with
precisely this use case in mind and provides a way to construct
arithmetic expressions of observable names (rather than branch
names). In order to employ this magic, all you need to do is write
`Multi: [FancyObs] > 0`. Please note the keyword `Multi`, which will
let `TQTreeObservable::getObservable` know that you are planning to
construct a `TQMultiObservable`, as well as the brackets `[ ... ]`
around the name of your observable, which will let `TQMultiObservable`
know that it should replace this part of the expression with your
observable.
