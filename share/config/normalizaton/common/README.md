Data Driven Normalization
=========================

Several methods to extract normalization factors (NFs) are available 
and described in the following.

Usage
--------------------

The configuration and scheduling of different calculations uses 
the TQFolder syntax. Each top-level folder corresponds to one 
calculation step and method. The order of folders determines the
order in which the calculations are performed.

High-level settings like number of toy experiments (to propagate 
statistical uncertainties of the numbers entering the calculations)
can be found in the master configuration file. Details on the uncertainty
propagation can be found in the 'Advanced' section below.

So called 'Scale Schemes' are names for different sets of NFs. NFs written to
the Scale Scheme '.default' are automatically applied when creating cutflows or 
plots of histograms unless a different Scale Scheme is explicitly specified 
in that regard. In the context of NF calculators a 'readScaleScheme' refers to the
Scale Scheme applied when a NF calculator retrieves its input values. 
A 'writeScaleScheme' specifies the name under which a NF calculator deposits the 
results of its calculation (some calculators have multiple writeScaleSchemes in 
order to also write out some intermediate results, these are usualy not meaningful
as actual NFs). Finally, when using multiple NF calculators in sequence each result
deposited can be either applied on top of an already existing one (for the same 
process and cut), i.e., setting it to `old x new` or it can simply replace the 
existing NF. This is usually controlled by the option `<overwrite=true>` (in this 
case replacing potentially existing NFs).

In the following, unless indicated otherwise, event yields in data are
considered after subtraction of MC based yields from processes which are 
held fixed within a certain calculation (but potentially taking NF from 
previous steps into account). Often these fixed processes need to be 
explicitly listed (see examples below).

Existing NF calculation methods are described in the following, options 
can be rather specific to a particular NF Calculator:
* TQABCDCalculator: computes a NF scaling the yields in region A (usually the signal region) is equal to A=B*C/D.
    ```
    +VBFABCD {
	    <verbosity=5>
	    #specifying which mode to use, here: TQABCDCalculator
	    <mode="ABCD">
	    #SampleFolder path to obtain MC samples from (used for region A,B,C, and D)
	    <pathMC="bkg/[ee+mm]/Zjets">
	    #SampleFolder path to obtain data samples from (used for regions B,C, and D)
	    <pathData="data/[ee+mm]">
	    #path to apply NFs to, defaults to pathMC if not set
	    <targetPath="bkg/[ee+mm]/Zjets">
	    <saveLog = "log/TQABCDCalculator-vbfZcontrolABCD.log">
	    #name of the cuts defining the A,B,C and D regions
	    <cutA="CutZCR_VBF_A">
	    <cutB="CutZCR_VBF_B">
	    <cutC="CutZCR_VBF_C">
	    <cutD="CutZCR_VBF_D">
	    #calculate and apply non-closure factor (A/B)/(C/D) (all from MC), default is false
	    <applyNonClosureCorrection=true>
	    #first cut in cut hierarchy to apply this NF to.
	    <applyToCut = "CutWeights">
	    #In order to specify multiple starting cuts, one can use the vector tag notation:
	    # <applyToCut = {"CutFoo", "CutBar"}>
	    #last cut in cut hierarchy to apply this NF to. If this doesn't match the name of any cut the 
	    #calculation results will be applied to all cuts descending from (one of the) starting cuts.
	    <stopAtCut = "never">
	    #Analogously one can specify multiple stopping cuts:
	    # <stopAtCut = {"CutBaz","CutOtherBaz"}>
      #overwrite=true replaces existing NFs at the same place (path+scalescheme), overwrite=false multiplies existing NFs with the result of this computation
	    <overwrite=false>
	    #scalescheme to read from (used to scale input quantities). When chaining multiple NF calculators make sure to read a scalescheme
	    <readScaleScheme = ".abcd.read">
	    <writeScaleScheme = {.default,.abcd.write}>
	    #specify (MC) samples that will be subtracted from data before the actual ABCD calculation is performed:
	    + samples {#the name of this folder ('samples') needs to be kept as is!
	      + top { #the name of the folders for the different contributions, however, is arbitrary
			    <path = "bkg/[ee+mm]/top">
		    }
		    + ddFakes {
			    <path = "bkg/[ee+mm]/ddFakes">
		    }
		    + diboson {
			    <path = "bkg/[ee+mm]/diboson">
		    }
		    + QCD {
			    <path = "bkg/[ee+mm]/QCD">
		    }
    }
    
    ```
