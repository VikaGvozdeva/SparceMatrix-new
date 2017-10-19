﻿#define _CRT_SECURE_NO_WARNINGS

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include <mkl.h>

#include "CCS.h"

#include "CD.h"
#include "Converter.h"
 #include "include\CRS.h"
 #include "include\COO.h"
 #include "include\JD.h"

using namespace std;

//#define USE_INT_64
#define USE_DOUBLE

#ifdef USE_INT
	typedef int64_t INTTYPE;
#else
	typedef int32_t INTTYPE;
#endif

#ifdef USE_DOUBLE
	typedef double FPTYPE;
#define ZERO 0.0
#else
	typedef float FPTYPE;
#define ZERO 0.0f
#endif

#define MAX_LINE_LEN 1000000
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

double SearchMax_double(double* arr, int N)
{
	int i;
	double max_arr = arr[0];
	for (i = 1; i < N; i++)
	{
		if (max_arr < arr[i])
			max_arr = arr[i];
	}
	return max_arr;
}
double CheckCorrectness(double* my_mult, double* mkl_mult, int N)
{
	int i;
	double res;
	double* arr_abs;
	arr_abs = (double*)malloc(N * sizeof(double));
	for (i = 0; i < N; i++)
		arr_abs[i] = abs(my_mult[i] - mkl_mult[i]);
	res = SearchMax_double(arr_abs, N);
	free(arr_abs);
	return res;
}
int main(int argc, char** argv)
{
	FPTYPE startTime, endTime;

	char *fileName;
	char *act;
	char *fileInput;
//	char *fileOutput;
	char *typeMatrix;
	INTTYPE CDdiag;
	INTTYPE maxvalJD;
//	INTTYPE diagelemSL;

	
	//FPTYPE* v;
	FPTYPE* result_crs;
	FPTYPE* result_ccs;
	FPTYPE* result_cd;
	FPTYPE* result_jd;
	FPTYPE* result_mkl;

	FILE *fp = fopen("data.dt", "w");
	COOMatrix* Matrix = new COOMatrix(0, 0);
	CRSMatrix* CRS = new CRSMatrix(0, 0);
	CCSMatrix* CCS = new CCSMatrix(0,0);
	CDMatrix* CD = new CDMatrix(0,0,0);
	JDMatrix* JD = new JDMatrix(0,0,0);
	//SLMatrix *SL;

	fileName = new char[20];
	fileInput = new char[30];
	typeMatrix = new char[5];

	act = new char[3];
	strcpy(fileName, argv[1]);
	char* name = new char[strlen(fileName) - 4];
	//strncpy(name, fileName, strchr(fileName, '.')- fileName);
	strcpy(act, argv[2]);
	strcpy(typeMatrix, argv[3]);
	//strncpy(name, fileName, strchr(fileName, '.') - fileName);
	cout << "file" << name << endl;
	//strncpy(name,"nos3");
	sprintf(name, "nos3");
	sprintf(fileInput, "COO_nos3.bin");


	if (strcmp("COO", typeMatrix)==0)
	{
		/*char* res_file = "COO_res_";
		strcat(res_file, name);
		strcat(res_file, ".dt");
		
		FILE *fp = fopen(res_file, "w");*/
		startTime = getCPUTime();
		Matrix = Matrix->ReadMatrix(fileName);

		endTime = getCPUTime();

		if (strcmp("wr", act) == 0)
		{
			sprintf(fileInput, "COO_%s.bin", name);
			Matrix->WriteInBinaryFile(*Matrix,fileInput);
			fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
		}

		if (strcmp("r", act) == 0)
		{
			//std::cout << "name is" << name << std::endl;
			//std::cout << "name is" << fileInput << std::endl;
			Matrix->ReadFromBinaryFile(fileInput);
			Matrix->PrintMatrix(Matrix->NNZ);
		}
	}

	if (strcmp("CRS", typeMatrix) == 0)
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix = Matrix->ReadFromBinaryFile(fileInput);
		FPTYPE* v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}

		//cout << "matr:" << Matrix->N << " " << Matrix->NNZ << endl;
		cout << "input name" << fileInput << endl;
		if (strcmp("wr", act) == 0)
		{
			*CRS = CRSMatrix(Matrix->NNZ, Matrix->N);
			startTime = getCPUTime();
			Converters::COOToCRS(*Matrix, *CRS);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCRS: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "CRS_%s.bin", name);
			CRS->WriteInBinaryFile(*CRS, fileInput);
			//CRS->PrintMatrix(CRS->NNZ, CRS->N);
		}
		if (strcmp("r", act) == 0)
		{
			cout << "error before" << endl;
			sprintf(fileInput, "CRS_%s.bin", name);
			CRS = CRS->ReadFromBinaryFile(fileInput);
			cout << "error after" << endl;
			cout << "NNZ" << CRS->NNZ << " N" << CRS->N << endl;
			CRS->PrintMatrix(CRS->NNZ, CRS->N);
		}

		result_crs = new FPTYPE[Matrix->N];
		
		startTime = getCPUTime();
		CRS->MatrixVectorMultCRS(CRS, v, Matrix->N, result_crs);
		endTime = getCPUTime();
		fprintf(fp, "Time Matrix-Vector multiplication in \n\nCRS: \t%.15f\n", endTime - startTime);

		result_mkl = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N + 1; i++)
		{
			CRS->row_ptr[i]++;
		}
		for (int i = 0; i < Matrix->NNZ; i++)
		{
			CRS->col_ind[i]++;
		}
		startTime = getCPUTime();
		mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
		endTime = getCPUTime();
		fprintf(fp, "\n Time Matrix-Vector multiplication in \n\nMKL CRS: \t\t%lf\n", endTime - startTime);
		fprintf(fp, "\ndifference %.15f with CRS\n", CheckCorrectness(result_crs, result_mkl, CRS->N));
	}

	if (strcmp("CCS", typeMatrix) == 0)
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix = Matrix->ReadFromBinaryFile(fileInput);
		FPTYPE* v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{
			*CCS = CCSMatrix(Matrix->NNZ, Matrix->N);
			startTime = getCPUTime();
			Converters::COOToCCS(*Matrix, *CCS);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCCS: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "CCS_%s.bin", name);
			CCS->WriteInBinaryFile(*CCS, fileInput);
			//CRS->PrintMatrix(CRS->NNZ, CRS->N);
		}
		if (strcmp("r", act) == 0)
		{
			sprintf(fileInput, "CCS_%s.bin", name);
			CCS = CCS->ReadFromBinaryFile(fileInput);
		}

		result_ccs = new FPTYPE[Matrix->N];
		
		startTime = getCPUTime();
		CCS->MatrixVectorMultCCS(CCS, v, Matrix->N, result_ccs);
		endTime = getCPUTime();
		fprintf(fp, "Time Matrix-Vector multiplication in \n\nCCS: \t%.15f\n", endTime - startTime);

}	
	if (strcmp("CD", typeMatrix) == 0)
	{

		sprintf(fileInput, "COO_%s.bin", name);
		Matrix = Matrix->ReadFromBinaryFile(fileInput);
		FPTYPE* v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{
			CDdiag = Matrix->DiagCDMatrix(*Matrix);
			*CD = CDMatrix(Matrix->NNZ, Matrix->N, CDdiag);
			startTime = getCPUTime();
			Converters::COOToCD(*Matrix, *CD);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCD: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "CD_%s.bin", name);
			cout <<"check" <<fileInput << endl;
			CD->WriteInBinaryFile(*CD, fileInput);
		}

		if (strcmp("r", act) == 0)
		{
			sprintf(fileInput, "CD_%s.bin", name);
			CD = CD->ReadFromBinaryFile(fileInput);
		}
	
		cout << "size" << Matrix->N << endl;
		result_cd = new FPTYPE[Matrix->N];
		startTime = getCPUTime();
		CD->MatrixVectorMultCD(CD, v, Matrix->N, result_cd);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCD: \t\t%lf\n", endTime - startTime);
		for (int i = 0; i < Matrix->N; i++)
		{
			cout << result_cd[i] << endl;
		}
	}
	if (strcmp("JD", typeMatrix) == 0)
	{

		sprintf(fileInput, "COO_%s.bin", name);
		Matrix = Matrix->ReadFromBinaryFile(fileInput);
		maxvalJD = Matrix->maxvalJDMatrix(*Matrix);
		cout << "maxvaljd2 " << maxvalJD << endl;
		FPTYPE* v = new FPTYPE[Matrix->N];

		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{

			//Matrix->PrintMatrix(Matrix->NNZ);
			maxvalJD = Matrix->maxvalJDMatrix(*Matrix);
			cout << "maxvaljd1 " << maxvalJD << endl;
			*JD = JDMatrix(Matrix->NNZ, Matrix->N, maxvalJD);
			startTime = getCPUTime();
			//Converters::COOToJD(*Matrix, *JD);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nJD: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "JD_%s.bin", name);
			JD->WriteInBinaryFile(*JD, fileInput);
		}
		if (strcmp("r", act) == 0)
		{
			sprintf(fileInput, "JD_%s.bin", name);
			JD->ReadFromBinaryFile(fileInput);
		}

		result_jd = new FPTYPE[Matrix->N];
		startTime = getCPUTime();
		JD->MatrixVectorMultJD(JD, v, Matrix->N, result_jd);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nJD: \t\t%lf\n", endTime - startTime);
	}
	////if (typeMatrix == "SL")
	////{
	////	sprintf(fileInput, "COO_%s.bin", name);
	////	Matrix->ReadFromBinaryFile(fileInput);
	////	if (act == "wr")
	////	{
	////		diagelemSL = Matrix->diagSLMatrix(*Matrix);
	////		SL = new SLMatrix(Matrix->NNZ, Matrix->N, diagelemSL);
	////		startTime = getCPUTime();
	////		Converters::COOToSL(*Matrix, *SL);
	////		endTime = getCPUTime();
	////		fprintf(fp, "Time Convert in \n\nSL: \t\t%.15f\n", endTime - startTime);
	////		sprintf(fileInput, "SL_%s.bin", name);
	////		JD->WriteInBinaryFile(*JD, fileInput);
	////	}
	////	if (act == "r")
	////	{
	////		sprintf(fileInput, "SL_%s.bin", name);
	////		SL->ReadFromBinaryFile(fileInput);
	////	}
	////	v = new FPTYPE[Matrix->N];
	////	for (int i = 0; i < Matrix->N; i++)
	////	{
	////		v[i] = 1;
	////	}
	////	result_sl = new FPTYPE[Matrix->N];
	////	startTime = getCPUTime();
	////	SL->MatrixVectorMultSL(SL, v, Matrix->N, result_sl);
	////	endTime = getCPUTime();
	////	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCD: \t\t%lf\n", endTime - startTime);
	////}

	//if (typeMatrix == "MKL")
	//{
	//	//	char name[256];
	//	//strncpy(name, fileName, strchr(fileName, '.') - fileName);
	//	//sprintf(fileInput, "CRS_Lin.bin");
	//	Matrix = Matrix->ReadFromBinaryFile("COO_Lin.bin");
	//	if (act == "crs")
	//	{
	//		*CRS = CRSMatrix(Matrix->NNZ, Matrix->N);
	//		CRS->ReadFromBinaryFile("CRS_Lin.bin");
	//		FPTYPE* v = new FPTYPE[Matrix->N];
	//		for (int i = 0; i < Matrix->N; i++)
	//		{
	//			v[i] = 2;
	//		}
	//		result_mkl = new FPTYPE[Matrix->N];
	//		for (int i = 0; i < Matrix->N + 1; i++)
	//		{
	//			CRS->row_ptr[i]++;
	//		}
	//		for (int i = 0; i < Matrix->NNZ; i++)
	//		{
	//			CRS->col_ind[i]++;
	//		}
	//		startTime = getCPUTime();
	//		mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
	//		endTime = getCPUTime();
	//		fprintf(fp, "\n Time Matrix-Vector multiplication in \n\nMKL CRS: \t\t%lf\n", endTime - startTime);
	//		//fprintf(fp, "\ndifference %.15f with CRS\n", CheckCorrectness(result_crs, result_mkl, CRS->N));
	//	}
	//}
	//	if (act == "coo")
	//	{
	//		//
	//	}

	//}



	
	//char name[100] = "matrix.mtx";
	//char name2[100];
	//char name3[100];
	//strncpy(name3, name, strchr(name, '.') - name);
	//sprintf(name2, "COO_%s.bin", name3);

	

	//	mkl_dcoogemv("N", &Matrix->N, Matrix->val, Matrix->row_ind, Matrix->col_ind, &Matrix->NNZ, v, result_mkl);
	

	//fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nMKL CRS: \t\t%.15f\n", endTime - startTime);
	//fprintf(fp, "\ndifference %.15f with JDIAG\n", CheckCorrectness(result_jd, result_mkl, Matrix.N));
	//fprintf(fp, "\ndifference %.15f with CCS\n", CheckCorrectness(result_ccs, result_mkl, Matrix.N));
	//fprintf(fp, "\ndifference %.15f with CRS\n", CheckCorrectness(result_crs, result_mkl, Matrix.N));
	////fprintf(fp, "\ndifference %.15f with SKYLINE\n", CheckCorrectness(result_sl, result_mkl, Matrix->N));
	//fprintf(fp, "\ndifference %.15f with COMPDIAG\n", CheckCorrectness(result_cd, result_mkl, Matrix.N));


//	delete[] result_crs;
	//delete[] result_ccs;
//	delete[] result_cd;
//	delete[] result_jd;
	//delete[] result_sl;
	//delete[] result_mkl;
	system("pause");
	return 0;
}
