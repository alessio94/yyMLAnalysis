InputFileLists
======================
InputFileLists provide an alternative to automatic sample discovery functions
in CAF. This allows, for example, to also read files from more complex 
directory structures or grid storage elements (e.g. some LOCALGROUPDISK) as 
long as it can be accessed from the computing nodes used.

Usage
---------------------
Simply list one file path per line. The paths should be formated just as if one
was to pass them to TFile::Open. For example for some files residing on a 
particular grid storage element:

```
dcap://se.bfg.uni-freiburg.de/pnfs/bfg.uni-freiburg.de/data/atlaslocalgroupdisk/rucio/group/phys-higgs/72/bb/group.phys-higgs.361700.e4721_s2726_r7772_r7676_p3042.11320712.PAOD_2LDF._000168.pool.root
dcap://se.bfg.uni-freiburg.de/pnfs/bfg.uni-freiburg.de/data/atlaslocalgroupdisk/rucio/group/phys-higgs/89/c0/group.phys-higgs.361700.e4721_s2726_r7772_r7676_p3042.11320712.PAOD_2LDF._000169.pool.root
dcap://se.bfg.uni-freiburg.de/pnfs/bfg.uni-freiburg.de/data/atlaslocalgroupdisk/rucio/group/phys-higgs/44/6b/group.phys-higgs.361700.e4721_s2726_r7772_r7676_p3042.11320712.PAOD_2LDF._000170.pool.root
```

In general, if a path works when passing it to TFile::Open in an interactive 
ROOT shell CAF should be able to use it as well.


Advanced
---------------------
Everything following a '#' is removed *after* the matching to particular DSIDs
(in case of MC samples). This allows, for example, to add a DSID to a file name
if that file name does not contain the DSID directly (please note that DSID 
in this context does not stricly eforce an actual DSID in the common 6 digit 
format but rather some string used to clearly identify files belonging to a
particular MC sample as also used in the XSec and whitelist files)
