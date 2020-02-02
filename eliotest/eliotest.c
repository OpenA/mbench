/*
   Getting from:

   Roy Longbottom's PC Benchmark Collection
   http://www.roylongbottom.org.uk/openmp%20mflops.htm

   original source code: http://www.roylongbottom.org.uk/linux_openmp.tar.gz
   @ test.c

   Code modifed by OpenA special for https://github.com/OpenA/mbench
*/
#include "../mbench.h"

int main()
{
	unsigned int i, j;
	const size_t length = 125000;
	double data[length];
	char timeday[30];

	for (i = 0; i < length; i++) {
		data[i] = (double)i;
	}

	getLocalTime(&timeday);
	printf("\n elio Test 0.1 - %s", timeday);

	double secs = 0.0;
	double x    = 0.5;
	double y    = 0.5;
	double z    = 2.0;
	double t    = 0.49999975;

	start_time();
	for (i = 0; i < 10000; i++)
	{
		for (j = 0; j < 333; j++)
		{
			x = t * atan(z * sin(x) * cos(x) / (cos(x + y) + cos(x - y) - 1.0));
			y = t * atan(z * sin(y) * cos(y) / (cos(x + y) + cos(x - y) - 1.0));
		}
	}
	secs = end_time();

	printf("\n  Time %20.10f secs %12.4f\n\n", secs, y);
	return 0;
}
