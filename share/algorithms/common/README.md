Algorithms
====================

An algorithm is an object (instance of a classs derived from TQAlgorithm) that
performs some pre- and/or post processing of events. In this context 'pre-' 
refers to execution before the first cut (instance of TQCut) is evaluated for
the current event; 'post-' refers to execution after the entire cut hierarchy 
(and thereby all AnalysisJobs like histogram filling, cutflow counting,...) 
have been evaluated.

An example use case of algorithms is to perform (non-trivial) event-wise 
computations and then store the results in the TStore for easier use, e.g., in 
Observables during the main part of the event evaulation.


Booking an Algorithm
--------------------------
To execute an algorithm during the evaluation of samples it needs to be 
announced to the framework ('booked') at runtime. To this end, a small python 
snippet (in the following example: 'myAlgorithm.py') can be used. This snippet
in turn needs to be listed in the master config of the 'analyze' step:
```
# -*- mode: config -*-
[analyze]
algorithms:myAlgorithm.py
```
To book multiple algorithms, use a comma-seperated list.

An example for the content of such a snippet can be found in the following:
```
import QFramework
import ROOT

def addAlgorithms(visitor,config): # this function will be executed when listing
                                   # the containing snippet in the master config
                                   # as indicated above
  # booking of your Algorithm, here the CxAODReader Algorithm
  # already contained in CAFCore/CxAODUtils

  # creating an instance of the algorithm
  cxaodreader = CxAODReaderAlgorithm(False,False)
  # [optional] call arbitrary methods to configure your algorithm. 
  # These should only be global setting, i.e., ones that are not specific to 
  # a samples or channel 
  cxaodreader.select(CxAODSelectors.IntAccessor("isVVLooseSignalElectron"),
                     CxAODSelectors.IntAccessor("isVVLooseMuon"),
                     CxAODSelectors.PassThrough())
  #add the algorithm to the list of (active) algorithms
  visitor.addAlgorithm(cxaodreader)
  return True
```

Writing an Algorithm
--------------------------
A custom algorithm is implemented by writing a class derived from the (abstract)
base class TQAlgorithm. This derived class need to overwrite the following 
methods:
```
  #these are called once per sample (=input file):
  virtual bool initialize(const TQSample* sample) override; #prepare for processing the next sample
  virtual bool finalize() override; #tasks after all events of a sample are processed
  #these are called once per event:
  virtual bool execute() override; #pre-event (before cuts)
  virtual bool cleanup() override; #post-event (after cuts,...)
```
If the TQMultiChannelAnalysisSampleVisitor is used (as opposed to the 
TQAnalysisSampleVisitor) the algorithm will then be automatically cloned (using
ROOTs streamer technology) for each channel to be processed. The ```initialize```
method of each so created instance is then called with the ```TQSample``` of 
the corresponding channel.

If the algorithm should not be cloned but instead be treated as a 'singleton'
one needs to override the ```isSingleton``` and ```initializeSingleton``` 
methods as follows (instead of overriding ```initialize```):
```
#mark this algorithm as a singleton, i.e., don't clone for each channel
inline virtual bool isSingleton() const override {return true;} 
#initialization method for signletons
virtual bool initializeSingleton(const std::vector<TQSample*>& samples) override;
```


Additional Remarks
--------------------------
- Algorithms (as opposed to Observables, AnalysisJobs and Cuts) are initialized 
  even on samples which an empty/no TTree (no CollectionTree for xAODs). This 
  allows to extract bookkeeping information, e.g., when merging multiple samples.


