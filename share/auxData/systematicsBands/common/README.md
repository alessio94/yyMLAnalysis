systematicsBands
=========================

Making plots with systematic uncertainties shown can be a tedious
exercise, since the systematic variations can occupy a large amount of
space, making their usage slow, and can require excessive
bookkeeping. Hence, CAFCore supports a very specific format of file,
the *systematicsBands* file. These are instances of `TQFolder` saved
in `ROOT` files that can contain either total yield uncertainties, or
shape uncertainty histograms, or both.

Format
--------------------

The standard way a *systematicsBand* file should look like is a `ROOT`
file with a content like the following:

    +SomeCut {
      <yield = 0.01> # this is a 1% total yield uncertainty
      TH1F("leadLepPt", "", 2, 0, 100) << 100@{0+-0,0.1,-0.2,0+-0}; # this is a 2-bin shape histogram with a +10% variation in the first bin and a -20% variation in the second bin
      ...
      # arbitrarily many histograms can go here
    }
    +SomeOtherCut {
      <yield = 0.1> # this is a 10% total yield uncertainty
      TH1F("leadLepPt", "", 2, 0, 100) << 100@{0+-0,0.5,-1.5,0+-0}; # this is a 2-bin shape histogram with a +50% variation in the first bin and a -150% variation in the second bin
      ...
      # arbitrarily many histograms can go here
    }
    ...

Creation
--------------------

These files can be generated with the class `TQSystematicsHandler`
based on an input sample folder that contains the systematic
variations. A script to enable creation of these files called
*generateSystematicsBands.py* is provided in the *tools* directory. In
order to use this script, you will need a configuration file defining
your systematic uncertainties. This file might look like this:

    # -*- mode: tqfolder -*-
    +Variations {
        # sfSystematics
        +MyVariationDown {
            <Variation = "_MyVariation_Down">
        }
        +MyVariationUp {
            <Variation = "_Myvariation_Up">
        }
        ...
    }
    
    +Systematics {
    	  +MySystematic {
    	    <Down = "MyVariationDown", Up = "MyVariationUp">
    	  }
        ...
    }

If your variations all exist as parts of the same sample folder, for
example, in a pattern like this

    bkg/
      em/
      me/
      em_MyVariation_down/
      me_MyVariation_down/
      em_MyVariation_up/
      me_MyVariation_up/

you could have this automatically resolved by calling

   python generateSystematicsBands.py --input samples.root:samples --pathPattern "bkg/[em\$(Variation)+me\$(Variation)]"

If, on the other hand, your variations, exist is different files like this

   samples.root
   samples_MyVariation_Up.root
   samples_MyVariation_Down.root

you could want to call

   python generateSystematicsBands.py --input samples\$(Variation).root:samples --pathPattern "bkg/[em+me]"

Usage
--------------------

Once you have obtained a *systematicsBands* file, you can easily use
it to have the systematic uncertainty band added to your plots, using
the `systematicsBands` option of your `visualize` config. There, you
would put a string like

    systematicsBands:"common/auxData/systematicsBands/mySystematics_df>>::df","common/auxData/systematicsBands/mySystematics_em>>::em","common/auxData/systematicsBands/mySystematics_me>>::me"

to have your three *systematicsBands* files be loaded and added to the
plotter under the names `df`, `em` and `me`. As long as the names you
pick here are identical to the names of your channels as defined in
your channel configuration file, the systematics for each channel will
be automatically picked up by the plotter. If you don't like that You
can then select which one of them will be used with the flag

    plotter.errors.showSys: default

You can also apply systematics to a cutflow by putting the tag
`.includeSysError="mylabel"` into your cutflow process definition
file. Be careful though, because the cutflow printer will only pick up
the number you give in your systematics file as a relative variation
and apply, which would usually be a systematic uncertainty for the
total background yield. If you want to have the systematics broken
down for each process in your cutflow, you will need to produce one
*systematicsBand* file for each process you want to have systematics
for!
