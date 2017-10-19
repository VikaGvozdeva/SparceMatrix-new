#include <mkl.h>

#include "CooMatrix.h"
#include "MatrixEllpack.h"
#include "MatrixEllpackWithCoo.h"
#include "MatrixSellC.h"
#include "MatrixSellCSigma.h"
#include "Converter.h"

#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

#else
#error "Unable to define getCPUTime( ) for an unknown OS."
#endif

/**
* Returns the amount of CPU time used by the current process,
* in seconds, or -1.0 if an error occurred.
*/
double getCPUTime()
{
#if defined(_WIN32)
	/* Windows -------------------------------------------------- */
	FILETIME createTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;
	if (GetProcessTimes(GetCurrentProcess(),
		&createTime, &exitTime, &kernelTime, &userTime) != -1)
	{
		SYSTEMTIME userSystemTime;
		if (FileTimeToSystemTime(&userTime, &userSystemTime) != -1)
			return (double)userSystemTime.wHour * 3600.0 +
			(double)userSystemTime.wMinute * 60.0 +
			(double)userSystemTime.wSecond +
			(double)userSystemTime.wMilliseconds / 1000.0;
	}

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
	/* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
	/* Prefer high-res POSIX timers, when available. */
	{
		clockid_t id;
		struct timespec ts;
#if _POSIX_CPUTIME > 0
		/* Clock ids vary by OS.  Query the id, if possible. */
		if (clock_getcpuclockid(0, &id) == -1)
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
			/* Use known clock id for AIX, Linux, or Solaris. */
			id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
			/* Use known clock id for BSD or HP-UX. */
			id = CLOCK_VIRTUAL;
#else
			id = (clockid_t)-1;
#endif
		if (id != (clockid_t)-1 && clock_gettime(id, &ts) != -1)
			return (double)ts.tv_sec +
			(double)ts.tv_nsec / 1000000000.0;
	}
#endif

#if defined(RUSAGE_SELF)
	{
		struct rusage rusage;
		if (getrusage(RUSAGE_SELF, &rusage) != -1)
			return (double)rusage.ru_utime.tv_sec +
			(double)rusage.ru_utime.tv_usec / 1000000.0;
	}
#endif

#if defined(_SC_CLK_TCK)
	{
		const double ticks = (double)sysconf(_SC_CLK_TCK);
		struct tms tms;
		if (times(&tms) != (clock_t)-1)
			return (double)tms.tms_utime / ticks;
	}
#endif

#if defined(CLOCKS_PER_SEC)
	{
		clock_t cl = clock();
		if (cl != (clock_t)-1)
			return (double)cl / (double)CLOCKS_PER_SEC;
	}
#endif

#endif

	return -1;      /* Failed. */
}


