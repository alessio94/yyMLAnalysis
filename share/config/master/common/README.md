Master
=========================

Master config files are config files directly passed as command line
arguments to the analysis scripts in this package, as opposed to all
other config files, which are indirectly included and pointed to by
the master config files. They are parsed by the python `configparser`
module and follow the
[corresponding syntax](https://docs.python.org/3/library/configparser.html#quick-start)

Usage
--------------------

Master config files can address one or more of the scripts in this
package. The examples contained in this package are all written to
create a 1-to-1 correspondence between config files and scripts, but
this need not be the case. If you want to have your entire analysis
encoded in a single config file, you can do that easily.

Start off your master config file with a section. For example, if you
want to use the config file in conjunction with the `prepare.py`
script, you should have a section `[prepare]`. Within that section,
you can give arbitrarily many configuration options. The syntax should
look like this.

    # comment
    [section]
    key: value
    anotherkey: anothervalue
    
    [anothersection]
    key: value

The scripts in this package are addressed by the following sections:
  * `prepare.py` reads the contents of the section `[prepare]`
  * `initialize.py` reads the contents of the section `[initialize]`
  * `analyze.py` reads the contents of the section `[analyze]`
  * `visualize.py` reads the contents of the section `[visualize]`
All scripts ignore all other sections except their own.

There is, however, one additional section that is always evaluated:
the section `[config]`. This section currently only supports one
option, `include`, which can be given one (or more comma-separated)
config file names, that are included as if the corresponding file had
been attached to the bottom of this one.

    [config]
    include: some/path/otherconfig.cfg,some/otherpath/moreconfig.cfg

Additional Features
--------------------

The syntax explained above is just one of the options that you
have. You can also use the `TQFolder` text configuration syntax. The
syntax correpsonding to the lines

    [section]
    somenumber: 1
    somestring: hello
    
would be

    +section {
      <somenumber=1>
      <somestring="hello">
    }
    
All of the scripts accept one or more config files, and you can mix
and match different syntax versions to your convenience. The
`TQFolder` syntax also supports including other config files, using
the command `$include("some/otherconfig.txt");`. However, `cfg` files
can only include other `cfg` files with the `[config] include` option,
and `TQFolder` files can only include other `TQFolder` files with the
`$include` command.

All scripts in this package have command line options that allow to
write the fully expanded and compiled set of configuration options to
a text file of either syntax. Use the `--help` option to learn more
about them.

All scripts in this package will autmatically attach a copy of the
config options (and, for that matter, also the command line arguments)
to the sample folder they write out. You can look at them in the
`info` folder, where you will find many other bookeeping information
as well.
submit.py config/master/DijetsFF/analyze-DijetsFakeFactor_2L_Tight.cfg --jobs config/jobLists/DijetsFF/DijetsFF-stbc-jobList-FullRun2.txt --allowArgChanges --identifier Dijets_2L_Tight_MTsplit_v1 --submit=torque --checkmissing
submit.py config/master/DijetsFF/analyze-DijetsFakeFactor_2L_TightMedium.cfg --jobs config/jobLists/DijetsFF/DijetsFF-stbc-jobList-FullRun2.txt --allowArgChanges --identifier Dijets_2L_TightMedium_MTsplit_v2 --submit=torque --checkmissing

#just run the TH2s
submit.py config/master/DijetsFF/analyze-DijetsFakeFactor_2L_Tight_reduced.cfg --jobs config/jobLists/DijetsFF/DijetsFF-stbc-jobList-FullRun2.txt --allowArgChanges --identifier Dijets_2L_Tight_reduced_v1 --submit=torque --checkmissing
submit.py config/master/DijetsFF/analyze-DijetsFakeFactor_2L_TightMedium_reduced.cfg --jobs config/jobLists/DijetsFF/DijetsFF-stbc-jobList-FullRun2.txt --allowArgChanges --identifier Dijets_2L_TightMedium_reduced_v1 --submit=torque --checkmissing


## For the Wjets Control region
submit.py config/master/DijetsFF/analyze-DijetsFakeFactor_2L_Tight_PLIVVeryTight.cfg --jobs config/jobLists/DijetsFF/DijetsFF-stbc-jobList-FullRun2.txt --allowArgChanges --identifier Dijets_2L_Tight_PLIVVeryTight_MTsplit_v1 --submit=torque --checkmissing

```


## visualize commands

```bash

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16a_e.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16d_e.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16e_e.cfg


visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16a_m.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16d_m.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16e_m.cfg


visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_Run2_m.cfg


visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVVeryTight_Run2_e.cfg


visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVVeryTight_Run2_m.cfg



visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16e_e.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_e_eta1.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_e_eta2.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_m_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_m_eta4.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16a_m_trig2015.cfg
# visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16d_m_trig2015.cfg
# visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16e_m_trig2015.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_Run2_m_trig2015.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16a_m_trig2016plus.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16d_m_trig2016plus.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_16e_m_trig2016plus.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_Run2_m_trig2016plus.cfg


visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_Run2_e.cfg



#Nominal plots:
# config/master/DijetsFF/visualize-DijetsFakeFactor.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_e_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16a_e_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16d_e_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16e_e_eta3.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_e_eta4.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16a_e_eta4.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16d_e_eta4.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16e_e_eta4.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_m_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16a_m_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16d_m_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16e_m_eta3.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_Run2_m_eta4.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16a_m_eta4.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16d_m_eta4.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVTight_c16e_m_eta4.cfg


visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVVeryTight_Run2_e_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVVeryTight_Run2_e_eta4.cfg

visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVVeryTight_Run2_m_eta3.cfg
visualize.py config/master/DijetsFF/visualize-DijetsFakeFactor_2L_Tight_PLIVVeryTight_Run2_m_eta4.cfg









```
