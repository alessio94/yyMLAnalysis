TQFolder Patches
=========================

TQFolder Patch Files allow to place additional information on a SampleFolder
structure in the form of additional tags. They also provide a way to modify the
SampleFolder structure itself, e.g, by moving, copying, deleting, or creating
TQ(Sample)Folders and TQSamples.

Applying these kind of files as 'patches' or 'patch files' usually implies 
that they are applied to the SampleFolder structure of an analysis. The syntax,
however, can be used to modify any TQFolder structure.


Syntax: Folder Paths
--------------------
The path syntax use in TQFolder files mostly follows the pattern one knows from
most shells on Linux/MacOS. Yet, some specifics are listed in the following:

* ```this/is/a/relative/path```: like on a shell this is a relative path
* ```/this/is/an/absolute/path```: leading '/' is interpreted as an absolute 
    path, i.e., w.r.t. the root node of a TQFodler structure.
* ```path/with/wi\*card/matching```: matches any folder whoes path starts as 
    'path/with', the name of the folder at the third level matches 'wi\*cards'
    and the fourth level folder is called 'matching'. The '*' wildcard, here,
    matches any number of characters within a single folders name.
* ```path/with/wil?card/matching```: same as previous case but the '?' wildcard
    only matches a single character
* ```path/with/?/matching```: here, the '?' wildcard character is isolated. The 
    expression therefore matches any folder at the third level.
* ```path/with/\*/matching```: here, the '\*' wildcard character is isolated. It
    matches any number of folders inside 'path/with'. The paths of the folders
    matched by the full expression still need to end with a folder named 
    'matching'.

In summary regarding wildcards: a questionmark '?' matches a single object, the 
star any number of objects. If the character in question is used on its own as a 
folder layer in a path the objects matched are entire folders, otherwise the 
objects matched are characters in a single folders name.

Names of TQFolders are also more restrictive than those of folders on a file 
system. Allowed characters are 
```
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.
```
Please take note that especially '-' and '+' are excluded as they have a special
function for SampleFolders.

Usage: Creating new TQFolders
--------------------
New instances of TQFolder (please note: NOT TQSampleFolder) can be created using
```
+myNewFolder;
```
This will create a new subfolder (instance of TQFolder) inside the current
TQFolder. If a folder with the name 'myNewFolder' already exists inside the 
current folder, nothing is done. Alternatively, one can add a block after the 
creation of a new folder:
```
+myNewFolder {
  #something else (this is a comment)
}
```
Everything inside the block delimited by '{' and '}' is evaluated from within
'myNewFolder'. Hence, in that context 'myNewFolder' is the 'current' folder.

Multiple layers of folders can be created in the same way:
```
+myNewFolder/myNewSubFolder;
# or:
+myNewFolder/myNewSubFolder {
  #do something inside 'myNewSubFolder'
}
```

In order to create new instances of TQSampleFolder or TQSample one can 
explicitly  specify the type of folder using the ```$create``` command:
```
$create(path='my/folder/path/to/create', type='sf'); # or type='samplefolder'
$create(path='my/other/path/to/create', type='s'); # or type='sample'
```
This will create all folders in the given path that do not exist, yet. The
specified type of folder (instance of TQSampleFolder or TQSample) is only taken
into account if the respective folder does not yet exist, the type of existing 
folders is not changed.


Usage: Adding Tags to Existing Folders
--------------------
One or multiple tags (pairs of 'key' and 'value') can be set on a TQFolder.
Depending on the use case either of the two following examples can be more 
convenient than the other:
```
#place the two tags 'myKey' (string valued) and 'myOtherKey' (double valued) at some/path:
<myKey="myStringValue", myOtherKey=42. ,someBool=true> @some/path; 
#equivalently:
@some/path {
  <myKey="myStringValue", myOtherKey=42. ,someBool=true>
}
#available types of tags: int, double, string, bool
```
The paths involved here can make use of wildcards as described above.

