Channels
=========================

If your analysis is splitting into several channels, for example by
lepton flavour, you can configure the layout and pretty-print names of
the channels here.

Usage
--------------------

The channels of your analysis can be defined in a config file like this

    name  path  latex  datapath
    e     e     e      e
    m     m     \mu    m

The meanings of the columns are like this:

 * `name` defines how you want to refer to this channel in your config files

 * `path` defines what the path-component associated to this channel
   is in your sample folder. For example, if you are using paths like
   `bkg/$(channel)/someSample` in your config files, then the
   placeholder `$(channel)` will replaced by the `path` given for each
   channel.

 * `latex` is the pretty-print LaTeX code for your channel to be used
   for plots and somesuch.

 * `datapath` is the same as `path`, but defines what is to be used
   for things declared as `data`. If you wonder why this is important,
   you might want to read the following section.


Additional Features
--------------------

A fairly advanced channel definition file might look like this

    name           path                                                           latex        datapath
    ee             ee$(variation)                                                 ee           ee
    mm             mm$(variation)                                                 \mu\mu       mm
    em             em$(variation)                                                 e\mu         em
    me             me$(variation)                                                 \mu{}e       me
    eemm,mmee,sf   [ee$(variation)+mm$(variation)]                                ee+\mu\mu    [ee+mm]
    emme,meem,df   [em$(variation)+me$(variation)]                                e\mu+\mu{}e  [em+me]
    ll,?,all       [em$(variation)+me$(variation)+ee$(variation)+mm$(variation)]  \ell\ell     [em+me+ee+mm]

Here, you can see that in `name`, you can also define several names
for each channel for convenient use in your config files.  You might
wonder about the fact that the `path` also contains a placeholder
variable `$(variation)` here. This is useful for setups where the
systematic variations are treated similar to channels, which is a very
useful design, because systematic variations are often saved in
separate files and/or can often be processed in parallel in much the
same way as channels are. Now, it also becomes apparent why there is a
separate column `datapath`, because for data, there are usually no
systematic variations.