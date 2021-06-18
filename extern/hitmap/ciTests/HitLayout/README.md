# HitLayout Tests

### Usage
To perform the tests you must execute the script

`$ ./testsMPI.sh [-d] [-v] [.c file]`

If you include a C file as parameter only that test will be performed; otherwise all
executable files finished as ".test" will.

Of course, you can execute them individually with `mpiexec`, but **keep in mind** that most of
these tests have been made to be correct with the number of process specified, so executing
them with a random number may give wrong results.


### Specifying number of processes
If you want the test to be executed with a specific number of processes,
you must declare it with a comment line in the C file as below:

`/* #numprocs: 5 7 */`  &nbsp;This test will be executed with 5 and 7 processes

`/** #numprocs: 1 */` &nbsp;&nbsp;&nbsp;This test will be executed only with 1 process

If you don't include this line, the test will be executed with the default
number of processes: 1, 4, 5 and 18.