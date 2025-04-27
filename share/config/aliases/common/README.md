Aliases
=========================

Aliases are string translations that you can use to keep your config
files lightweight and human-readable. They are parsed by the python
`configparser` module and follow the
[corresponding syntax](https://docs.python.org/3/library/configparser.html#quick-start).

You can use them to globally map placeholders to arbitrarily
complicated string expressions.

Usage
--------------------

Aliases are only needed for the `analyze.py` step, where the contents
of the tree are accessed. You can include your alias file into your
`analyze`-config using the `[config] include` option. Check the
`READMEmd` of the `master` config folder to see how that is done.

Your alias file itself could look like this:

    [analyze]
    aliases.key: value
    
Which will then allow you to use `$(key)` instead of `value` in your
cut and histogram definition files.

Advanced Features
--------------------

Aliases can depend on one another, so

    [analyze]
    aliases.key: $(key1)+$(key2)
    aliases.key1: A
    aliases.key2: B
    
will make `$(key)` expand to `A+B`.

You don't need to include your alias file with the `[config] include`
method. If you use `TQFolder` configuration files, you can use
`$include` instead, in which case your alias file should look like 

    +analyze {
      <aliases.somenumber=1>
      <aliases.somestring="hello">
    }

Alternatively, you can even just list your alias file as an
additional command line argument.

Tips & Tricks
--------------------

If you want an expression that changes depending on which sample you
are currently reading, you cannot use aliases, as they are by
definition global. However, placeholders of the form `$(...)` are not
only expanded using aliases, but also the tags set on your samples and
sample folders. If you want to know how to set these, take a look at
`patches`. Keep in mind, however, that in that case, you do not need
the `aliases.` prefix to your key names.
