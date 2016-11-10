davinci
=======
[![Build Status](https://travis-ci.org/robwink/davinci.svg?branch=master)](https://travis-ci.org/robwink/davinci)

Davinci is a hyperspectral math engine, developed for JPL under
contract #959904.  You can always obtain the latest version of davinci from
[davinci.asu.edu](davinci.asu.edu)

## Compiling
Davinci requries a C99 compliant compiler.  A configuration script has been
included to make compilation as straight forward as possible.  In
most instances you should be able to run configure, and then run make:

	% ./configure
	% make
	% make install

If that doesn't work check [the wiki page](http://davinci.asu.edu/index.php?title=Compiling_Davinci)
for OS specific details.


## Documentation
Documentation can be found on the wiki at [davinci.asu.edu](davinci.asu.edu).

In addition the file dv.gih, which is distributed with davinci and can be
accessed from within the interpreter, documents all the functions and more.

