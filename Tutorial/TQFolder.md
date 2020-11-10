This document provides hands-on instructions for the [CAF
tutorial](https://indico.cern.ch/event/875315/). The instructions are meant to be 
self-contained and usable as an offline resource. Please let us know if they are not.

The only prerequisite of this tutorial is a working
[CAF](https://gitlab.cern.ch/atlas-caf/CAFCore) installation. You can check this
by running (for example in the run directory)

<!-- console -->
```bash
$ python -c "import QFramework"
xAOD::Init                INFO    Environment initialised for data access
```

The `TQFolder` syntax is a versatile syntax to define annotated trees. Several
utility methods allow the definition and manipulation of large trees in a few
lines. The syntax and TQFolders are used in several places in CAF. For examples
the cuts file is written in TQFolder syntax; all the samples, histograms and
counters are organized and stored in TQFolders.

In this tutorial, we will work on an
example completely detached from a real-life physics analysis. This tutorial builds a tree structure of
animals (not even close to real taxonomy). This approach underlines the
versatility of the syntax and helps to focus on the actual syntax instead of
being distracted by all the details of a real physics cut file.

# Basic `TQFolder` syntax
We want to create a tree structure containing information about animals. The
base folder should be called "Animals". The sub folders "Mammals", "Birds" and
"Reptiles" should also contain a third layer of sub folders. The desired
structure is illustrated below.

```
Animals
 |- Mammals
 |   |- Dog
 |   |- Cat
 |   |- Fox
 |- Birds
 |   |- Eagle
 |   |- Mockingbird
 |- Reptiles
     |- Turtle
     |- Lizard
```

The standard way to define a folder in TQFolder syntax is by using the `+FolderName { Content }`
syntax, where `FolderName` is an arbitrary name, `Content` 
is a placeholder for the contents of a folder. We can create the file
`animals.tqfolder` (for example in the run directory) and add 

<!-- write animals.tqfolder -->
```php
+Animals {
  +Mammals {
    +Dog {
    }
    +Cat {
    }
    +Fox {
    }
  }

  +Birds {
    +Eagle {
    }
    +Mockingbird {
    }
  }

  +Reptiles {
    +Turtle {
    }
    +Lizard {
    }
  }
}
```

Let us check that the syntax above is correct.
<!-- console -->
```bash
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('animals.tqfolder').printContents(':dtr2')"
```

So far, the tree does not contain a lot of information. A very useful feature of
TQFolders is that they can be annotated with tags. A tag is a key value pair
attached to a specific TQFolder in the tree. The TQFolder syntax allows
you to attach tags directly while we define the folders. A folder with the four
tags `tagName1="value"`, `tagName2=42`, `tagName3="other_value"` and
`tagName4="spanning over two lines"` can be created with

<!-- write tag_example.tqfolder -->
```php
+FolderName {
  <tagName1="value">
  <tagName2=42, tagName3="other_value">
  <tagName4 = 
		"spanning over two lines">
}
```

Multiple tags can be defined in separate `<>` brackets or multiple can be defined at once if they are separated by a comma.

Let us check that the syntax above works, i.e. create the file 'tag_example.tqfolder' with the content above and execute
<!-- console -->
```
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('tag_example.tqfolder').printContents(':dtr2')"
```
'tagName2' is of type integer. Can you modify the file such that it becomes another string or a double? Can you also make a tag of type bool?

As an example we would like to annotate our animal tree with tags to
indicate the animals skin type (`skin` tag), whether they are vegetarian (tag
`vegetarian`) and their approximate global population.

<!-- write animals.tqfolder -->
```php
+Animals {
  +Mammals {
    <skin="fur">
    <vegetarian=false>
    +Rabbit {
      <vegetarian=true>
      <population=8.5e8>
    }
    +Cat {
      <population=6e8>
    }
    +Fox {
      <population=2.4e5>
    }
  }

  +Birds {
    <skin="feather">
    +Hummingbird {
      <vegetarian=true>
      <population=2.3e6>
    }
    +Mockingbird {
      <vegetarian=false>
      <population=3.2e7>
    }
  }

  +Reptiles {
    <vegetarian=false>
    +Turtle {
      <skin="shell">
      <population=120000>
    }
    +Lizard {
      <skin="dragon">
      <population=9e5>
    }
  }
}
```

Let us confirm that this works. In order to see all the tags, we need to print 3 layers of content now
<!-- console -->
```
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('animals.tqfolder').printContents(':dtr3')"
```

Wait. There are tags attached to parent folders, like `skin` attached to
`Mammal`, but the `population` tags are attached to the leaf notes?  Are all
Mammals non-vegetarian? We will see
in the next chapter why this structure is useful. In short, it prevents us from
repeating tags if they apply for all or almost all the child folders.

So far, we have seen the standard way of defining a full TQFolder tree. There
are alternative methods for setting tags and creating folders. The alternative
syntax can be used to modify or extend an existing TQFolder tree. We can modify
and extend specific folders in the tree by addressing them with their path, e.g.
`Animals/Birds/Mockingbird`.

The path can contain the wildcard characters `?`
and `*`. The meaning of the two wildcards depends on their use
* If they are used
as a placeholder of a full folder name (i.e. between two `/` or between `/` and
the start/end of the path), `*` matches any number of folder layers (including
zero), whereas `?` match exactly one folder layer. For example
`Animals/?/Ghost` matches `Animals/Mammals/Ghost`, `Animals/Birds/Ghost` or
`Animals/Reptiles/Ghost`. Multiple matches are possible. On the other hand
`Animals/*/Ghost` would match `Animals/Ghost`, `Animals/Mammals/Ghost`,
`Animals/Mammals/Cats/EuropeanHouseCat/Ghost`.
* When the wildcard characters are used as part of a folder name, `*` matches any
number of characters, whereas `?` matches exactly one character. This means
`R*` matches `Rabbit` or `Reptiles`, `?at` matches `Rat`, `Cat` or `Bat`.

To set tags in an existing TQFolder tree, you can use the `<TagName=Value> @FolderPath;`
syntax. Let us say we want add a tag to the `Fox` folder. Append
<!-- append animals.tqfolder -->
```
<better_than_chrome=true> @Animals/Mammals/Fox;
```
to your existing TQFolder definition file 'animals.tqfolder'.

Furthermore, lets assume, that we want to add the tag `number_of_legs` tag to all
the leaf folders. All the animals except birds have 4 legs in our example. This
means we could use wildcards to add the tag `number_of_legs=4` to all leaf
folders and then overwrite this value for Birds. Add 

<!-- append animals.tqfolder -->
```
<number_of_legs=4> @Animals/?/?;
<number_of_legs=2> @Animals/Birds/?;
```

Let us check that the tag example is correct. Does the order of these lines matter?
<!-- console -->
```
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('animals.tqfolder').printContents(':dtr3')"
```

Instead of setting just tags, we can also create new folders in an existing
folder tree. Append the following lines of code to the TQFolder definition file
in order to add a lion.
<!-- append animals.tqfolder -->
```
@Animals/Mammals {
  +Lion {
    <population=20000>
    <number_of_legs=4>
  }
}
```

This piece of code adds a new folder to the `Mammals` folder. As you see, it is
possible to create new TQFolders and/or new tags using this method. We are not
limited to a single folder nor to a single folder layer. In our example, we have to
set the `number_of_legs` tag manually because the `<number_of_legs=4> @Animals/?/?;`
line is executed before the lion folder existed. The lion is placed amoung the 
remaining mammels in the printout.

This syntax becomes really powerful when we combine it with wildcards. If we are
charged with the task to add the `Unicorn` to the `Mammals`, `Birds` and
`Reptiles` folder, we can do this  by adding
<!-- append animals.tqfolder -->
```
@Animals/? {
  +Unicorn {
    <population=0>
    <number_of_legs=4>
    <vegetarian=true, skin="fur">
  }
}
```
to our TQFolder definition file. Use the familiar command to confirm that it works.

It is often helpful to have tag value, which contains (or equals) the name of
the TQFolder it contains. This can be set via the 'magic' BASEFOLDERNAME
syntax, e.g. '<whoami="$(BASEFOLDERNAME)>'.

Similarly, you can set a name of a SampleFolder by using a tag name by using
the '$(~tagname)' syntax, which helps creating multiple similar SampleFolders
at once. An example can be found on slide 8 of
https://indico.cern.ch/event/875315/contributions/3689030/attachments/1979936/3296736/talk.pdf
Here 'tagname' = 'region'.

So far so good, we can now create a TQFolder. In the next section we will learn to interact with it.

# API
In this chapter we will work with an interactive python shell and look how we
can interact with our animals tree. Start a interactive python shell and
import `TQFolder`

<!-- write load.py -->
```
python
>>> from QFramework import TQFolder
```

## Loading and saving TQFolders
The result of the previous chapter should be stored in the file
`animals.tqfolder`. We can load a TQFolder definition with the static method
`TQFolder.loadFromTextFile`.

<!-- append load.py -->
```python
animals = TQFolder.loadFromTextFile("animals.tqfolder")
print animals
```

This is it (unless you have a syntax error in the definition file in which case you will be greeted by a
`ReferenceError: attempt to access a null-pointer`).

Make yourself familiar with our TQFolder by printing it and check if all the
`number_of_legs` tags are correct, and that there are unicorns in the tree. For
example run

<!-- append load.py -->
```python
animals.printContents()
animals.printContents("Animals/Birds:dt")  # d=details, t=include tags
animals.printContents("Animals/Mammals/Fox:dt")
animals.printContents("Animals/Reptiles:dtr2")  # r2=recursive 2 layers
```


We can write the TQFolder tree and its tags to a text file. In order to do this,
run
<!-- append load.py -->
```python
animals.exportToTextFile("animals_exported.tqfolder")
```

This creates the file `animals_exported.tqfolder` containing our animals
tree in standard syntax. Open it with your favorite editor. It should look like
<!-- console_output -->
```
$ cat animals_exported.tqfolder
# -*- mode: tqfolder -*-
+Animals {
	+Mammals {
		<skin = "fur", vegetarian = false>
		+Rabbit {
			<vegetarian = true, population = 850000000, number_of_legs = 4>
		}
		+Cat {
			<population = 600000000, number_of_legs = 4>
		}
		+Fox {
			<population = 240000, better_than_chrome = true, number_of_legs = 4>
		}
		+Lion {
			<population = 20000, number_of_legs = 4>
		}
		+Unicorn {
			<population = 0, number_of_legs = 4, vegetarian = true, skin = "fur">
		}
	}
	+Birds {
		<skin = "feather">
		+Hummingbird {
			<vegetarian = true, population = 2300000, number_of_legs = 2>
		}
		+Mockingbird {
			<vegetarian = false, population = 32000000, number_of_legs = 2>
		}
		+Unicorn {
			<population = 0, number_of_legs = 4, vegetarian = true, skin = "fur">
		}
	}
	+Reptiles {
		<vegetarian = false>
		+Turtle {
			<skin = "shell", population = 120000, number_of_legs = 4>
		}
		+Lizard {
			<skin = "dragon", population = 900000, number_of_legs = 4>
		}
		+Unicorn {
			<population = 0, number_of_legs = 4, vegetarian = true, skin = "fur">
		}
	}
}
```

The above method only makes sense if we do not have any binary object (like
`TH1F`) inside our tree. In our case we only had tags. If you want to save and
load a TQFolder including its objects, you should use 

<!-- append load.py -->
```
animals.setName("animal_tree")
animals.writeToFile("tutorial.root")
reloaded = TQFolder.loadFolder("tutorial.root:animal_tree")
reloaded.printContents()
```

Of course, usually you will exit the python session and start a new one before
reloading the folder.

You can also open the `tutorial.root` file with ROOT and inspect it with `.ls` or
a `TBrowser`. There should be an item called `animal_tree`. This is our TQFolder
inside the ROOT file.

In the following you will load the animals tree a number of times. This can be
done by calling 'python -i load.py' with the following file content (the last
line is a check)
<!-- console -->
```
from QFramework import TQFolder
animals = TQFolder.loadFromTextFile("animals.tqfolder")
print animals
```

## Tags
Loading and saving TQFolders is already cool, but we want to do more. For
example we want to read and write tags.

For example, assume we want to get the population of turtles programmatically.
We can retrieve the turtle folder and get the value of the `population` tag by
using the `getTag...()` methods.
<!-- append load.py -->
```
turtle = animals.getFolder("Animals/Reptiles/Turtle")
population = turtle.getTagIntegerDefault("population", -1)
print population
```
The method `getTagIntegerDefault` returns the tag's value as an integer. If the
tag is not found, it returns the given default value, here `-1`. There are a
couple of other methods to retrieve strings, booleans, lists (a.k.a. vector
tags), etc. Type `animals.getTag` and press `Tab` to trigger the auto completion.
The list should include
`getTagStandardStringDefault`, `getTagString`, `getTagStringDefault`,
`getTagVBool`, `getTagVDouble`, `getTagVInt`, `getTagVInteger`,
`getTagAsString`, `getTagVStandardString`, `getTagBool`, `getTagVString`,
`getTagBoolDefault`, `getTagDefault`, `getTagDouble`, `getTagDoubleDefault`,
`getTagInteger`, `getTagIntegerDefault`, `getTagList` and `getTagListLength`.

When using the `getTagIntegerDefault` method, we can not distinguish the cases
when the tag value is equal to the default, or the tag is non-existent. We can check
whether a tag exists with

<!-- append load.py -->
```
print turtle.hasTag("population")
```
which in this case prints `True`.


To modify existing or add new tags, we can use the method `setTag("tagName", "value")`.
The type of the tags is deduced form the value you give to the method. Assume
that we have discovered that the mammal-type unicorn has only three legs. The following
lines fix this issue.

<!-- append load.py -->
```
mammal_unicorn = animals.getFolder("Animals/Mammals/Unicorn")
mammal_unicorn.setTag("number_of_legs", 3)
mammal_unicorn.printContents(":dt")
```

In the beginning, when we annotated the folders with tags, we attached some tags
to the parents. How does this work out now? Let us say we want to know the skin
type of a rabbit. Is there a skin tag for the rabbit?

<!-- append load.py -->
```
rabbit = animals.getFolder("Animals/Mammals/Rabbit")
print rabbit.hasTag("skin")
```

The above snippet prints `False`. So the answer is no. We can, however, ask: *Is
there a skin tag for the rabbit or any of its parents. If yes, print its value.*

<!-- append load.py -->
```
if rabbit.hasTag("~skin"):
  print rabbit.getTagStringDefault("~skin", "")
```

Instead of using the exact tag name `skin`, we can prefix it with a tilde `~`
to get the tag at the folder or at any of its parents. The snippet above
therefore prints `fur`.

This also works the other way round. If you append a tilde to a tag name, it will
search for the tag at the folder or at any of its children.

> Task: Unicorns have been spotted. Change the population of all unicorns to 1
  by using the API.

## Creating folders in python

Besides manipulating tags programmatically, we can also manipulate TQFolders
itself within python. New TQFolders can be created via the constructor.

<!-- console -->
```
new_folder = TQFolder("new_folder")
animals.addFolder(new_folder)
```

Load the initial TQFolder and confirm that the above creates a new TQFolder and
adds it to the animals tree.

> Task: Add the eagle to the tree. Make sure that it has the usual tags:
  `number_of_legs`, `vegetarian`, `skin`, `population`. Some of its parent tags
  might be reusable.

## Patching the folder
In CAF, TQFolders are often *patched*. This means, that an other TQFolder
definition file is loaded in order to modify an existing TQFolder structure.
For example, this is  done after Monte Carlo samples have been initialized. This
allows the use to set tags or move/copy samples around.


Back to our example. Lets create a patch file called `patch.tqfolder`.

<!-- write patch.tqfolder -->
```
<population=2> @Animals/Mammals/Unicorn;
```

Run `animals.importFromTextFile("patch.tqfolder")`
to patch our animals with the above file. Instead of loading a file, you can
pass the TQFolder string directly to `importFromText`.

> Task: It has been discovered that there are exactly two unicorns of all types. Apply
  this modification using a patch file. Wildcards might make things easier.

> Task: Add the dog to the tree using a patch file. Make sure that it has the usual tags:
  `number_of_legs`, `vegetarian`, `skin`, `population`. Some of its parent tags
  might be reusable.


# Advanced
So far, all the methods were static. The TQFolder syntax defines functions which
are executed while the folder is loaded. These functions modify the tree structure
and influence the way the file is parsed. These functions are particularly powerful
in a patch files where you can change an existing structure. All these
functions start with a `$` sign followed by their name and
their arguments in parenthesis.

The subsections in this chapter each cover a single function. The chapters assume
that you have the following animals tree in the file
`animals_advanced.tqfolder`. Make sure that you restore this file at the
beginning of each section.

<!-- write animals_advanced.tqfolder -->
```
+Animals {
  +Mammals {
    <skin="fur">
    <vegetarian=false>
    +Rabbit {
      <vegetarian=true>
      <population=8.5e8>
    }
    +Cat {
      <population=6e8>
    }
    +Fox {
      <population=2.4e5>
    }
    +Hummingbird {
      <vegetarian=true>
      <population=2.3e6>
      <skin="feather">
    }
  }

  +Birds {
    +Mockingbird {
      <vegetarian=false>
      <population=3.2e7>
      <skin="feather">
    }
  }

  +Reptiles {
    <vegetarian=false>
    +Turtle {
      <skin="shell">
      <population=120000>
    }
    +Lizard {
      <skin="dragon">
      <population=9e5>
    }
  }
}
```

## $copy
The `copy` method lets you copy parts of the TQFolder tree to a new location.
The
function takes a single string argument containing the source and destination
path separated by `>>`.
Lets say we want to copy the hummingbird folder from the mammals to the
birds. Copy the original animals_advanced file

<!-- console -->
```
$ cp animals_advanced.tqfolder copy.tqfolder
```

and append the following command to the `copy.tqfolder` file.
<!-- append copy.tqfolder -->
```
$copy("Animals/Mammals/Hummingbird >> Animals/Birds");
```

Let us check that the syntax above is correct.
<!-- console -->
```
$ echo "Copy"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('copy.tqfolder').printContents(':dtr2')"
```

If we want to rename the folder on the fly, we can append `::NewName` to the
argument. Lets say, by mistake the folder named `Hummingbird` should represent
the pigeon instead, we could write

```
$copy("Animals/Mammals/Hummingbird >> Animals/Birds::Pigeon");
```

## $move
The `move` method lets you move parts of the TQFolder tree to a new location.
The syntax of `move` is identical to `copy`.

> Task: We want to move the hummingbird folder from the mammals to the
  birds.

<!-- console
```
$ cp animals_advanced.tqfolder move.tqfolder
```
-->

<!-- append move.tqfolder
```
$move("Animals/Mammals/Hummingbird>>Animals/Birds");
```
-->

<!-- Let us check that the syntax above is correct. -->
<!-- console
```
$ echo "Move"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('move.tqfolder').printContents(':dtr2')"
```
-->

Be careful never to move a folder into itself or into a subfolder of itself.
Possible side effects include crashes that are very hard to debug.

## $create
The `create` method creates a new `TQFolder` or `TQSampleFolder`. The function
takes the two named arguments `path` and `type`.  The type can be either "f" or
"folder" to create a `TQFolder`, or it can be "sf" or "samplefolder" to create a
`TQSampleFolder`. Parent folders are created automatically if missing.

```
$create(path="path/to/new/folder", type="f or sf");
```

For TQFolders the '+' operator can be used as well as 'create'. Main use case
for the 'create' method is making TQSampleFolders, which cannot be done via '+'.

> Task: Create a new TQFolder for the mammal zebra.

<!-- console
```
$ cp animals_advanced.tqfolder create.tqfolder
```
-->

<!-- append create.tqfolder
```
$create(path="Animals/Mammals/Zebra", type="f");
```
-->

<!-- console
```
$ echo "Create"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('create.tqfolder').printContents(':dtr2')"
```
-->

## $delete
The `delete` method lets you delete parts of the TQFolder. The `delete` method
takes a single argument specifying the folder. If the folder is not empty,
deleting it will fail. To delete non-empty folders,  append `!` to the folder
name.

```
$delete("some/folder/with/children!");
```

> Task: We want to delete the hummingbird folder from the mammals.

<!-- console
```
$ cp animals_advanced.tqfolder delete.tqfolder
```
-->

<!-- append delete.tqfolder
```
$delete("Animals/Mammals/Hummingbird!");
```
-->

<!-- console
```
$ echo "Delete"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('delete.tqfolder').printContents(':dtr2')"
```
-->

## $ignore
The `ignore` command functions as a block comment. It does not take any 
arguments. Everything wrapped in curly braces following the ignore command is
ignored. The block inside the curly braces is arbitrary, however you have to
make sure that the opening and closing curly braces match.

```
$ignore() {this here is never
executed}
```

> Task: Modify `animals_advanced.tqfolder` such that the hummingbird is ignored.

<!-- write ignore.tqfolder
```
+Animals {
  +Mammals {
    <skin="fur">
    <vegetarian=false>
    +Rabbit {
      <vegetarian=true>
      <population=8.5e8>
    }
    +Cat {
      <population=6e8>
    }
    +Fox {
      <population=2.4e5>
    }
    $ignore() {
    +Hummingbird {
      <vegetarian=true>
      <population=2.3e6>
      <skin="feather">
    }
    }
  }

  +Birds {
    +Mockingbird {
      <vegetarian=false>
      <population=3.2e7>
      <skin="feather">
    }
  }

  +Reptiles {
    <vegetarian=false>
    +Turtle {
      <skin="shell">
      <population=120000>
    }
    +Lizard {
      <skin="dragon">
      <population=9e5>
    }
  }
}
```
-->

<!-- console
```
$ echo "Ignore"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('ignore.tqfolder').printContents(':dtr2')"
```
-->

## $escape
The `escape` method is similar to ignore but takes an optional argument which
gives it the power of an if statement. Without the optional argument everything
in the current level of a sample folder below the statement is skipped.

<!-- write escape.tqfolder -->
```
+AA {
  +A {}
  $escape();
  +B {}
}
+BB {
  +C {}
}
```

The result of `escape` in this case is that `AA` contains only `A`. The folder
`BB` and `C` are unaffected by the usage of escape

<!-- console
```
$ echo "Escape"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('escape.tqfolder').printContents(':r2')"
```
-->

If the optional argument is used, the rest of the current folder level is
skipped when the TQFolder name matches the argument. Please note that optional
argument must not be put in quotes.

<!-- write escape.tqfolder -->
```
+AA {
  +A {}
  $escape(AA);
  +B {}
}
+BB {
  +C {}
  $escape(AA);
  +D {}
}
```
The behaviour can be tested by pasting either of the above in the file 'escape.tqfolder' and calling
<!-- console -->
```
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('escape.tqfolder').printContents(':r2')"
```

The argument of escape can be prefixed by an `!` which inverts the logic.

The real power of this command becomes obvious when it is combined with `for`
loops or wildcards in which case the current  folder name is not hard coded but can be
different in each iteration.

## $for
The `for` command iterates over numbers in a given range. The syntax is as
follows:
```
$for(index, start, stop) { ... };
```

Here `index` is the running variable. The loop body (here `...`) is executed
with `index=start` until `index=stop-1`, so the stop value is not included.
Inside the loop body, the tag `$(index)` is available to use the running index.

> Task: Load the following example and print the structure to see if it looks like what
  you expect.

<!-- write for.tqfolder -->
```
+A {
  $for(i, 10, 20) {
    +B_$(i) {
        <tag=$(i)>
    }
  }
}
```

<!-- console
```
$ echo "For"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('for.tqfolder').printContents(':r2')"
```
-->

## $import
The `import` command allows you to import a TQFolder structure from a root file.
Assumed that you still have the `tutorial.root` from above, we could import this
animal tree into a larger structure.

<!-- write import.tqfolder -->
```
+BiologicalTaxonomy {
  $import("tutorial.root:animal_tree");
}
```
<!-- Let us check that the syntax above is correct -->
<!-- console
```
$ echo "import"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('import.tqfolder').printContents(':dtr2')"
```
-->

## $include
The `include` command is similar to the `import` function. The `include` command
allows you to include a TQFolder structure from a text
file as if the file's content is copy-pasted into the TQFolder
definition. The syntax for of the function is:

```
$include("path_to/file");
```

> Task: A colleague wants to contribute to the animal tree and wrote the
  definition of a horse. Save the following snipped to the file `horse.tqfolder`
  and import it in the mammals folder.

<!-- write horse.tqfolder -->
```
+Horse {
    <vegetarian=true, population=3300000, number_of_legs=4>
}
```

<!-- write include.tqfolder 
```
$include("horse.tqfolder");
```
-->

<!-- Let us check that the syntax above is correct. -->
<!-- console
```
$ echo "Include"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('include.tqfolder').printContents(':dtr2')"
```
-->


## $modify
The `modify` command is able to modify existing tags. The following table
summarizes the argumets of `modify`. The arguments `tag`, `operator` and `value`
are mandatory.

| Argument | Description |
|----------|-------------|
| `tag`    | The name of the tag which should be modified. |
| `operator` | Can be either of `+-*/=`. This determines the operation that is performed between the existing tag's value and the one specified by the `value` argument.| 
| `value` | The value used with the operator and the tag's value. |
| `path`  | Path to the sample folder on which the modification should be performed. |
| `filter` | The optional argument `filter` can be `s` or `sf` to limit the application to sample or sample folders, respectively. |
| `create` | If `true`, creates the non-existent tags. Existing tags are not touched. |
| `override` | If `true` and `create=true`, creates non-existent tags, existing tags are overwritten. |
| `force` | If `true`, allows you to overwrite the type of an existing tag. |

> Task: The definition of *legs* has been changed. The tag `number_of_legs` should be
  the number of legs on one side of the animal. This means a dog has two legs,
  an eagle has one leg. Use the `filter` method to apply this change.

<!-- console
```
$ cp animals_advanced.tqfolder modify.tqfolder
```
-->

<!-- append modify.tqfolder
```
$modify(path="?/?/?",tag="number_of_legs",operator="/",value=2);
```
-->

<!-- console
```
$ echo "Modify"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('modify.tqfolder').printContents(':dtr2')"
```
-->

## $print
The print method is similar to the `printContents` method. It is executed on the
TQFolder where it is written with all the folders defined so far. The print
method takes an optional argument. The optional argument specifies the print
options (the part after the `:` on `printContents`), for example `d`, `t` or
`r1` , `r2`, ...

For example, the following snipped print the `AA` folder before `B` is added. 

<!-- write print.tqfolder -->
```
+AA {
  +A {}
  $print();
  +B {}
}
```

<!-- Let us check that the syntax above is correct >>
<!-- console -->
```
$ echo "Print"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('print.tqfolder')"
```

> Task: Add a `print` statement such that the animals folder is printed before
  the reptiles are added. The printout should contain all tags an all sub
  folders (except the ones of the reptiles).

## $printline
The `printline` function print its argument onto the command line when the text file is loaded. This
is useful, for example, to debug complex TQFolder definitions.

<!-- write printline.tqfolder -->
```
+AA {
  $printline("Building AA");
  +A {}
  $printline("Done buildig AA");
}
```

Let us see it in action
<!-- console -->
```
$ echo "Printline"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('printline.tqfolder')"
```

## $replace

The `replace` method lets you expand `$(tagName)` placeholders inside a
tag's value. For example, if we have the tag
```
<habitat="Lives in $(location)">
```
which uses another tag `location` inside a tag, `replace` can be used to
replace `$(location)` with a string.

The function takes a positions argument which determines which tags should be
replaced. It consists of a folder filter string and a tag filter string
separated by a colon. If this first argument is `"*:*"`, the replacement is
applied to all folders an all tags. The first argument is followed by an
arbitrary number of named arguments which are the key-value pairs for the
substitution.

```
$replace("folderFilter:tagFilter", tag1="value", tag2="value", ...);
```

> Task: Add the above habitat tag to all animals using a command from above. 
  Then use `replace` to set the following locations: Mammals -> Geneva, Birds -> R1, Reptiles -> Australia.

<!-- write replace.tqfolder
```
$modify(create=true,path="?/?/?",tag="habitat",operator="+",value="Lives in $(location)");
$replace("Animals/Mammals/?:habitat", location="Geneva");
$replace("Animals/Birds/?:habitat", location="R1");
$replace("Animals/Reptiles/?:habitat", location="Australia");
```
-->

<!--Let us check that the syntax above is correct. -->
<!-- console
```
$ echo "Replace"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('replace.tqfolder').printContents(':dtr4')"
```
-->

## $write
The `write` command lets your write parts of the TQFolder tree to a text file.
The method takes the mandatory argument `filename` which is the destination text file. 
The optional argument `target` is the path to the TQFolder you
would like to export. If this is omitted, the current folder is exported.

The following example illustrates how to export the mammals folder.

<!-- write write.tqfolder  -->
```
+Animals {
  +Mammals {
    <skin="fur">
    <vegetarian=false>
    +Rabbit {
      <vegetarian=true>
      <population=8.5e8>
    }
    +Cat {
      <population=6e8>
    }
  }
  $write(filename="write_out.txt");

  +Birds {
    +Mockingbird {
      <vegetarian=false>
      <population=3.2e7>
      <skin="feather">
    }
  }
}
```

<!-- Let us check that the syntax above is correct. -->
<!-- console
```
$ echo "Write"
$ python -c "import QFramework; QFramework.TQFolder.loadFromTextFile('write.tqfolder')"
```
-->

After loading the above TQFolder, the file `write_out.txt` contains all mammals.

<!-- console_output
```
$ cat write_out.txt
# -*- mode: tqfolder -*-
+Mammals {
	<skin = "fur", vegetarian = false>
	+Rabbit {
		<vegetarian = true, population = 850000000>
	}
	+Cat {
		<population = 600000000>
	}
}
```
-->