* TQNFCalculator: provides a set of different methods explained inline in the example. For options already explained in previous examples, please see these for more verbose comments.
    ```
    +VBFSF {
      #When using the TFractionFitter method 'fixed' processes are set to be constraint within their original fraction +/- epsilon. This is required for numerical reasons.
	    <epsilon = 0.001>
	    #first cut in cut hierarchy to apply this NF to
	    <applyToCut = "CutWeights">
	    #last cut in cut hierarchy to apply this NF to
	    <stopAtCut = "never">
	    <saveLog = "log/TQNFCalculator-vbfsf.log">
	    <saveResults = "log/TQNFCalculator-vbfsf.txt">
	    <writePreFitHistograms = "log/TQNFCalculator-vbfsf-preFit.root">
	    <writePostFitHistograms = "log/TQNFCalculator-vbfsf-postFit.root">
	    #SampleFolder path from where data yields are taken unless an explicit 'datapath' is given for particular regions (see below)
	    <defaultDataPath = "data/[ee+mm]">
	    #overwrite=true replaces existing NFs at the same place (path+scalescheme), overwrite=false multiplies existing NFs with the result of this computation
	    <overwrite=false>
	    <readScaleScheme = ".abcd.write">
	    <writeScaleScheme = {.default,.nfc.write}>
	    #list of NF calculation methods to be used. Will proceed to the next method in the list if a method fails.
	    #"Simple": Only possible for 1 NF and 1 non-fixed sample (see blow). Determines the ration data/MC in the given control region
	    #"MatrixInversion": valid for n NFs and n non-fixed samples. Solves the resulting fully determined set of linear equations through matrix inversion.
	    #"TFractionFitter": valid for n NFs and m non-fixed samples where n<m (overconstrained system). Uses TFractionFitter to determine NFs
	    #"FAIL": give up, return failed state
	    <methods = {"Simple","MatrixInversion","TFractionFitter","FAIL"} >
      #MC samples to be considered for this calculation. Fixed samples are subtracted from data before other calculations are done
	    + samples {
		    + top {
			    <path = "bkg/[ee+mm]/top">
			    <lower = 1.>
			    <upper=1.5>
		    }
		    + Zjets {
			    <path = "bkg/[ee+mm]/Zjets">
			    <lower = -3.>
			    <upper = 3.>
		    }
		    + ddFakes {
			    <path = "bkg/[ee+mm]/ddFakes">
			    <fixed = true>
		    }
		    + diboson {
			    <path = "bkg/[ee+mm]/diboson">
			    <fixed = true>
		    }
		    + QCD {
			    <path = "bkg/[ee+mm]/QCD">
			    <fixed = true>
		    }
	    }
	    #Cuts specifying the phase space regions used to determine the NFs
	    + regions {
		    + topControl {
			    <name = "CutTopControl_2jetincl">
		    }
		    + ZControl {
			    <name = "CutZControl_2jetincl">
		    }
	    }
    }
    
    ```
