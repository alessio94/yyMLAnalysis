This document is intended to provide hands-on instructions on how to write a custom observable for the [CAF tutorial](https://indico.cern.ch/event/1380637/). General reading on observables can be found in the tutorial and also in the observable [README](https://gitlab.cern.ch/atlas-caf/CAFExample/blob/master/share/common/observables).

Prerequesite for the tutorial is a working [CAF](https://gitlab.cern.ch/atlas-caf/CAFCore) installation. You can check this
by running

```bash
$ python -c "import QFramework"
RooFit v3.60 -- Developed by Wouter Verkerke and David Kirkby 
                Copyright (C) 2000-2013 NIKHEF, University of California & Stanford University
                All rights reserved, please read http://roofit.sourceforge.net/license.txt
```

# Basic steps to create a new observable 
There are four steps that need to be performed to introduce a new observable to an existing analysis:
1. Create and write the observable class including a source and header file. This is the main part of the observable creation where the actual calculation of the desired quantity needs to be implemented. In this hands-on exercise the helper script [wizard.py](https://gitlab.cern.ch/atlas-caf/CAFCore/blob/master/QFramework/share/TQObservable/wizard.py) is used for these purposes. In general it is also a good idea to look at already defined observable classes and start from those as a baseline to implement your own observable.
2. Create dedicated python snippet for instantiating the class and adding it to the observable database.
3. Tell your analysis where to find the python snippet.
4. Implement observable in analysis flow in terms of histograms, cuts, weights, etc.

# Creating the MjjMaxObservable
We want to create a new observable that calculates the invariant mass (called Mjj in the following) for every possible combination of two jets in an event and returns the maximum of those values. 
The new observable class is to be implemented in the existing [xAOD Example analysis](https://gitlab.cern.ch/atlas-caf/CAFExample/tree/master/share/xAOD).

## 1. Write c++ code with the magic wizard.py script
[CAFCore](https://gitlab.cern.ch/atlas-caf/CAFCore) provides the python script [wizard.py](https://gitlab.cern.ch/atlas-caf/CAFCore/blob/master/QFramework/share/TQObservable/wizard.py) helping you to create a source and header file for your new observable. Make sure you are setup (`source build/setupAnalysis.sh`) and are in the main directory of the CAFExample repository (which you can ensure e.g. with `cd $CAFANALYSISBASE`). Then, you can call the script via
```bash
./CAFCore/QFramework/share/TQObservable/wizard.py
```

The wizard will ask a couple of questions that we answer as follows (for more information about these questions, see at the end of this document).
```
Should the observable wizard put the files into your current working directory (leave empty) or into some package (type package name)? CAFExample
What is the name of the observable you would like to create? MjjMaxObservable
Please specify the type of observable, choose  from {Event,Tree,<empty>}: Event
Do you want to create a vector observable that can return multiple values? (y/N) N
Should your class have an 'expression' member variable? (y/N) N
Are you planning to provide a factory for your observable class? (y/N) N
```
After providing the answers the wizard will summarize the made choices and asks for confirmation to build the observable:

```
Your choices:
class name: MjjMaxObservable
inherits from: TQEventObservable
in package: CAFExample
written to directory: .
not including expression member
build this observable now? (y/N) y
wrote './CAFExample/MjjMaxObservable.h'!
wrote './Root/MjjMaxObservable.cxx'!
the wizard has detected a LinkDef.h file in the target package - do you want to add an entry for your observable class? (y/N) y
```
We can also directly add our new class to the `LinkDef.h` to ensure that the framework will pick up our new class (although this step might not be necessary, since the link definition file of CAFExample (and CAFCore) is created automatically).
Once we have confirmed that the wizard should build the observable, two new files are created: `CAFExample/MjjMaxObservable.h` and `Root/MjjMaxObservable.cxx`. The important parts in the files contain comments that guide you through the further process. There are example blocks given that provide very adequate pieces of code which we want to use.
Let's have a look at `MjjMaxObservable.h` first. You should modify it such that it looks like the following:

```c++
//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __MJJMAXOBSERVABLE__
#define __MJJMAXOBSERVABLE__
#include "CAFxAODUtils/TQEventObservable.h"

class MjjMaxObservable : public TQEventObservable {
protected:
  // Put here data members you wish to use.
  // For example you might want to add a variable
  // for the name of the xAOD container to be retrieved that
  // will be initialized in the initialize function.
  
  TString mContName = ""; 

public:
  virtual double getValue() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
public:
  MjjMaxObservable();
  MjjMaxObservable(const TString& name);
  virtual ~MjjMaxObservable();
  ClassDefOverride(MjjMaxObservable, 1);

};
#endif
```
Three main functions are already declared (`initializeSelf()`, `finalizeSelf()` and `getValue()`). Now, we will go in the source file of the observable and implement them.
In `initializeSelf()` we will initialize the container name `mContName` by retrieving the tag from the sample folder with the following lines of code. We put this piece of code in the `initializeSelf()` method, because it only needs to be initialized once per sample. This way, we avoid costly string parsing once per event:
```c++
TString ContName = "";
if(!this->fSample->getTagString("~cand",ContName)) return false;
this->mContName = "Event"+ContName;
```
Now we have to implement the actual calculation of the desired quantity. This has to be executed for every event, so we will put the code in the `getValue()` method. For our example the following lines should be included:
```c++
  // retrieve candidate
  const xAOD::CompositeParticleContainer *cand = 0;
  if(!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()){
    DEBUGclass("failed to retrieve candidates!");
    return false;
  }
  // after you have retrieved your data members, you can proceed to calculate the return value
  // probably, you first have to retrieve an element from the container
  const xAOD::CompositeParticle *Evt = cand->at(0);
  int nParts = Evt->nParts();
  double MjjMax = 0;
  for ( int i=2; i < nParts; ++i ){
    for ( int j=i+1; j < nParts; ++j ){
      double tmp_Mjj = Evt->p4(i,j).M();
      if (tmp_Mjj > MjjMax){
        MjjMax = tmp_Mjj;
      }
    }
  }

  const double retval = MjjMax;
  return retval;
```
After implementing the above, the observable should be ready to be compiled - almost. We use a CompositeParticleContainer, but your c++ doesn't know what this is. So let's include the corresponding header file
```c++
#include "xAODParticleEvent/CompositeParticleContainer.h"
```
in the beginning of the file. Now, we are ready to execute `cafcompile`  which will automatically run cmake for you, find the new class and compile it.
Once your class compiles fine along with the other observable classes we can move on to the next step.

Two small remarks:

*  The line at the top of `MjjMaxObservable.cxx` saying `// #define _DEBUG__` can be uncommented to enable printouts from the `DEBUGclass(...)` function. This might be useful for initial tests and checks of the new observable.
*  CAF provides the small functions `cafcompile` and `cafbuild`. The latter only compiles your code with `make`, while the former calls `cmake` and then compiles. To see what exactly these functions do, type `type cafcompile` in your shell.

## 2. Creating a python observable snippet
A small python snippet needs to be added for the new observable to the designated observable/ folder of the analysis.
In the xAOD Example analysis, the observable snippets are located [here](https://gitlab.cern.ch/atlas-caf/CAFExample/tree/master/share/xAOD/observables). (If you write an observable that is used by multiple analyses, you should think of creating the observable snippet in [common/](https://gitlab.cern.ch/atlas-caf/CAFExample/tree/master/share/common/observables).)
The snippet will instantiate the observable class and adds it to the observable database. The python script should have the same name as the observable itself (in our case MjjMaxObservable.py) and can look like this:

```python
from QFramework import TQObservable, INFO
from CAFExample import MjjMaxObservable

def addObservables():

    obs = MjjMaxObservable("MjjMax")
    if not TQObservable.addObservable(obs, "MjjMax"):
        INFO("failed to add MjjMax Observable")
        return False
    return True;
```
Here, calling the constructor of the new observable class with `MjjMaxObservable("MjjMax")` will set the name of the observable instance to `MjjMax`. This is the name that we will use later in the analysis to get the value of the observable.

## 3. Tell analysis about the python snippet
Next, we need to list the path to your script in the config file of the analyze step so that the framework can find the code and execute it. The relevant part you should add to [analyze-xAOD-Example.cfg](https://gitlab.cern.ch/atlas-caf/CAFExample/blob/master/share/xAOD/config/master/analyze-xAOD-Example.cfg) is:
```
customObservables.directories: xAOD/observables
customObservables.snippets: [...all other observables...], MjjMaxObservable
```

## 4. Defining histograms/cuts/...
The observable can now be used to define histograms, cuts, event lists, etc. Let's define a simple histogram with the MjjMax distribution.
We add a new histogram definition in the appropriate [histogram definition file](https://gitlab.cern.ch/atlas-caf/CAFExample/blob/master/share/xAOD/config/histograms/xAOD-Example-histograms.txt) and add it at the desired cut stages e.g.:
```
TH1F('hist_MjjMax', '', 50, 0., 500.) << ( [MjjMax]*0.001 : 'm_{jj}^{max} [GeV]');
@CutChannels/*: hist_MjjMax;

```
The first line defines a new histogram with a syntax reminiscent of a ROOT `TH1` constructor. In the second part of the line, we call our observable `[MjjMax]` with the name that we gave to the constructor in the python snippet. The square brackets indicate that this is an observable.
The second line books the histogram at the cut `CutChannels` and all subsequent cuts (indicated by `/*`). We could have given the histogram the same name as the observable, which probably makes things easier. But to understand what the different identifiers mean, we appended a `hist_` to the observable name.

## Bonus: Running the analysis and looking at newly booked histogram
If the analysis is executed (you only have to perform the analyze step) and everything was correctly implemented, the new histogram should appear in the output sample folder.
<!-- It is assumed that you have already learned how to run a complete analysis.-->
You can check this by opening the respective sample folder with `tqroot -sfr sampleFolders/analyzed/samples-analyzed-xAOD-Example.root` and draw one of the histograms with
```c++
r_samples->getHistogram("bkg/[ee+mm]/[c16a+c16d+c16e]/top/ttbar", "CutChannels/hist_MjjMax")->Draw("")
```
If you see a reasonable distribution: Congratulations! You just successfully created your own observable.
Now, you can do with it what ever you want (define cuts/cutflows, event lists, etc.) and/or create more awesome observable!

# Creating the PtLep1SquaredObs
We want to create a new observable which calculates the square of the leading electron's pT.
The new observable class is implemented in the existing [Easyjet Example analysis](https://gitlab.cern.ch/atlas-caf/CAFExample/tree/master/share/easyjet_example).
For brevity, we will skip some of the details from the xAOD example above.

## 1. Write c++ code with the magic wizard.py script
Call the script using:
```bash
./CAFCore/QFramework/share/TQObservable/wizard.py
```

Answer its questions like:
```
Should the observable wizard put the files into your current working directory (leave empty) or into some package (type package name)?
Type your choice: CAFExample
What is the name of the observable you would like to create? PtLep1SquaredObs
What type of observable would you like to create?
If you want to read xAODs using the xAOD::TEvent mechanism, please type 'Event'
If you want to access the TTree pointer and use TTreeFormula, please type 'Tree'
If your new observable does not need direct data access but uses some custom mechanism to work, please leave empty.
Please specify the type of observable, choose  from {Event,Tree,<empty>}: Tree
Do you want to create a vector observable that can return multiple values? (y/N) N
Some observable classes have an 'expression' member variable that allows to alter the configuration based on sample tags, but complicates identifying the right observable.
Should your class have an 'expression' member variable? (y/N) N
Are you planning to provide a factory for your observable class? (y/N) N
Your choices:
- Class name: PtLep1SquaredObs
- Inherits from: TQTreeObservable
- In package: CAFExample
- Write to directory: ./CAFExample/CAFExample
- Not including expression member
- Not configured for factory use
Build this observable now? (y/N) y
Wrote './CAFExample/CAFExample/PtLep1SquaredObs.h'!
Wrote './CAFExample/Root/PtLep1SquaredObs.cxx'!
```

Notably, we are creating a TTree observable now, for ntuples.

Modify the header file, `CAFExample/CAFExample/PtLep1SquaredObs.h`, like:

```c++
//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __PTLEP1SQUAREDOBS__
#define __PTLEP1SQUAREDOBS__
#include "QFramework/TQTreeObservable.h"
#include "TTreeFormula.h"
#include "TString.h"

class PtLep1SquaredObs : public TQTreeObservable {
protected:
  // Put here any data members your class might need and don't forget potential includes.
  TTreeFormula *fFormula = 0;
  TString mVariation = "";
 
public:
  virtual double getValue() const override;
  virtual TObjArray* getBranchNames() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
public:
  PtLep1SquaredObs();
  PtLep1SquaredObs(const TString& name);
  virtual ~PtLep1SquaredObs();
  ClassDefOverride(PtLep1SquaredObs, 1);

};
#endif
```

`fForumla` stores a `TTreeForumla` for evaluating an expression based on input branches, and `mVariation` stores a `TString` indicating the variation (nominally: `"NOSYS"`).

Also modify the source code, `CAFExample/Root/PtLep1SquaredObs.cxx`. Firstly, `initializeSelf()` should be updated like:
```c++
bool PtLep1SquaredObs::initializeSelf(){
  // initialize this observable
  // called once per sample (input file) so that the observable knows the name of the variation
  DEBUGclass("initializing");

  // Obtain the variation name
  TString variation = "";
  if (!this->fSample->getTagString("~Variation", variation)) {
    ERRORclass("Could not get tag string ~Variation.");
    return false;
  }
  this->mVariation = variation;

  // since this function is only called once per sample, we can
  // perform any checks that seem necessary
  if (!this->fTree){
    DEBUGclass("no tree, terminating");
    return false;
  }

  // create string expression that calculates pTlep1^2
  TString PtLep1Squared = "pow(el_pt_" + mVariation + "[0], 2)";
  DEBUGclass("Configured expression: %s", PtLep1Squared.Data());

  this->fFormula = new TTreeFormula("PtLep1Squared", PtLep1Squared.Data(), this->fTree);

  return true;
}
```

This extracts the variation using a tag set on the inputs and then builds an expression which squares the leading (i.e., index 0) electron from the branch, e.g., `"el_pt_NOSYS"`. We also load this branch in `getBranches()`:
```c++
TObjArray* PtLep1SquaredObs::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = new TObjArray();
  bnames->SetOwner(false);

  // add the branch names needed by your observable here, e.g.
  bnames->Add(new TObjString("el_pt_" + mVariation));
  
  return bnames;
}
```

Evaluate the expression in `getValue()`:
```c++
double PtLep1SquaredObs::getValue() const {
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!
  
  // here, you should calculate your return value
  // of course, you can use other data members of your observable at any time
  const double retval = this->fFormula->EvalInstance();

  DEBUGclass("returning");
  return retval;
}
```

Finally, clean up `fFormula` in `finalizeSelf()`:
```c++
bool PtLep1SquaredObs::finalizeSelf(){
  // finalize this observable
  // remember to undo anything you did in initializeSelf() !
  
  DEBUGclass("finalizing");

  delete this->fFormula;
  this->fFormula = 0;

  return true;
}
```

## 2. Creating a python observable snippet

Create a python snippet for the observable, `share/easyjet_example/observables/PtLep1SquaredObs.py`:
```python
import QFramework

from CAFExample import PtLep1SquaredObs

def addObservables():

  myObs = PtLep1SquaredObs("PtLep1Squared")
  if not QFramework.TQTreeObservable.addObservable(myObs):
    return False

  return True

if __name__ == "__main__":
  print("Inside main of PtLep1SquaredObs snippet, will now call addObservables")
  addObservables()
```

## 3. Tell analysis about the python snippet

We update [analyze-2LSC.cfg](hhttps://gitlab.cern.ch/atlas-caf/CAFExample/-/blob/master/share/easyjet_example/config/master/analyze-2LSC.cfg) like:
```
customObservables.directories: easyjet_example/observables/
customObservables.snippets: PtLep1SquaredObs
```

And it can then be used! An example of a "sanity check" comparing the observable's output to the same quantity obtained using aliases (in fact, the operations we perform using aliases rely on similar machinery, but this occurs behind-the-scenes in this case) can be found in the corresponding [cuts file](https://gitlab.cern.ch/atlas-caf/CAFExample/-/blob/master/share/easyjet_example/config/cuts/cuts-2LSC.def) at `+CutSanityCheck`. While we can use aliases to do the same thing (and much more simply), observables excel at encapsulating more complicated operations.

# More wizard options (Advanced)

Without talking about the specifics, we answered a few wizard questions with no. Let's see what these questions are about.

## Creating a custom vector observable
There is also the possibility to create observables that return multiple values per event. This can be useful for a bunch of things, especially in combination with `TQVectorAuxObservables` it will give you the opportunity to manipulate the output of the observable just by modifying a small string in config files later on. Let's do an example. Call the observable script `wizard.py`, answer the questions as above except choosing a different observable name and answering the following question with yes:
```
Do you want to create a vector observable that can return multiple values? (y/N) y
```
New files `CAFExample/YourObservableName.h` and `Root/YourObservableName.cxx` are being created from the wizard. Vector observables have additional functions compared to simple scalar functions. You will again find many comments and example blocks in the code that will help you establishing your observable class. At this point you are asked to explore yourself and to implement a working vector observable (You need to follow the same 3 steps as for scalar observables). As a suggestion, you might want to create a vector observable returning a list with invariant masses for every two jet combinations. Then, booking a histogram like
```
TH1F('MjjMaxVec', '', 50, 0., 500.) << ( [VecMAX(MjjMaxVec)]*0.001 : 'm_{jj}^{max} [GeV]');
```
should give you the same results than the previously booked histogram with using the `MjjMaxObservable`. You can then also make use of other `TQVectorAuxObservables` such as `VecSUM`, `VecMAX/MIN`, `VecAND` and many more (see [observable README](https://gitlab.cern.ch/atlas-caf/CAFExample/tree/master/share/common/observables) or simply browse through the [code](https://gitlab.cern.ch/atlas-caf/CAFCore/blob/master/QFramework/Root/TQVectorAuxObservable.cxx) for more information).

If you managed to write your vector observable you can also compare it with [MjjVectorObservable](https://gitlab.cern.ch/atlas-caf/CAFExample/blob/master/Root/MjjVectorObservable.cxx) which should already be available in your CAFExample fork.

## Expression member variable
Sometimes you might want to add additional information to your observable, when you call it. This is what expressions are good for. But honestly, expressions are not used frequently and are mostly needed for internal handling. For all intents and purposes, you want to answer no when the wizard asks you.

## Using factories to create observables
Observable factories are a handy way of creating observables on-the-fly. You don't need an observable snippet and the observable will only be instantiated when it is first called. Let's look at an example.

You want to calculate the delta eta between the jets `i` and `j`, but you only know the values of `i` and `j` during runtime. Then you create an observable with a factory that takes as argument a string of the format `DeltaEtaBetweenJets:i,j`. The factory checks if the string starts with `DeltaEtaBetweenJets`. If it does, it creates an observable and uses the indices `i` and `j` to select the correct jets (this is something you have to implement yourself). The advantage is that you will only create this one observable instead of creating one for every combination of i and j.

In the internals of CAF, factories are used all the time. For example when you combine observables in the fashion `[Observable1 * Observable2]`. The factory of the `TQMultiObservable` looks for square brackets with any non-number character in between. If it finds at least one such occurrence the factory instantiates a `TQMultiObservable` from the expression in question.
