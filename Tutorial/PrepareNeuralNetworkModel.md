# Prepare keras models for usage in CAF

This document provides instructions on how to use neural networks (NNs) trained in keras in a CAF analysis.

## Tools

The [lwtnn](https://github.com/lwtnn/lwtnn) package provides C++ libraries to apply NNs and can be incorporated in any CAF based analysis with the wrapper [CAFlwtnn](https://gitlab.cern.ch/atlas-caf/caflwtnn). Please see the repositories for installation instructions. The lwtnn package comes with converter scripts that take saved networks from SciKit Learn or [Keras](https://keras.io/) (recommended!) and dump NN models in standard JSON format. These network json files need to be adapted slightly to be used with CAFlwtnn, which is explained in the following.

# Tutorial to incorporate NN with working commands

Consider the following steps (please start a fresh session on an lxplus machine and login), for which details and general comments are given below:

#### Step 0: Train a neural network
[Keras](https://keras.io/) is recommended, for which there is lots of documentation and support online, no need to elaborate here.
#### Step 1: Save necessary network outputs
We need a network architecture json file, a weights HDF5 file (both from keras), and a input variables file (prepared by the user). Example files can be found under `/eos/user/a/atlascaf/tutorial/2020_Feb/Keras-Example-Network/`.
#### Step 2: Convert outputs to single NN json file

```
# go to your favorite working directory
mkdir $HOME/CAFNNTutorial; cd $HOME/CAFNNTutorial;
export NNFilesPath=/eos/user/a/atlascaf/tutorial/2020_Feb/Keras-Example-Network/;

# necessary clones and installation of h5py for conversion
git clone https://github.com/lwtnn/lwtnn.git; cd lwtnn; make; cd ../;# should take less than 1min
virtualenv -p python3 venv; 
source venv/bin/activate # now source it
pip3 install h5py

# convert
lwtnn/converters/keras2json.py $NNFilesPath/architecture.json $NNFilesPath/variables.json $NNFilesPath/weights.h5 > neural_net.json
deactivate # deactivate python virtualenv

# modify network to assign expressions to variable names
cafsetup; cd $HOME/CAFNNTutorial; # source your analysis setup script to use the following
adaptDNNJSONFileToCAFCore.py --networkInputFile neural_net.json --networkOutputFile neural_net_modified.json --nTupleDefinitionFile $NNFilesPath/ntuple-definition.txt;
```

#### Step 3: Use NN
Now you can use the network in CAF with the expression
```
lwtnnSeq(path/to/neural_net.json, {dense_8})
```
Note, that this uses the "Sequential API" of lwtnn.

# General explanations and tips for different steps

### Save network outputs

To convert the network to a single json file we need three files (see also [Keras-Converter](https://github.com/lwtnn/lwtnn/wiki/Keras-Converter)):

1. keras architecture json file
2. keras weights HDF5 file
3. input variable file

The former two come from keras, e.g. with the following code:

```python
# get the architecture as a json string
arch = model.to_json()
# save the architecture string to a file somehow, the below will work
with open('architecture.json', 'w') as arch_file:
    arch_file.write(arch)
# now save the weights as an HDF5 file
model.save_weights('weights.h5')
```

The input variable file needs to be prepared by the user. The file has the following format for the sequential API (See [Keras-Converter](https://github.com/lwtnn/lwtnn/wiki/Keras-Converter) for the variables file layout of the function API):

```
{
 "inputs": [
        {
          "name": "DPhill",
          "offset": -1.299909432742597,
          "scale": 0.009114703901093474
        },
        ...
        ]
        "class_labels": ["dense_8"]
}
```

If e.g. the `StandardScaler` provided in the `preprocessing` module from `sklearn` is used during the training, the following code dumps a suitable variables file:

```python
varDict = {"inputs":[]}
for i, var in enumerate(arrayOfVariableNames):
    scale = 1./scaler.scale_[i]
    offset = -1.*scaler.mean_[i]
    varDict["inputs"].append( {"name":var, "offset":offset, "scale":scale } )
varDict["class_labels"] = ["nameOfOutputNode"]
with open(variablesFileOutputPath, "w") as jsonfile:
    json.dump(varDict, jsonfile)
```

The only other thing that remains to be done is to manipulate the "name" entry in the variable json file. We need to make our analysis aware of the (CAF) expression that is to be used for the different variables.
We simply add this information to our variable json file with 
```
...
  "name": "DPhill=$(DPhill)",
...
```

Tip: In case the ntuples used for training were dumped by CAF, the ntuple definition file that was utilized has this information included already (see e.g. [here](https://gitlab.cern.ch/atlas-physics/higgs/hww/HWWAnalysisCode/blob/7a69b9e4/share/config/nTuples/VBF/mva-ntuple.txt)). Lines in this file like

```
... , float mjj << $(Mjj). , float DPhill << $(DPhill), ...
```
can be interpreted automatically by the [adaptDNNJSONFileToCAFCore.py](https://gitlab.cern.ch/atlas-physics/higgs/hww/HWWAnalysisCode/blob/7a69b9e4/tools/adaptDNNJSONFileToCAFCore.py) script and the input variables file manipulated with

```
adaptDNNJSONFileToCAFCore.py --networkInputFile variables.json --networkOutputFile variables-modified.json --nTupleDefinitionFile path/to/your/nTuple/definition/file.txt
```

Note, this script can also be run on the final neural network json file, after the following conversion.

### Convert network to single json file

To convert the three files to a single json file that can be used in the analysis, one needs to clone the lwtnn repository

```
git clone https://github.com/lwtnn/lwtnn.git; cd lwtnn; make; # should take less than 1min
```

and use

```
lwtnn/converters/keras2json.py architecture.json variables-modified.json weights.h5 > neural_net.json
```
Note, that this requires python3 and the h5py module to be installed. If you are working on lxplus, python3 will be available, but h5py needs to be installed. The python virtualenv package can be used, e.g., and the above conversion command should succeed.
```
virtualenv -p python3 ./venv
source venv/bin/activate
pip3 install h5py
```

### Make it an observable in CAF

The NN model (which in our case has a layout for the "Sequential API") can then be used by calling the expression
```
lwtnnSeq(path/to/neural_net.json, {nameOfOutputNode})
```
If you use the function API, please use
```
lwtnn(path/to/neural_net.json, {nameOfOutputNode})
```