* TQNFManualSetter: this "calculator" allows to set manually/externally derived NFs and uncertainties. For options already explained in previous examples, please see these for more verbose comments.
    ```
    +NFSET {
    #specifying the mode, here: using the TQNFManualSetter
	  <mode="MANUAL">
	  +V1 {#this folder can have an arbitrary name
		  #path to apply NFs to
		  <path="bkg/[ee+mm]/Zjets">
		  #first cut in cut hierarchy to apply this NF to
		  <applyToCut = "CutWeights">
		  #last cut in cut hierarchy to apply this NF to
		  <stopAtCut = "never">
		  #replace the old value
		  <overwrite=true>
		  #scaleschemes the modification should be applied to
		  <writeScaleScheme = {.default,.manual.write}>
		  #value to be applied
		  <value = 42.>
		  #corresponding uncertainty (assumption: gaussian)
		  <uncertainty = 23.>
	  }
	  #a second NF to be placed (there is no strict limitation on how many of these one can add as long as each folder (here: V1,V2) has a unique name)
	  +V2{#this folder can have an arbitrary name
		  #path to apply NFs to
		  <path="bkg/[ee+mm]/top">
		  #first cut in cut hierarchy to apply this NF to
		  <applyToCut = "CutWeights">
		  #last cut in cut hierarchy to apply this NF to
		  <stopAtCut = "never">
		  #multiply the old value with the one given here 
		  <overwrite=false>
		  #scaleschemes the modification should be applied to
		  <writeScaleScheme = {.default,.manual.write}>
		  #value to be applied
		  <value = 42.>
		  #corresponding uncertainty (assumption: gaussian)
		  <uncertainty = 23.>
	  }
  }

    ```
