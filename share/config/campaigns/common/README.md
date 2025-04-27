Campaigns (visualization)
=========================

If your analysis is splitting into several campaigns, you can 
configure the layout and pretty-print names of the campaigns here.

Usage
--------------------

The campaigns of your analysis can be defined in a config file like this

    name  path  latex     datapath
    c16a  c16a  2015+2016 c16a
    c16d  c16d  2017      c16e
    c16e  c16e  2018      c16e

The meanings of the columns are like this:

 * `name` defines how you want to refer to this campaign in your config files

 * `path` defines what the path-component associated to this campaign
   is in your sample folder. For example, if you are using paths like
   `bkg/$(channel)/$(campaign)/someSample` in your config files, then the
   placeholder `$(campaign)` will replaced by the `path` given for each
   channel (for the replacement of `$(channel)` see the similarly structured 
   channels definition file).

 * `latex` is the pretty-print LaTeX code for your campaign to be used
   for plots and somesuch.

 * `datapath` is the same as `path`, but defines what is to be used
   for things declared as `data`. If you wonder why this is important,
   you might want to read config/channels/common/README.md.


A more advanced campaign definition file might look like this

    name           path                                                           latex          datapath
    c16a           c16a                                                           2015+2016      c16a
    c16d           c16d                                                           2017           c16d
    c16e           c16e                                                           2018           c16e
    c16ad          [c16a+c16d]                                                    2015-2017      [c16a+c16d]
    c16ae          [c16a+c16e]                                                    2015+2016+2018 [c16a+c16e]
    c16de          [c16d+c16e]                                                    2017+2018      [c16d+c16e]
    run2,?,all     [c16a+c16d+c16e]                                               Full~Run~2     [c16a+c16d+c16e]

