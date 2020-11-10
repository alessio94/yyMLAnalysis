This document is intended to provide instructions on how to utilize the GridScanner feature provided by [CAF](https://gitlab.cern.ch/atlas-caf/CAFCore). The tool takes a multidimensional histogram (technically realized with ROOT's THnSparse) as a feature space and scans through this grid by evaluating a figure of merit (usually a significance estimator) for one cut configuration after another. The optimal cut configuration is simply found by sorting the configurations with respect to their resulting figure of merit.

# Running the full example (you-have-no-time-to-read-pseudo-documentation)
The following commands provide an example of how to utilize the tool. From here you can start your own endeavours of finding the ultimate cut based analysis. Assuming you are in the `CAFExample/share` directory execute the following:

```bash
# Step 1: Prepare multidimensional histogram (what we call grid)
./prepare.py xAOD/config/master/prepare-GridScanner-Example.cfg
./initialize.py xAOD/config/master/initialize-GridScanner-Example.cfg
./analyze.py xAOD/config/master/analyze-GridScanner-Example.cfg # takes a while ~10min

# Step 2: Run Gridscanner
../tools/runGridScanner.py common/config/gridscanner/GridScanner-Example.cfg --plotInputs --plotResults
```

# More details on the different steps
## Step 1: Prepare multidimensional histogram
The first thing you need to do is book a multidimensional histogram with all the variables you want to consider for the grid scan. This is done during the analyze step in exactly the same way as you book ordinary histograms. The only thing you have to do is to point to a definition file in which you specify the desired n-dimensional histograms. This is done with a dedicated tag in the analyze config and looks e.g. like this:
```bash
multidimHistograms: xAOD/config/histograms/GridScanner-Example-multidim-histograms.txt
```
Please visit [GridScanner-Example-multidim-histograms.txt](https://gitlab.cern.ch/atlas-caf/CAFExample/blob/master/share/xAOD/config/histograms/GridScanner-Example-multidim-histograms.txt) for explanations on the syntax to book an n-dimensional histograms.

## Step 2: Run Gridscanner
The output of the analyze step - or more so: the multidimensional histogram (or grid) in the output file - can now be read by the [runGridScanner.py](https://gitlab.cern.ch/atlas-caf/CAFExample/blob/master/tools/runGridScanner.py). The script takes a configuration file as an argument. Here, you specify the details of the grid scan, i.e. what variables do you want to regard in the scan, what cut range do you want to consider, and more. Please consider the example configuration file [GridScanner-Example.cfg](https://gitlab.cern.ch/atlas-caf/CAFExample/blob/master/share/common/config/gridscanner/GridScanner-Example.cfg) for documentation on the different options. When executing the gridscanner you can specify the argument `--plotInputs` to dump plots of distributions of the configured variables. The argument `--plotResults` ensures the production of plots that display the figure of merit as a function of the cut values for each variable. At the end of the gridscan you should find a list with the best cut configurations printed to the console. Congratulations! 

In case of any questions, confusions, bug reports or more, please don't hesitate to reach out to us directly or via the mailing list `qframework-users`.