* TQNFTop0jetEstimator: computes a NF according to the top+0jet normalization strategy in the [HWW Run 1 analysis](http://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/PAPERS/HIGG-2013-13/). Please note that some input numbers have partial correlations due to partial event overlap. These correlations are taken into account assuming the regions are structured as in the HWW Run 1 analysis.
    ```
    #example use of TQNF0jetEstimator
    +TOP0JET {
	    <verbosity=5>
	    #specifying the mode, here: using the TQNFTop0jetEstimator
	    <mode="TOP0JET">
	    #path to read MC samples from
	    <pathMC="bkg/[em+me]/top">
	    #path to read data samples from
	    <pathData="data/[em+me]">
	    #path to apply NFs to, defaults to pathMC if not set
	    <targetPath="bkg/[ee+mm+em+me]/top">
	    <saveLog = "log/TQNFtop0jetEstimator-ggFtop0jet.log">
	    #cuts defining the control and jet efficiency regions. Note that the jet efficiency regions are currently being assumed to be fully correlated.
	    <cutCR="CutTop0jetCR">
	    <cutJetEffNumerator="CutTop0jetJetEffNumerator">
	    <cutJetEffDenominator="CutTop0jetJetEffDenominator">
	    #first cut in cut hierarchy to apply this NF to
	    <applyToCut = "CutWeights">
	    #last cut in cut hierarchy to apply this NF to
	    <stopAtCut = "never">
      #overwrite=true replaces existing NFs at the same place (path+scalescheme), overwrite=false multiplies existing NFs with the result of this computation
	    <overwrite=false>
	    #scalescheme to read from (used to scale input quantities). When chaining multiple NF calculators make sure to read a scalescheme
	    <readScaleScheme = ".top0jet.read">
	    <writeScaleScheme = {.default,.top0jet.write}>
	    #specify (MC) samples that will be subtracted from data before the actual Top0jet calculation is performed:
	    + samples {
	      + Zjets {
			    <path = "bkg/[ee+mm]/Zjets">
		    }
		    + ddFakes {
			    <path = "bkg/[ee+mm]/ddFakes">
		    }
		    + diboson {
			    <path = "bkg/[ee+mm]/diboson">
		    }
		    + QCD {
			    <path = "bkg/[ee+mm]/QCD">
		    }
      }
    }
    ```
* TQNFTop1jetEstimator: computes a NF according to the top+1jet normalization strategy in the [HWW Run1 analysis](http://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/PAPERS/HIGG-2013-13/)
    ```
    +TOP1JET {
	    <verbosity=5>
	    #specifying the mode, here: using the TQNFTop1jetEstimator
	    <mode="TOP1JET">
	    #path to read MC samples from
	    <pathMC="bkg/[em+me]/top">
	    #path to read data samples from
	    <pathData="data/[em+me]">
	    #path to apply NFs to, defaults to pathMC if not set
	    <targetPath="bkg/[ee+mm+em+me]/top">
	    <saveLog = "log/TQNFtop0jetEstimator-ggFtop0jet.log">
	    #cuts defining the control and jet efficiency regions. Note that the jet efficiency regions are currently being assumed to be fully correlated.
	    <cut1j0b="CutTop1jet1j0b">
	    <cut1j1b="CutTop1jet1j1b">
	    <cut2j1b="CutTop1jet2j1b">
	    <cut2j2b="CutTop1jet2j2b">
	    #first cut in cut hierarchy to apply this NF to
	    <applyToCut = "CutWeights">
	    #last cut in cut hierarchy to apply this NF to
	    <stopAtCut = "never">
      #overwrite=true replaces existing NFs at the same place (path+scalescheme), overwrite=false multiplies existing NFs with the result of this computation
	    <overwrite=false>
	    #scalescheme to read from (used to scale input quantities). When chaining multiple NF calculators make sure to read a scalescheme
	    <readScaleScheme = ".top1jet.read">
	    <writeScaleScheme = {.default,.top1jet.write}>
	    #specify (MC) samples that will be subtracted from data before the actual Top1jet calculation is performed:
	    + samples {
	      + Zjets {
			    <path = "bkg/[ee+mm]/Zjets">
		    }
		    + ddFakes {
			    <path = "bkg/[ee+mm]/ddFakes">
		    }
		    + diboson {
			    <path = "bkg/[ee+mm]/diboson">
		    }
		    + QCD {
			    <path = "bkg/[ee+mm]/QCD">
		    }
      }
    }
    ```
* TQNFCustomCalculator: computes a NF according to a custom formula (please note: all input numbers are assumed to be uncorrelated when creating toys!) given by the 'expression' option
    ```
    +CUSTOMCALC{
      <verbosity=5>
	    #specifying the mode, here: using the TQNFCustomCalculator
	    <mode="CUSTOM">
	    #the default path to read MC samples from
	    <pathMC="bkg/[ee+mm]/top">
	    #the default path to read data samples from
	    <pathData="data/[ee+mm]">
	    #path to apply NFs to, defaults to pathMC if not set
	    <targetPath="bkg/[ee+mm+em+me]/top">
	    <saveLog = "log/TQNFtop0jetEstimator-ggFtop0jet.log">
      #first cut in cut hierarchy to apply this NF to
	    <applyToCut = "CutWeights">
	    #last cut in cut hierarchy to apply this NF to
	    <stopAtCut = "never">
      #overwrite=true replaces existing NFs at the same place (path+scalescheme), overwrite=false multiplies existing NFs with the result of this computation
	    <overwrite=false>
      #scalescheme to read from (used to scale input quantities). When chaining multiple NF calculators make sure to read a scalescheme
	    <readScaleScheme = ".default">
	    <writeScaleScheme = {.default,.custom.write}>
	    #expression to be used (evaluated using TFormula with parameters following the definitions below)
	    <expression="[0]/[1]">
	    #list how the individual parameters should be obtained: for each parameter the tags "cut.number" and "type.number" have to be set, where "number" is the index of the parameter as used in the expression (please note that the numbering should be continuous (only integers) and starting at 0, otherwise this will cause problems)
	    ##Cut at which the value of parameter 0 should be retrieved:
	    <cut.0="CutTopControl_2jetincl">
	    ##type of parameter (for "data" the processes listed below (+samples{...}) are automatically subtracted. No subtraction is performed for the other types: "MC" and "dataRaw")
	    <type.0="data">
	    #here, we use the same cut as before, but it could be any cut
	    <cut.1="CutTopControl_2jetincl">
	    <type.1="MC">
	    #if we wanted to use a value from another process/path than the one specified by "pathMC=...", we can override it for this parameter with the following line (unless commented)
	    #<path.1="some/other/path">
	
      #specify (MC) samples that will be subtracted from data before the actual calculation is performed:
	    + samples {
	      + Zjets {
			    <path = "bkg/[ee+mm]/Zjets">
		    }
		    + ZjetsEW {
		      <path = "bkg/[ee+mm]/ZjetsEW">
		    }
		    + ddFakes {
			    <path = "bkg/[ee+mm]/ddFakes">
		    }
		    + diboson {
			    <path = "bkg/[ee+mm]/diboson">
		    }
		    + QCD {
			    <path = "bkg/[ee+mm]/QCD">
		    }
      
      }
    }
    ```
    
* TQNFUncertaintyScaler
    ```
    #example to use TQNFUncertaintyScaler (inteded to add systematic uncertainties for example). This NF "calculator" is executed after the regular NF calculation is already finished and (statistical) uncertainties and correlations are computed. Please note that correlations between different NFs calculated previously are not changed by the NFUncertaintyScaler!
    +SYSTERR {
	    #specifying the mode, here: using the TQNFUncertaintyScaler
	    <mode="ERRSCALE"> #or "UNCSCALE" or "UNCERTSCALE"
	    +absExample {
		    #path to modify uncertainties of NFs
		    <path="bkg/[ee+mm]/Zjets">
		    #first cut in cut hierarchy to apply this modification to
		    <applyToCut = "CutWeights">
		    #last cut in cut hierarchy to apply this modification to
		    <stopAtCut = "never">
		    #scaleschemes the modification should be applied to
		    <writeScaleScheme = {.default,.manual.write}>
		    #as an alternative, <scaleScheme = ...> can be used
		    #absolute uncertainty to be added (please note: don't use absvalue and relvalue at the same time!)
		    <absvalue = .3> # increase uncertainties of all specified NFs by 0.3 (linearly, i.e. 100% correlation is assumed)
	    }
	    +relExample{
		    #path to modify uncertainties of NFs
		    <path="bkg/[ee+mm]/Zjets">
		    #first cut in cut hierarchy to apply this modification to
		    <applyToCut = "CutWeights">
		    #last cut in cut hierarchy to apply this modification to
		    <stopAtCut = "never">
		    #scaleschemes the modification should be applied to
		    <writeScaleScheme = {.default,.manual.write}>
		    #as an alternative, <scaleScheme = ...> can be used
		    #relative uncertainty to be added (please note: don't use absvalue and relvalue at the same time!)
		    <relvalue = .25> # increase uncertainties of all specified NFs by 0.25*NF (linearly, i.e. 100% correlation is assumed)
	    }
    }
    ```
    
    
Advanced: Details on Toys and Iterative Calculations
--------------------

In order to propagate statistical uncertainties from both, predictions and (control region) data to the resulting NFs a toy based approach is taken. To this end the chain of calculations is repeated multiple times (the number of toys can be specified by the user, typically via the master configuration file). During each repetition the different calculators obtain a random variation for each of their input values according to the statistical uncertainty of that quantity (assuming a gaussian distribution). This variation is centrally managed such that within one toy it is consistent between multiple uses (typically identified by the combination of cut and process).

If multiple, potentially different steps are used a dependency on the order of calculations can arise due to the subtraction of processes which are considered fixed in one step but are to be normalized in another.  To overcome this the framework can be instructed to run a chain of NF calculations serveral times. The number of these iterations (default: 1) can again (typically) be controlled from the master config file.

In combination with toys the iterations are performed within the context of each individual toy computation, i.e., in pseudo code:
```
foreach toy t:
    forach iteration i:
        runCalculationChain;
    gatherResults;
    resetForNextToy;
```


