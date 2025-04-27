Style Files
=======================
Style files allow to add various options regarding the visualization
of analysis results (mostly histograms) such as default colors, line and
fill styles, ... on a SampleFolder

Usage
-----------------------
Stle files are technically identical to Patch files. Due to the different 
purpose, however, it makes sense to split these in order not keep purely
stylistic settings appart from settings that have an actual influence on the
analysis result.

The vast majority of such style settings can be applied using the syntax
```
<myOption="someStringValue", myOtherOption=42.> @bkg/?/Zjets;
```
which would place to 'tags' on all parts of the SampleFolder structure matching
```bkg/?/Zjets```.



Advanced
-----------------------
For a more complete documentation of the TQFolder syntax used in Style files,
please see the README for ['Patch Files'](../../patches/README.md)