int main(int argc, char** argv)
{
	char* fileName;
	FPTYPE timer;
	FPTYPE* v;
	FPTYPE* result_ell;
	FPTYPE* result_ellc;
	FPTYPE* result_sell;
	FPTYPE* result_sells;
	FPTYPE * result_mkl_coo;
	FILE *fp = fopen("data.dt", "w");

	fileName = new char[256];
	strcpy(fileName,argv[1]);

	fprintf(fp, "Matrix file name: %s\n\n", fileName);

	CooMatrix Matrix(0, 0);
	timer = getCPUTime();
	Matrix.ReadMatrix(fileName);
	timer = getCPUTime() - timer;
	fprintf(fp, "Time Read Matrix Info: \t%lf\n\n", timer);

	timer = getCPUTime();
	Matrix.SortWithVectors();
	timer = getCPUTime() - timer;
	fprintf(fp, "Time Sort with Vectors: \t%lf\n\n", timer);

	MatrixEllpack ell(0, 0);
	timer = getCPUTime();
	Converter::CooToEll(Matrix, ell);
	timer = getCPUTime() - timer;
	fprintf(fp, "Time Convert in \n\nELLPACK: \t\t%lf\n", timer);

	MatrixEllpackWithCoo ellc(0, 0,0);
	timer = getCPUTime();
	Converter::CooToEllWithCoo(Matrix, ellc, Matrix.n/2);
	timer = getCPUTime() - timer;
	fprintf(fp, "Time Convert in \n\nELLPACK_With_Coo: \t\t%lf\n", timer);

	MatrixSellC sell(0, 0, 0);
	timer = getCPUTime();
	Converter::CooToSellC(Matrix, sell, Matrix.n/2);
	timer = getCPUTime() - timer;
	fprintf(fp, "Time Convert in \n\nSell-C: \t\t%lf\n", timer);

	MatrixSellCSigma sells(0, 0, 0,0);
	timer = getCPUTime();
	Converter::CooToSellCSigma(Matrix, sells, Matrix.n / 2, Matrix.n / 2);
	timer = getCPUTime() - timer;
	fprintf(fp, "Time Convert in \n\nSell-C-Sigma: \t\t%lf\n", timer);

	v = new FPTYPE[Matrix.n];
	for (INTTYPE i = 0; i < Matrix.n; i++)
		v[i] = 1;
	result_ell = new FPTYPE[Matrix.n];
	result_ellc = new FPTYPE[Matrix.n];
	result_sell = new FPTYPE[Matrix.n];
	result_sells = new FPTYPE[Matrix.n];
	result_mkl_coo = new FPTYPE[Matrix.n];

	timer = getCPUTime();
	ellc.Matrix_VectorMultiplicationInELLPACKwithCOO(v, Matrix.n, result_ellc);
	timer = getCPUTime() - timer;
	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nELLPACK_With_Coo: \t\t%lf\n", timer);

	for (INTTYPE i = 0; i < Matrix.nnz; i++)
	{
		Matrix.row_ind[i]++;
		Matrix.col_ind[i]++;
	}
	timer = getCPUTime();
	mkl_dcoogemv("n", &Matrix.n, Matrix.val, Matrix.row_ind, Matrix.col_ind, &Matrix.nnz, v, result_mkl_coo);
	timer = getCPUTime() - timer;
	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nMKL(Coo): \t\t%lf\n", timer);
	/*for (int i = 0; i < Matrix.n; i++)
	{
		cout << result_mkl_coo[i] << ", ";
	}
	cout << endl;*/

	/*cout << ellc.n << endl;
	cout << ellc.nnz_max << endl;*/

	timer = getCPUTime();
	ell.Matrix_VectorMultiplicationInEllpack(v, Matrix.n, result_ell);
	timer = getCPUTime() - timer;
	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nELLPACK: \t\t%lf\n", timer);

	/*cout << ellc.n << endl;
	cout << ellc.nnz_max << endl;
	timer = getCPUTime();
	ellc.Matrix_VectorMultiplicationInELLPACKwithCOO(v, Matrix.n, result_ellc);
	timer = getCPUTime() - timer;
	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nELLPACK_With_Coo: \t\t%lf\n", timer);
	for (int i = 0; i < Matrix.n; i++)
	{
		cout << result_ellc[i] << ", ";
	}
	cout << endl;*/

	timer = getCPUTime();
	sell.Matrix_VectorMultiplicationInSELL_C(v, Matrix.n, result_sell);
	timer = getCPUTime() - timer;
	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nSell-C: \t\t%lf\n", timer);

	timer = getCPUTime();
	sells.Matrix_VectorMultiplicationInSELL_C_sigma(v, Matrix.n, result_sells);
	timer = getCPUTime() - timer;
	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nSell-C-Sigma: \t\t%lf\n", timer);

	printf("difference in ell %.15f\n", CheckCorrectness(result_ell, result_mkl_coo, Matrix.n));
	printf("difference in ellc %.15f\n", CheckCorrectness(result_ellc, result_mkl_coo, Matrix.n));
	printf("difference in sell %.15f\n", CheckCorrectness(result_sell, result_mkl_coo, Matrix.n));
	printf("difference in sells %.15f\n", CheckCorrectness(result_sells, result_mkl_coo, Matrix.n));

	delete[] v;
	delete[] result_ell;
	delete[] result_ellc;
	delete[] result_sell;
	delete[] result_sells;
	delete[] result_mkl_coo;
	delete[] fileName;
	fclose(fp);
	//cin >> Matrix.n;
	return 0;
}