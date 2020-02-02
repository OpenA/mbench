/*
   Getting from:

   Roy Longbottom's PC Benchmark Collection
   http://www.roylongbottom.org.uk/openmp%20mflops.htm

   original source code: http://www.roylongbottom.org.uk/linux_openmp.tar.gz
   @ openmp/OpenMPMFLOPS.c

   Code modifed by OpenA special for https://github.com/OpenA/mbench
*/
#define _VERSION_ "1.0"

#include "../mbench.h"
#include <malloc.h>
#include <mm_malloc.h>

float *x_cpu; // Pointer to CPU arrays
int repeats = 2500;
int words   = 100000;

void triadplus2(int n, float a, float b, float c, float d, float e, float f, float g, float h, float j, float k, float l, float m, float o, float p, float q, float r, float s, float t, float u, float v, float w, float y, float *x)
{
	int i;
#ifdef _OMP_
#pragma omp parallel for
#endif
	for (i = 0; i < n; i++)
		x[i] = (x[i] + a) * b - (x[i] + c) * d + (x[i] + e) * f - (x[i] + g) * h + (x[i] + j) * k - (x[i] + l) * m + (x[i] + o) * p - (x[i] + q) * r + (x[i] + s) * t - (x[i] + u) * v + (x[i] + w) * y;
}

void triadplus(int n, float a, float b, float c, float d, float e, float f, float *x)
{
	int i;
#ifdef _OMP_
#pragma omp parallel for
#endif
	for (i = 0; i < n; i++)
		x[i] = (x[i] + a) * b - (x[i] + c) * d + (x[i] + e) * f;
}

void triad(int n, float a, float b, float *x)
{
	int i;
#ifdef _OMP_
#pragma omp parallel for
#endif
	for (i = 0; i < n; i++)
		x[i] = (x[i] + a) * b;
}

char runTests(char part)
{
	static const float x = 0.999950f;
	static const float a = 0.000020f;
	static const float b = 0.999980f;
	static const float c = 0.000011f;
	static const float d = 1.000011f;
	static const float e = 0.000012f;
	static const float f = 0.999992f;
	static const float g = 0.000013f;
	static const float h = 1.000013f;
	static const float j = 0.000014f;
	static const float k = 0.999994f;
	static const float l = 0.000015f;
	static const float m = 1.000015f;
	static const float o = 0.000016f;
	static const float p = 0.999996f;
	static const float q = 0.000017f;
	static const float r = 1.000017f;
	static const float s = 0.000018f;
	static const float t = 1.000018f;
	static const float u = 0.000019f;
	static const float v = 1.000019f;
	static const float w = 0.000021f;
	static const float y = 1.000021f;

	unsigned int i;

	// calculations in CPU
	if (part == 0)
	{
		for (i = 0; i < repeats; i++)
			triad(words, a, x, x_cpu);
		return 2;
	}
	else if (part == 1)
	{
		for (i = 0; i < repeats; i++)
			triadplus(words, a, b, c, d, e, f, x_cpu);
		return 8;
	}
	else if (part == 2)
	{
		for (i = 0; i < repeats; i++)
			triadplus2(words, a, b, c, d, e, f, g, h, j, k, l, m, o, p, q, r, s, t, u, v, w, y, x_cpu);
		return 32;
	}
}

// main program that executes in the CPU
int main(int argc, char *argv[])
{
	char opt, timeday[30];

	while ((opt = getopt(argc, argv, "w:r:")) != -1)
	{
		switch (opt) {
			case 'w': words   = (int)(atol(optarg)); break;
			case 'r': repeats = (int)(atol(optarg)); break;
			case 'h':
				printf("\n\
  OpenMP MFLOPS Benchmark v"_VERSION_" ("_MARCH_" Bit)\n\n\
    -w {n} ; Number of words in arrays (def 100000)\n\
    -r {n} ; Number of repeat passes   (def 2500)\n\
    -h     ; Show this Help\n\
\n");
				exit(0);
				break;
		}
	}

	getLocalTime(&timeday);

#define _OP1_ " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n"
#define _OP2_ "              OpenMP MFLOPS Benchmark v"_VERSION_" ("_MARCH_" Bit)\n\n"
#define _OP3_ "   compiled with "_COMPILER_"\n"

#define _PARAMS_ "\n   %s\n\
    repeats : %i\n\
      words : %i\n\
        cpu : %i\n\n"

#define _TH1_ " |    4 Byte  Ops/   Repeat    Seconds   MFLOPS       First   All\n"
#define _TH2_ " |     Words  Word   Passes                         Results  Same\n"
#define _TH3_ " + — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — —\n"

	printf(_OP1_""_OP2_""_OP3_""_OP1_);
	printf(_PARAMS_, timeday, repeats, words, omp_get_max_threads());
	printf(_TH1_""_TH2_""_TH3_);

#define _RESULT_ " | %9d %5d %8d %10.6f %8.0f %10.6f   %s\n"

#define _ERROR_ " ERROR - At least one first result of 0.999999 - no calculations?\n\n"

	const unsigned int S_WORDS   = words;
	const unsigned int S_REPEATS = repeats;
	const float        NEW_POINT = 0.999999f;

	unsigned int i;
	unsigned char p, n;

	float errors[2][10];
	float secs = 0.0;
	
	int erdata[5][10];
	int count1 = 0;

	bool isNotCalc = false;

	for (p = 0; p < 3; p++)
	{
		words   = S_WORDS;
		repeats = S_REPEATS;

		for (n = 0; n < 3; n++)
		{
			size_t size_x = words * sizeof(float);

			// Allocate arrays for host CPU
			x_cpu = (float *)_mm_malloc(size_x, 16);

			if (x_cpu == NULL) {
				errcall("ERROR WILL EXIT");
			}

			// Data for array
			for (i = 0; i < words; i++) {
				x_cpu[i] = NEW_POINT;
			}

			start_time();
			char  opwd = runTests(p);
			float secs = (float)end_time();

			float fpmops = (float)words * (float)opwd,
			      mflops = (float)repeats * fpmops / 1000000.0f / secs;

			float one = x_cpu[0];
			bool same = !(isNotCalc = one == NEW_POINT);

			for (i = 1; i < words; i++)
			{
				if (!(same = one == x_cpu[i]))
				{
					if (count1 < 10)
					{
						errors[0][count1] = x_cpu[i];
						errors[1][count1] = one;
						erdata[0][count1] = i;
						erdata[1][count1] = words;
						erdata[2][count1] = opwd;
						erdata[3][count1] = repeats;
						count1++;
					}
				}
			}

			// Print results
			printf(_RESULT_, words, opwd, repeats, secs, mflops, x_cpu[0], (same ? "Yes" : "No"));
			
			// Cleanup
			_mm_free(x_cpu);

			words   = words * 10;
			repeats = repeats <= 10 ? 1 : (int)(repeats / 10);
		}
		printf("\n");
	}

	if (isNotCalc)
	{
		printf(_ERROR_);
	}
	if (count1 > 0)
	{
		printf(" First Unexpected Results\n");
		for (i = 0; i < count1; i++) {
			printf(" %9d %5d %8d word %9d was %10.6f not %10.6f\n",
					erdata[1][i], erdata[2][i], erdata[3][i], erdata[0][i], errors[0][i], errors[1][i]);
		}
		printf("\n");
	}
	return 0;
}