Please note that this will only deposit the tags and their values at the 
specified location. The interpretation of the repective tag is then up to some
(at this point unspecified) part of the analysis code. For a (almost complete)
list of tags, please see the [TagDocumentation](https://atlas-caf.web.cern.ch/tags.html).

Usage: Interpretation in a Different Folder
-------------------

The ```@some/path { ... }``` syntax above can not only be used to set tags on
subfolders but is of more general use: it allows to move the interpretation
of whatever is contained in the { ... } block into the scope of the specified
folder. If the path expression contains wildcards this can also be used to
perform a modification several times, e.g.:
```
+foo;
+bar;
@? {
  +buzz; #will create a subfolder 'buzz' inside 'foo' and inside 'bar'
}

```


Usage: Advanced Commands
-------------------
For more complex operations, additional commands are available
* Removing a folder:
    ```
    $delete("somePath/someFolder!"); #deletes someFolder inside 'somePath'.
                                   #the trailing '!' enforces the deletion even
                                   #if 'someFolder' is not empty at this point
                                   #(deleting all its content as well)
    $delete("somePath/someFolder-!"); #same but will also delete 'somePath'
                                    #(and its parent folders) recursively if it 
                                    #becomes empty by the previous deletion 
                                    #starting with the deletion of 'someFolder'
    ```
    
    Wildcards in folder names/paths are supported.
* Moving a folder to a different location:
    ```
    $move("oldPath/oldFolderName>>newPath/::newFolderName);
    ```
    
    The part after the '::' is the new name of the moved folder. If omitted the 
    name will be unchanged.
* Copying a folder:
    ```
    $copy("oldPath/oldFolderName>>newPath/::newFolderName);
    ```
    
    This works analogously to '$move' but creates a copy of the source folder
    and all of its content (including subfolders).

* Modifying a tag (advanced):
    The ```$modify``` command allows to perform more sophisticated changes to
    the values of tags than just setting/overriding them:
    ```
    $modify(tag='myKey', operator='=', value=1.25, path='sig/em//*', 
          filter='s', create=true, override=false);
    # creates a tag with name 'myKey' on all folders 
    # - matching the specified path and 
    # - are of the type indicated by the filter 
    # (here: TQSample; note: a TQSample is always also a TQSampleFolder!)
    # The tag will be assigned the value 1.25 in this example.
    # Options with default values are path="" (defaults to current folder), 
    # filter="" (defaults to no filter), create=false (only existing tags will be 
    # modified by default), 
    # override=true/false (existing tags are overridden by default if creation 
    # of new tags is disabled), 
    # force=false (if set to true existing tags of a different type but same 
    # name will be replaced).
    # Required arguments are tag, operator and value. 
    # Valid operators depend on the type of the value argument:
    # string: '=' (set), '+' (append) '-' (remove from end if matching)
    # double/int: '=' (set), '+', '-', '*', '/' (newTagValue = oldTagValue value)
    # bool: '=' (set), '!=', '==' (compare tag to value argument), '&&', '||' 
    # (combine tag and value argument)
    # for numeric, non-boolean values the operator can be prefixed with '!' in 
    # order to exchange the roles of old (existing) and new tag (passed to $modify). 
    # for string tags prefixing '!' to the operators '+' and '-' prepends to / 
    # removes from the front of the existing tag 
    ```

* Creating folders of a specific type (TQSampleFolder or TQSample): see [above](usage:-creating-new-tqfolders)

* Printing of the current folder can be triggered during the interpretation of 
    patch files using ```$print("r2dt");```. The argument to the ```$print```
    command is the same as if calling ```TQFolder::print(const TString& options="")```
    
* For printing some arbitrary text to the command line whenever some part of a
    patch file is read one can use ``` $printLine("some arbitrary text here");```.
    This can be particularly useful when patch files are getting more complex
    and one wants to do some debugging.
    
* A folder structure can be written to a text file (in TQFolder syntax) using
    ```
    $write("myfile.txt"); #internally calls TQFolder::exportToTextFile
    ```
    
* Another TQFolder patch file can included in the current scope using
    ```
    $include("someOtherFile.txt");
    ```
    
* An existing TQFolder can be imported from a ROOT file using
    ```
    #importing folder named 'nameOfFolderInSideTheFile' from
    #file with relative path 'path/to/someFile.root' :
    $import("path/to/someFile.root:nameOfFolderInsideTheFile");
    ```

* To skip the rest of the current block one can use 
    ```
    @foo {
      <tagA="this will be applied">
      $escape();
      <tagB="this will not be applied">
    }
    ```
    
    If the argument of ```$escape()``` is not empty the interpretation of the 
    remaining lines of a block is only skipped if the name of the name of the 
    current folder matches the argument, e.g., 
    ```
    @foo,bar {
      <tagA="applied to all folders">
      $escape("f*");
      <tagB="only applied to 'bar' ">
    }
    ```
    
* Simple loops can be created using
    ```
    $for(i,0,100) { #will execute the following block for i=0 to i=99
                    #(i.e., the upper bound is not included!)
                    #the current value of 'i' can be used through $(i)
      +folder_$(i) {
        <someTag="this is folder $(i)">
      }
    }
    ```
    
    

    
    
    
    
    
    



















