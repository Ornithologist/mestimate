# mestimate
mestimate is a simple C program that makes reasonable guess about a computer's memory

## Build & Run
To build, run `make check` or simply `make all`.

To clean up the previous build, run `make clean`

## Estimation Strategy
This program makes use of standard C library function _malloc()_ and _memset()_ to manipulate memory.

The program makes request for memory of size 20 * PAGE_SIZE. After successful allocation, it will immediately call _memset()_ to fill integer 0 into each byte in the requested space. It will then repeat the above 2 steps in an endless loop.

Before the system reaches the maximum memory capacity, each iteration would take nearly equal duration. Once trashig began, the next iteration would complete in much slower speed. The program uses an arbitrary number *TRIGGER_RATIO* (of value 2) to estimate this: once the actual duration taken divided by the expected duration is bigger than *TRIGGER_RATIO*, the program guesses trashing had already began.

The program keeps counting its trashing guesses and tries to confirm its guess by checking if its guess count is larger than *TRIGGER_BUFFER* (of value 100). If it does, then trashing can be confirmed, and the program would print out its estimation of system memory and terminate. If not, the program will carry on looping until the next trashing guess occurs.

The final estimation is calculated by multiplying the number of non-trashing iterations with (20 * PAGE_SIZE).

The program has been tested on ubuntu precise 64 distribution and MacOS High Sierra.
