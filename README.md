# gecodeMCP
## Using Gecode to solve musical constraints problems

In this repository, you will find all the code described in the chapter 5 ("_Using Gecode to solve Musical Constraint Problems_") of C. Truchet (Ed.), _Constraint Programming in Music_, Wiley. (2011)
## installation
	'make all'
## library
- compile slm-models framework
    - in xcode : 
    - build target slm_models (debug)
## max object
- compile mgeco max object
   - in xcode :
   - build target mgeco-nobach (debug)
## max package
(not yet)
## bach object
(not yet)

## still to do
chord sorting : timelimit + simpletest crash
interpolm : script does not parse arguments


## news

## note : porting to gecode 6.1

icl->ipl

string char* ->  const char*

The Boolean share argument during cloning is not any longer needed. (major)

Removed the deprecated classes MinimizeSpace and MaximizeSpace (use IntMinimizeSpace and IntMaximizeSpace instead). (minor)

The integer consistency level (IntConLevel) has been renamed to integer propagation level (IntPropLevel). Additional levels for speed, memory, basic, and advanced propagation have been added. See Modeling and Programming with Gecode for details. (major)

The commandline option -icl (for integer consistency level) has been replaced by -ipl (for integer propagation level). All new levels can be specified by a comma separated list as argument to -ipl. (major)

Removed the deprecated classes MinimizeScript and MaximizeScript (use IntMinimizeScript and IntMaximizeScript instead). (minor)

IntArgs now support standard initializer lists but do not any longer support variable argument lists (as the combination is really confusing). Now one can write IntArgs c({1,2,3}) for example. (major)
	update Makefile
	deployment on gitlab
