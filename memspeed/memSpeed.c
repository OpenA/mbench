/*
   Getting from:

   Roy Longbottom's PC Benchmark Collection
   http://www.roylongbottom.org.uk/openmp%20mflops.htm

   original source code: http://www.roylongbottom.org.uk/linux_openmp.tar.gz
   @ memspeed/memory_speed.c

   Code modifed by OpenA special for https://github.com/OpenA/mbench
*/
#define _VERSION_ "4.1"

#include "../mbench.h"
#include <malloc.h>
#include <mm_malloc.h>

#ifdef _AVX_
	#define _EXTLIST_ "AVX "
#else
	#define _EXTLIST_ "SSE "
#endif

#ifdef _INT64_
	typedef long long IntM_t;
#else
	typedef int IntM_t;
#endif

double secs = 0, gsecs = 0;
int n1;

double *xd, *yd;
float  *xs, *ys;
IntM_t *xi, *yi;

bool checkTime()
{
	static const double o1 = 0.1;
	static const double o2 = 0.0125; // o1 / 8.0
	static const double o3 = 0.125;  // o1 * 1.25

	bool timeOK = (secs = end_time()) < o1;

	if (secs < o2)
	{
		n1 *= 10;
	}
	else if (timeOK)
	{
		n1 = (int)(o3 / secs * (double)n1 + 1);
	}
	gsecs += secs;
	return timeOK;
}

