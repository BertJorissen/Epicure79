# Hitmap Tests

### Usage
For each class there is a script (usually called testsMPI.sh)
which can be used to execute all tests at once and view the global results.

`$ ./testsMPI.sh [-d] [-v] [.c file]`

If you include a C file as parameter only that test will be performed; otherwise all
executable files finished as ".test" will.

To execute a test individually you only have to call the executable

`$ ./sigIntersect.test`

Except in HitLayout, where you should execute it with `mpiexec` to have more than
one process.


### Name convention
If you want to create a new test, you must call it &lt;name&gt;Test.c, and
its executable must be called &lt;name&gt;.test. For example:

| C File            | Executable File   |
|:-----------------:|:-----------------:|
| layoutCmpTest.c   | layoutCmp.test    |
| sigIntersectTest.c| sigIntersect.test |

Not using this name convention will cause that your tests won't be
executed when calling the scripts.


### Debug mode
In most tests there is a flag you can set to view more detailed information about the
cases in the test and their individual result instead of the global result.
To see the debug mode compile the tests using

`$ make debugmode`

