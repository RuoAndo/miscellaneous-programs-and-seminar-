// #ifndef _H_TIMER
#define _H_TIMER

#include <stdio.h>
#include <sys/time.h>

inline
unsigned long long gettimeval(void) {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return ((unsigned long long)tv.tv_sec)*1000000+tv.tv_usec;
}

inline
void begin_timer(unsigned int *begint) {
    *begint = (unsigned int)gettimeval();
    return;
}

inline
unsigned int stop_timer(unsigned int *begint) {
    unsigned int stopt = (unsigned int)gettimeval();
    return (stopt>=*begint)?(stopt-*begint):(stopt);
}

// #endif _H_TIMER
#define print_timer(te) {printf("time of %s:%f[msec]\n", #te, te*1.0e-3);}

