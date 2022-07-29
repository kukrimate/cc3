// #if with the first branch evaluated
#if 2+2
yep
#elif 1
nope1
#else
nope2
#endif

// #if with the second branch evaluated
#if 0
nope1
#elif 1
yep
#else
nope2
#endif

// #if with the else evaluated
#if 0
nope1
#elif 0
nope2
#else
yep
#endif
