#ifdef __cplusplus
extern "C" {
#endif

#ifdef _OMP_
	#include "omp.h"
#else
	extern int omp_get_max_threads(void) { return 1; }
#endif

#ifndef _MARCH_
	#define _MARCH_ "32"
#endif

#ifndef _COMPILER_
	#define _COMPILER_ "undefined"
#endif

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern void getLocalTime(char (*timeday)[]);
extern void errcall(char msg[]);
extern double start_time(void);
extern double   end_time(void);

#include <sys/time.h>
#include <time.h>

void getLocalTime(char (*timeday)[])
{
	time_t t = time(NULL);
	sprintf(*timeday, "%s", asctime(localtime(&t)));
}

double getSecs()
{
	static struct timespec tp1;
	clock_gettime(CLOCK_REALTIME, &tp1);
	return (tp1.tv_sec + tp1.tv_nsec / 1e9);
}

double startSecs = 0.0;

double start_time()
{
	return (startSecs = getSecs());
}

double end_time()
{
	double secs = getSecs() - startSecs;
	unsigned int millisecs = (int)(1000.0 * secs);
	return secs;
}

void errcall(char msg[])
{
	char g;
	printf("\n %s\n\n Press Enter\n", msg);
	g = getchar();
	exit(0);
}

#ifdef __cplusplus
};
#endif
