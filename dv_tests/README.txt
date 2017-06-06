This directory contains tests to validate davinci output.
Tests are organized into directories as files matching pattern "test-*".
Files are named descriptively so that they convey the test being run
to the user. Consorted effort has been made to ensure a single file contains
a single test and that is the pattern to follow going forward.

To run tests, cd into this (dv_tests) directory.

Add davinci you want to run tests with to the PATH, e.g.
	export PATH=/my/davinci/build/.libs:$PATH

Don't forget to set the appropriate library path, e.g.
	export LD_LIBRARY_PATH=/my/davinci/build/.libs:$LD_LIBRARY_PATH

Then run the 32-bit driver:
	davinci -q -f test_driver_32

If one is interested in HDF5 tests only, run it as follows:
	davinci -q -f test_driver_32 io/hdf5

where, "io/hdf5" is the sub-directory under "dv_tests" that contains
the HDF5 tests.

There is also a 64-bit driver, called test_driver for testing on 
the hugemem cluster nodes:
	davinci -q -f test_driver 

Data needed by various tests is stored in "data" sub-directory of
"dv_tests".

Each test could exit with the following return codes:
	0  success
	1  falure
	99 test skipped

