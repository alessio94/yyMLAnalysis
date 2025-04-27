MVA
=========================

Many physics analyses use multivariate techniques to improve their
sensitivity. A default way to do this in the context of a `ROOT`-based
analysis would be the `TMVA` interface, which supports amongst many
other techniques also the very popular *boosted decision trees*.

Multivariate classifiers need to be trained before they can be
used. The training can be done with `TMVA`, to which `CAFCore`
interfaces with the wrapper class `TQMVA`.

Usage
--------------------

The files in this directory are python scripts that are dynamically
loaded and executed by `analyze.py` should you choose to list them as

    MVA: myAnalysis/config/MVA/myMVA(eventSelector="Even"),myAnalysis/config/MVA/myMVA(eventSelector="Odd")

which will cause the file `myAnalysis/config/MVA/myMVA.py` to be
loaded. As you can see, you can also choose to load the same script
several times. The braces `(...)` and their contents are optional and
can be dropped, but their significance will be explained below. The
file itself might look like this:

    import QFramework

    def runMVA(mva):
        # the object passed here is an instance of TQMVA
        name =  "myBDT" + mva.getTagStandardStringDefault("eventSelector","")
        mva.createFactory(name+".root","V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification")
    
        mva.setTagBool("makeCounters",True)
    
        # add the signal and background samples (by path)
        mva.addSignal("sig/?/?/vbf")
        mva.addBackground("sig/?/?/ggf")
        mva.addBackground("bkg/?")

        # print the list of samples (debugging only)
        mva.printListOfSamples(TQMVA.Signal)
        mva.printListOfSamples(TQMVA.Background)
    
        # set the base cut to be used
        cut = mva.useCut("CutVBF2jet")
        QFramework.INFO("using cut '{:s}'".format(cut.GetName()))
    
        # book the variables to be used
        # the arguments are:
        #   mva.bookVariable(name, expression, title, type, min, max)
        # where:
        #   name is some arbitrary identifier
        #   expression is the definition of the variable (using the actual branch names)
        #   title is the string to be used for the histogram and axis titles (latex-formatted)
        #   type is the TMVA variable type
        #   min and max are the lower and upper bounds forwarded to the TMVA
    
        mva.bookVariable("DYjj"   , "$(DYjj)"          ,"DY_{#it{jj}}"           , 0.,8.)
        mva.bookVariable("Mjj"    , "$(Mjj)/1000."     ,"M_{#it{jj}} [GeV]"      , 0.,2500.)
        mva.bookVariable("DPhill" , "$(DPhill)"        ,"DPhi_{#it{ll}} [rad]"   , 0.,10.)
        mva.bookVariable("MT"     , "$(MT)/1000"       ,"M_{#it{T}} [GeV]"       , 0.,290.)
        mva.bookVariable("Mll"    , "$(Mll)/1000"      ,"#it{m}_{#ll} [GeV]"     , 0.,10000.)
        mva.bookVariable("contOLV", "$(contOLV)"       ,"continous OLV"          , 0.,2.)
        mva.bookVariable("SumMlj" , "$(SumOFMLepxJety)","#it{m}_{#lj} [GeV]"     , 0.,250.)
        mva.bookVariable("pT_Tot" , "$(ptTot)"        ,"#it{p}_{T}^{tot} [GeV]"  , 0.,200.)
  
        # set the verbosity of the TQMVA object
        mva.setVerbose(True)
  
        # read in the samples 
        # this one actually contains the event loop and may take a long time
        if (mva.getTagStringDefault("eventSelector","") == "") :
            mva.readSamples()
        elif (mva.getTagStringDefault("eventSelector","") == "EVEN") :
            mva.readSamples(TQMVA.EvenOddEventSelector())
        elif (mva.getTagStringDefault("eventSelector","") == "ODD") :
            mva.readSamples(TQMVA.OddEvenEventSelector())
  
        # prepare the input trees
        mva.prepareTrees()
      
        # retrieve the TMVA factory object
        factory = mva.getFactory()
    
        factory.BookMethod( TMVA.Types.kBDT, name, "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:DoBoostMonitor:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining" );	
  
        # perform the TMVA magic
        factory.TrainAllMethods()
        factory.TestAllMethods()
        factory.EvaluateAllMethods()
  
        # save and close the output file
        mva.closeOutputFile()
  
        return True

The important detail is that your file needs to contain a function
called `runMVA`, which will be called by `analyze.py` with an instance
of `TQMVA` as an argument. You then need to perform the steps similar
to the example given above to perform the training, but as this is
python, you can execute arbitrarily complicated operations here. 

Any arguments you gave to the `MVA` option via the `(...)` expression
will be passed down to your script via as tags on the `TQMVA`
object. In the example above, this is used to steer whether the MVA
training will be performed on even or odd events.