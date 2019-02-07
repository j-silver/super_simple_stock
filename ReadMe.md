**Test for an application to JP Morgan Chase** (which has never received a feedback... as it usually happens)


* Requirements: Linux; a C++17 compiler.


* Linux is necessary because entropy from /dev/random/ is used to initialize the random number generator. If not available, any seed provided to the mt19937_64 engine will do.


* There are some (many!) simplifications: prices, last dividends, par values are *unsigned* because they are assumed to be provided in pennies and they are only initialised, never calculated (so no need to use a *double*). There are a couple of overloads, and some exceptions are raised when functions are fed with bad input (e.g. zero prices or zero rates when calculating the P/E ratio).