int main()
{
	int i, m, j, nn;

	int allocMem[23] = { 2 }; // KB two arrays

	for (i = 1; i < 23; i++)
	{
		allocMem[i] = allocMem[i - 1] * 2;
	}

	static const int passes[23] = {
		250        , //      4 KB
		500        , //      8 KB
		1000       , //     16 KB
		2000       , //     32 KB
		4000       , //     64 KB
		8000       , //    128 KB
		16000      , //    256 KB
		32000      , //    512 KB
		64000      , //    1 MB
		128000     , //    2 MB
		256000     , //    4 MB
		512000     , //    8 MB
		1024000    , //   16 MB
		2048000    , //   32 MB
		4096000    , //   64 MB
		8192000    , //  128 MB
		16384000   , //  256 MB
		32768000   , //  512 MB
		65536000   , // 1024 MB
		131072000  , // 2048 MB
		262144000  , // 4096 MB
		524288000  , // 8192 MB
		1048576000   //16384 MB
	};

	int ramKB = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1024;

	char runs = (
		ramKB <= 14000    ? 10 : // 2 MB
		ramKB <= 30000    ? 11 :
		ramKB <= 60000    ? 12 :
		ramKB <= 120000   ? 13 :
		ramKB <= 250000   ? 14 :
		ramKB <= 500000   ? 15 :
		ramKB <= 1000000  ? 16 :
		ramKB <= 1500000  ? 17 :
		ramKB <= 3500000  ? 18 :
		ramKB <= 7500000  ? 19 :
		ramKB <= 15500000 ? 20 :
		ramKB <= 31500000 ? 21 :
		ramKB <= 63500000 ? 22 :
	23);

	long long useMem = allocMem[runs - 1];

	xd = (double *)_mm_malloc(useMem * 1024, 16);
	if (xd == NULL)
	{
		errcall("ERROR WILL EXIT");
	}

	yd = (double *)_mm_malloc(useMem * 1024, 16);
	if (yd == NULL)
	{
		_mm_free(xd);
		errcall("ERROR WILL EXIT");
	}

#define _HEAD_ "\n = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n\
              Memory Reading Speed Test %s ("_EXTLIST_""_MARCH_" Bit) \n\n\
        compiled with "_COMPILER_"\n\n\
                              Copyright (C) 2010, Roy Longbottom\n\
 = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n\
  %s\n  max threads : %i\n\n\
 + — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — —\n\
 |  Memory |  x[m]=x[m]+s*y[m]  |   x[m]=x[m]+y[m]   |     x[m]=y[m]\n\
 + — — — — + — — — — — — — — — —+— — — — — — — — — — + — — — — — — — — — —\n\
 |  KBytes   Doubl  Singl  Int"_MARCH_"  Doubl  Singl  Int"_MARCH_"  Doubl  Singl  Int"_MARCH_"\n\
 |    Used    MB/S   MB/S   MB/S   MB/S   MB/S   MB/S   MB/S   MB/S   MB/S\n\
 + — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — —\n"

#define _ENDLINE_ " + — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — — —\n\
  end time - %lf sec.\n\n"

	char timeday[30];
	getLocalTime(&timeday);
	printf(_HEAD_, _VERSION_, timeday, omp_get_max_threads());

	float *xs = (float *)xd;
	float *ys = (float *)yd;

	IntM_t *xi = (IntM_t *)xd;
	IntM_t *yi = (IntM_t *)yd;

	int kd, ks, ki, mem;
	float sums;
	IntM_t sumi;
	double sumd, mbpsd, mbpss, mbpsi, memMB;

#define Zero 0x0
#define One  0x1
#define Four 0x4

	for (j = 0; j < runs; j++)
	{
		kd = passes[j];
		nn = j < 15 ? 6400000 / kd : 1;
		ks = kd * 2;

#ifdef _INT64_
		ki = kd;
#else
		ki = kd * 2;
#endif

		memMB = (double)kd * 16.0 / 1000000;
		mem = (int)((double)kd * 16.0 / 1000);

		n1 = nn;
		do {
			sumd = 1.00001;
			for (m = 0; m < kd; m++)
			{
				xd[m] = One;
				yd[m] = One;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < kd; m += Four)
				{
					xd[m] = xd[m] + sumd * yd[m];
					xd[m + 1] = xd[m + 1] + sumd * yd[m + 1];
					xd[m + 2] = xd[m + 2] + sumd * yd[m + 2];
					xd[m + 3] = xd[m + 3] + sumd * yd[m + 3];
				}
			}
		} while (checkTime());

		mbpsd = (double)n1 * memMB / secs;

		n1 = nn;
		do {
			sums = 1.0001;
			for (m = 0; m < ks; m++)
			{
				xs[m] = One;
				ys[m] = One;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < ks; m += Four)
				{
					xs[m] = xs[m] + sums * ys[m];
					xs[m + 1] = xs[m + 1] + sums * ys[m + 1];
					xs[m + 2] = xs[m + 2] + sums * ys[m + 2];
					xs[m + 3] = xs[m + 3] + sums * ys[m + 3];
				}
			}
		} while (checkTime());

		mbpss = (double)n1 * memMB / secs;

		n1 = nn;
		do {
			sumi = 0;
			for (m = 0; m < ki; m++)
			{
				xi[m] = Zero;
				yi[m] = Zero;
			}
			yi[ki - 1] = One;
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < ki; m += Four)
				{
					xi[m] = xi[m] + sumi + yi[m];
					xi[m + 1] = xi[m + 1] + sumi + yi[m + 1];
					xi[m + 2] = xi[m + 2] + sumi + yi[m + 2];
					xi[m + 3] = xi[m + 3] + sumi + yi[m + 3];
				}
			}
		} while (checkTime());

		mbpsi = (double)n1 * memMB / secs;

		printf(" |%8d %7d%7d%7d", mem, (int)mbpsd, (int)mbpss, (int)mbpsi);

		n1 = nn;
		do {
			for (m = 0; m < kd; m++)
			{
				xd[m] = Zero;
				yd[m] = One;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < kd; m += Four)
				{
					xd[m] = xd[m] + yd[m];
					xd[m + 1] = xd[m + 1] + yd[m + 1];
					xd[m + 2] = xd[m + 2] + yd[m + 2];
					xd[m + 3] = xd[m + 3] + yd[m + 3];
				}
			}
		} while (checkTime());

		sumd  = xd[1];
		mbpsd = (double)n1 * memMB / secs;

		n1 = nn;
		do {
			for (m = 0; m < ks; m++)
			{
				xs[m] = Zero;
				ys[m] = One;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < ks; m += Four)
				{
					xs[m] = xs[m] + ys[m];
					xs[m + 1] = xs[m + 1] + ys[m + 1];
					xs[m + 2] = xs[m + 2] + ys[m + 2];
					xs[m + 3] = xs[m + 3] + ys[m + 3];
				}
			}
		} while (checkTime());

		sums  = xs[1];
		mbpss = (double)n1 * memMB / secs;

		n1 = nn;
		do {
			for (m = 0; m < ki; m++)
			{
				xi[m] = Zero;
				yi[m] = One;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < ki; m += Four)
				{
					xi[m] = xi[m] + yi[m];
					xi[m + 1] = xi[m + 1] + yi[m + 1];
					xi[m + 2] = xi[m + 2] + yi[m + 2];
					xi[m + 3] = xi[m + 3] + yi[m + 3];
				}
			}
		} while (checkTime());

		sumi  = xi[1];
		mbpsi = (double)n1 * memMB / secs;

		printf("%7d%7d%7d", (int)mbpsd, (int)mbpss, (int)mbpsi);

		memMB = (double)kd * 8 / 1000000;

		n1 = nn;
		do
		{
			for (m = 0; m < kd + Four; m++)
			{
				xd[m] = Zero;
				yd[m] = m;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < kd; m += Four)
				{
					xd[m] = yd[m];
					xd[m + 1] = yd[m + 1];
					xd[m + 2] = yd[m + 2];
					xd[m + 3] = yd[m + 3];
				}
			}
		} while (checkTime());

		sumd  = xd[kd - 1] + 1;
		mbpsd = (double)n1 * memMB / secs;

		n1 = nn;
		do {
			for (m = 0; m < ks + Four; m++)
			{
				xs[m] = Zero;
				ys[m] = m;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < ks; m += Four)
				{
					xs[m] = ys[m];
					xs[m + 1] = ys[m + 1];
					xs[m + 2] = ys[m + 2];
					xs[m + 3] = ys[m + 3];
				}
			}
		} while (checkTime());

		sums  = xs[ks - 1] + 1;
		mbpss = (double)n1 * memMB / secs;

		n1 = nn;
		do {
			for (m = 0; m < ki + Four; m++)
			{
				xi[m] = Zero;
				yi[m] = m;
			}
			start_time();
			for (i = 0; i < n1; i++)
			{
#ifdef _OMP_
#pragma omp parallel for
#endif
				for (m = 0; m < ki; m += Four)
				{
					xi[m] = yi[m];
					xi[m + 1] = yi[m + 1];
					xi[m + 2] = yi[m + 2];
					xi[m + 3] = yi[m + 3];
				}
			}
		} while (checkTime());

		sumi  = xi[ki - 1] + 1;
		mbpsi = (double)n1 * memMB / secs;

		printf("%7d%7d%7d\n", (int)mbpsd, (int)mbpss, (int)mbpsi);
	}
	printf(_ENDLINE_, gsecs);
	_mm_free(yd);
	_mm_free(xd);
	return 0;
}
