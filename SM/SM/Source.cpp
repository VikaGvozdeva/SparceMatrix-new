//#define _CRT_SECURE_NO_WARNINGS
//
//#include <inttypes.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <cmath>
//#include <string.h>
//#include <iostream>
//#include <vector>
//#include <string>
//#include <mkl.h>
////#include <asmlib.h>
//#include <intrin.h> 
//
//#include "CCS.h"
//#include "CD.h"
//#include "Converter.h"
//#include "CRS.h"
//#include "COO.h"
//#include "JD.h"
//#include "SL.h"
//
//using namespace std;
//
////#define USE_INT_64
//#define USE_DOUBLE
//
//#ifdef USE_INT
//	typedef int64_t INTTYPE;
//#else
//	typedef int32_t INTTYPE;
//#endif
//
//#ifdef USE_DOUBLE
//	typedef double FPTYPE;
//#define ZERO 0.0
//#else
//	typedef float FPTYPE;
//#define ZERO 0.0f
//#endif
//
//#define MAX_LINE_LEN 1000000
//#if defined(_WIN32)
//#include <Windows.h>
//
//#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
//#include <unistd.h>
//#include <sys/resource.h>
//#include <sys/times.h>
//#include <time.h>
//
//#else
//#error "Unable to define getCPUTime( ) for an unknown OS."
//#endif
//
///**
//* Returns the amount of CPU time used by the current process,
//* in seconds, or -1.0 if an error occurred.
//*/
//double getCPUTime()
//{
//#if defined(_WIN32)
//	/* Windows -------------------------------------------------- */
//	FILETIME createTime;
//	FILETIME exitTime;
//	FILETIME kernelTime;
//	FILETIME userTime;
//	if (GetProcessTimes(GetCurrentProcess(),
//		&createTime, &exitTime, &kernelTime, &userTime) != -1)
//	{
//		SYSTEMTIME userSystemTime;
//		if (FileTimeToSystemTime(&userTime, &userSystemTime) != -1)
//			return (double)userSystemTime.wHour * 3600.0 +
//			(double)userSystemTime.wMinute * 60.0 +
//			(double)userSystemTime.wSecond +
//			(double)userSystemTime.wMilliseconds / 1000.0;
//	}
//
//#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
//	/* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */
//
//#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
//	/* Prefer high-res POSIX timers, when available. */
//	{
//		clockid_t id;
//		struct timespec ts;
//#if _POSIX_CPUTIME > 0
//		/* Clock ids vary by OS.  Query the id, if possible. */
//		if (clock_getcpuclockid(0, &id) == -1)
//#endif
//#if defined(CLOCK_PROCESS_CPUTIME_ID)
//			/* Use known clock id for AIX, Linux, or Solaris. */
//			id = CLOCK_PROCESS_CPUTIME_ID;
//#elif defined(CLOCK_VIRTUAL)
//			/* Use known clock id for BSD or HP-UX. */
//			id = CLOCK_VIRTUAL;
//#else
//			id = (clockid_t)-1;
//#endif
//		if (id != (clockid_t)-1 && clock_gettime(id, &ts) != -1)
//			return (double)ts.tv_sec +
//			(double)ts.tv_nsec / 1000000000.0;
//	}
//#endif
//
//#if defined(RUSAGE_SELF)
//	{
//		struct rusage rusage;
//		if (getrusage(RUSAGE_SELF, &rusage) != -1)
//			return (double)rusage.ru_utime.tv_sec +
//			(double)rusage.ru_utime.tv_usec / 1000000.0;
//	}
//#endif
//
//#if defined(_SC_CLK_TCK)
//	{
//		const double ticks = (double)sysconf(_SC_CLK_TCK);
//		struct tms tms;
//		if (times(&tms) != (clock_t)-1)
//			return (double)tms.tms_utime / ticks;
//	}
//#endif
//
//#if defined(CLOCKS_PER_SEC)
//	{
//		clock_t cl = clock();
//		if (cl != (clock_t)-1)
//			return (double)cl / (double)CLOCKS_PER_SEC;
//	}
//#endif
//
//#endif
//
//	return -1;      /* Failed. */
//}
//
//double SearchMax_double(double* arr, int N)
//{
//	int i;
//	double max_arr = arr[0];
//	for (i = 1; i < N; i++)
//	{
//		if (max_arr < arr[i])
//			max_arr = arr[i];
//	}
//	return max_arr;
//}
//double CheckCorrectness(double* my_mult, double* mkl_mult, int N)
//{
//	int i;
//	double res;
//	double* arr_abs;
//	arr_abs = (double*)malloc(N * sizeof(double));
//	for (i = 0; i < N; i++)
//		arr_abs[i] = abs(my_mult[i] - mkl_mult[i]);
//	res = SearchMax_double(arr_abs, N);
//	free(arr_abs);
//	return res;
//}
//
//char* getFileNameFromPath(char* path)
//{
//	for (size_t i = strlen(path) - 1; i >= 0; i--)
//	{
//		if ((path[i] == '/') || (path[i] == '\\'))
//		{
//			return &path[i + 1];
//		}
//	}
//
//	printf("%s \n", path);
//	return path;
//}
//
//long long ReadTSC() { // Returns time stamp counter
//	int dummy[4]; // For unused returns
//	volatile int DontSkip; // Volatile to prevent optimizing
//	long long clock; // Time
//	__cpuid(dummy, 0); // Serialize
//	DontSkip = dummy[0]; // Prevent optimizing away cpuid
//	clock = __rdtsc(); // Read time
//	return clock;
//}
//int main(int argc, char** argv)
//{
//	FPTYPE startTime, endTime;
//
//	char *fileName;
//	char *act;
//	char *fileInput, *fileOutput;
//	char *typeMatrix;
//	char *fileInputCoo;
//	char *fn_pnt;
//	INTTYPE CDdiag;
//	INTTYPE SLdiag;
//	INTTYPE maxvalJD;
//	
//	FPTYPE* result_crs;
//	FPTYPE* result_ccs;
//	FPTYPE* result_cd;
//	FPTYPE* result_jd;
//	FPTYPE* result_sl;
//	FPTYPE* result_mkl;
//
//	//FILE *fp = fopen("data.dt", "w");
//	FILE *fp = stdout;
//	COOMatrix* Matrix = new COOMatrix(0, 0);
//	CRSMatrix* CRS = new CRSMatrix(0, 0);
//	CCSMatrix* CCS = new CCSMatrix(0,0);
//	CDMatrix CD(0, 0, 0); 
//	//CDMatrix* CD = new CDMatrix(0, 0, 0);
//	JDMatrix* JD = new JDMatrix(0,0,0);
//	SLMatrix* SL = new SLMatrix(0, 0, 0);
//
//	fileName = new char[300];
//	fileInput = new char[300];
//	fileOutput = new char[300];
//	fileInputCoo = new char[300];
//	typeMatrix = new char[300];
//	act = new char[3];
//
//	strcpy(fileName, argv[1]);
//	fprintf(fp, "Matrix file name: %s\n\n", fileName);
//	char* name = new char[strlen(fileName) - 4];
//	
//
//	fn_pnt = getFileNameFromPath(fileName);
//	strncpy(name, fn_pnt, strlen(fn_pnt) - 4);
//	name[strlen(fileName) - 4] = '\0';
//	cout << "name is -" << name << endl;
//	strcpy(act, argv[2]);
//	strcpy(typeMatrix, argv[3]);
//
//
//	//fileName = new char[10];
//	//fileInput = new char[30];
//	//typeMatrix = new char[5];
//	//act = new char[3];
//
//	//strcpy(fileName, argv[1]);
//	//char* name = new char[strlen(fileName) - 4];
//	//strncpy(name, fileName, strlen(fileName) - 4);
//	//cout << "length" << strlen(fileName) << endl;
//	//cout << "filename is " << fileName << endl;
//	//cout << "name is " << name << endl;
//	//strcpy(act, argv[2]);
//	//strcpy(typeMatrix, argv[3]);
//
//
//	if (strcmp("COO", typeMatrix)==0)
//	{
//
//		sprintf(fileInput, "COO_%s.bin", name);
//		startTime = getCPUTime();
//		Matrix = Matrix->ReadMatrix(fileName);
//
//		endTime = getCPUTime();
//		sprintf(fileOutput, "COO_%s_res.dt", name);
//		fp = fopen(fileOutput, "w");
//		if (strcmp("wr", act) == 0)
//		{
//			Matrix->WriteInBinaryFile(*Matrix,fileInput);
//			fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
//		}
//
//		if (strcmp("r", act) == 0)
//		{
//			Matrix->ReadFromBinaryFile(fileInput);
//		}
//		fclose(fp);
//	}
//
//	if (strcmp("CRS", typeMatrix) == 0)
//	{
//
//		long long st, end;
//		sprintf(fileInput, "CRS_%s.bin", name);
//		sprintf(fileInputCoo, "COO_%s.bin", name);
//		sprintf(fileOutput, "CRS_%s_res.dt", name);
//		fp = fopen(fileOutput, "w");
//		Matrix = Matrix->ReadFromBinaryFile(fileInputCoo);
//		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
//		FPTYPE* v = new FPTYPE[Matrix->N];
//		for (int i = 0; i < Matrix->N; i++)
//		{
//			v[i] = 1;
//		}
//	
//
//		if (strcmp("wr", act) == 0)
//		{
//			*CRS = CRSMatrix(Matrix->NNZ, Matrix->N);
//			startTime = getCPUTime();
//			Converters::COOToCRS(*Matrix, *CRS);
//			endTime = getCPUTime();
//			fprintf(fp, "Time Convert in \n\nCRS: \t\t%.15f\n", endTime - startTime);
//			//sprintf(fileInput, "CRS_%s.bin", name);
//			CRS->WriteInBinaryFile(*CRS, fileInput);
//		}
//		if (strcmp("r", act) == 0)
//		{
//			//sprintf(fileInput, "CRS_%s.bin", name);
//			CRS = CRS->ReadFromBinaryFile(fileInput);
//		}
//
//		result_crs = new FPTYPE[Matrix->N];
//		
//		st=ReadTSC();
//		//startTime = getCPUTime();
//		CRS->MatrixVectorMultCRS(CRS, v, Matrix->N, result_crs);
//		end = ReadTSC()-st;
//		//endTime = getCPUTime();
//		//fprintf(fp, "Time Matrix-Vector multiplication in \n\nCRS: \t%.15f\n", endTime - startTime);
//		fprintf(fp, "Time Matrix-Vector multiplication in \n\nCRS: \t%10I64i\n", end);
//
//	/*	result_mkl = new FPTYPE[Matrix->N];
//		for (int i = 0; i < Matrix->N + 1; i++)
//		{
//			CRS->row_ptr[i]++;
//		}
//		for (int i = 0; i < Matrix->NNZ; i++)
//		{
//			CRS->col_ind[i]++;
//		}
//
//		startTime = getCPUTime();
//		mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
//		endTime = getCPUTime();
//		fprintf(fp, "\n Time Matrix-Vector multiplication in \n\nMKL CRS: \t\t%lf\n", endTime - startTime);
//		fprintf(fp, "\ndifference %.15f with CRS\n", CheckCorrectness(result_crs, result_mkl, CRS->N));*/
//	
//	//	fclose(fp);
//
//		delete[] v;
////		delete[] result_mkl;
//		delete[] result_crs;
//
//	}
//
//	if (strcmp("CCS", typeMatrix) == 0)
//	{
//		sprintf(fileInput, "COO_%s.bin", name);
//		Matrix = Matrix->ReadFromBinaryFile(fileInput);
//		sprintf(fileOutput, "CCS_%s_res.dt", name);
//		fp = fopen(fileOutput, "w");
//		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
//		FPTYPE* v = new FPTYPE[Matrix->N];
//		for (int i = 0; i < Matrix->N; i++)
//		{
//			v[i] = 1;
//		}
//
//		if (strcmp("wr", act) == 0)
//		{
//			*CCS = CCSMatrix(Matrix->NNZ, Matrix->N);
//			startTime = getCPUTime();
//			Converters::COOToCCS(*Matrix, *CCS);
//			endTime = getCPUTime();
//			fprintf(fp, "Time Convert in \n\nCCS: \t\t%.15f\n", endTime - startTime);
//			sprintf(fileInput, "CCS_%s.bin", name);
//			CCS->WriteInBinaryFile(*CCS, fileInput);
//		}
//		if (strcmp("r", act) == 0)
//		{
//			sprintf(fileInput, "CCS_%s.bin", name);
//			CCS = CCS->ReadFromBinaryFile(fileInput);
//		}
//
//		result_ccs = new FPTYPE[Matrix->N];
//
//		sprintf(fileInput, "CRS_%s.bin", name);
//		CRS = CRS->ReadFromBinaryFile(fileInput);
//
//		for (int i = 0; i < Matrix->N + 1; i++)
//		{
//			CRS->row_ptr[i]++;
//		}
//		for (int i = 0; i < Matrix->NNZ; i++)
//		{
//			CRS->col_ind[i]++;
//		}
//
//		startTime = getCPUTime();
//		CCS->MatrixVectorMultCCS(CCS, v, Matrix->N, result_ccs);
//		endTime = getCPUTime();
//		fprintf(fp, "Time Matrix-Vector multiplication in \n\nCCS: \t%.15f\n", endTime - startTime);
//		
//		result_mkl = new FPTYPE[Matrix->N];
//		startTime = getCPUTime();
//		mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
//		endTime = getCPUTime();
//		fprintf(fp, "Time Matrix-Vector multiplication in \n\nMKL: \t%.15f\n", endTime - startTime);
//		fprintf(fp, "\ndifference %.15f with CСS\n", CheckCorrectness(result_ccs, result_mkl, CRS->N));
//		
//		//fclose(fp);
//		delete[] v;
//		delete[] result_mkl;
//		delete[] result_ccs;
//		
//}	
//	if (strcmp("CD", typeMatrix) == 0)
//	{
//
//		sprintf(fileInput, "CD_%s.bin", name);
//		sprintf(fileInputCoo, "COO_%s.bin", name);
//		sprintf(fileOutput, "CD_%s_res.dt", name);
//		//cout << "file" << fileInput << endl;
//		Matrix = Matrix->ReadFromBinaryFile(fileInputCoo);
//		CDdiag = Matrix->DiagCDMatrix(*Matrix);
//		//cout << "ok"<<endl;
//		fp = fopen(fileOutput, "w");
//		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
//		FPTYPE* v = new FPTYPE[Matrix->N];
//		for (int i = 0; i < Matrix->N; i++)
//		{
//			v[i] = 1;
//		}
//	
//		if (strcmp("wr", act) == 0)
//		{
//			//cout << "Matrix size:"<< Matrix->N<<" "<<Matrix->NNZ<<" "<<CDdiag<< endl;
//			CD = CDMatrix(Matrix->NNZ, Matrix->N, CDdiag);
//			cout << "ok2" << endl;
//			CD.FillDiagArray(*Matrix);
//			startTime = getCPUTime();
//			Converters::COOToCD(*Matrix, CD);
//			endTime = getCPUTime();
//			fprintf(fp, "Time Convert in \n\nCD: \t\t%.15f\n", endTime - startTime);
//			//sprintf(fileInput, "CD_%s.bin", name);
//			CD.WriteInBinaryFile(CD, "CD_Lin.bin");
//		}
//
//		if (strcmp("r", act) == 0)
//		{
//			sprintf(fileInput, "CD_%s.bin", name);
//			CD.ReadFromBinaryFile(fileInputCoo);
//		}
//
//	//	result_cd = new FPTYPE[Matrix->N];
//	//	startTime = getCPUTime();
//	//	CD.MatrixVectorMultCD(&CD, v, Matrix->N, result_cd);
//	//	endTime = getCPUTime();
//	//	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCD: \t\t%lf\n", endTime - startTime);
//
//	//	//sprintf(fileInput, "CRS_nos3.bin", name);
//	//	CRS = CRS->ReadFromBinaryFile("CRS_Lin.bin");
//
//	//	for (int i = 0; i < Matrix->N + 1; i++)
//	//	{
//	//		CRS->row_ptr[i]++;
//	//	}
//	//	for (int i = 0; i < Matrix->NNZ; i++)
//	//	{
//	//		CRS->col_ind[i]++;
//	//	}
//
//	//	result_mkl = new FPTYPE[Matrix->N];
//	//	startTime = getCPUTime();
//	//	mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
//	//	//mkl_ddiagemv("N", &(CD->N), *CD->val, &CD->N, CD->diag, &CD->B, v, result_mkl);
//	//	endTime = getCPUTime();
//	///*	for (int i = 0; i < Matrix->N; i++)
//	//	{
//	//		cout << "cd " << result_cd[i] << endl;
//	//	}
//	//	for (int i = 0; i < Matrix->N; i++)
//	//	{
//	//		cout << "mkl " << result_mkl[i] << endl;
//	//	}*/
//	//	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nMKL: \t\t%lf\n", endTime - startTime);
//	//	fprintf(fp, "\ndifference %.15f with CD\n", CheckCorrectness(result_cd, result_mkl, CD.N));
//	//	
//	//	fclose(fp);
//	//	delete[] v;
//	//	delete[] result_mkl;
//	//	delete[] result_cd;
//	}
//	if (strcmp("JD", typeMatrix) == 0)
//	{
//		sprintf(fileInput, "COO_%s.bin", name);
//		Matrix = Matrix->ReadFromBinaryFile(fileInput);
//		sprintf(fileOutput, "JD_%s_res.dt", name);
//		fp = fopen(fileOutput, "w");
//		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
//		maxvalJD = Matrix->maxvalJDMatrix(*Matrix);
//		FPTYPE* v = new FPTYPE[Matrix->N];
//
//		for (int i = 0; i < Matrix->N; i++)
//		{
//			v[i] = 1;
//		}
//
//		if (strcmp("wr", act) == 0)
//		{
//			maxvalJD = Matrix->maxvalJDMatrix(*Matrix);
//			*JD = JDMatrix(Matrix->NNZ, Matrix->N, maxvalJD);
//			startTime = getCPUTime();
//			Converters::COOToJD(*Matrix, *JD);
//			endTime = getCPUTime();
//			fprintf(fp, "Time Convert in \n\nJD: \t\t%.15f\n", endTime - startTime);
//			sprintf(fileInput, "JD_%s.bin", name);
//			JD->WriteInBinaryFile(*JD, fileInput);
//		}
//		if (strcmp("r", act) == 0)
//		{
//			sprintf(fileInput, "JD_%s.bin", name);
//			JD = JD->ReadFromBinaryFile(fileInput);
//
//		}
//		result_jd = new FPTYPE[Matrix->N];
//		startTime = getCPUTime();
//		JD->MatrixVectorMultJD(JD, v, Matrix->N, result_jd);
//		endTime = getCPUTime();
//		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nJD: \t\t%lf\n", endTime - startTime);
//
//		sprintf(fileInput, "CRS_%s.bin", name);
//		CRS = CRS->ReadFromBinaryFile(fileInput);
//
//		for (int i = 0; i < Matrix->N + 1; i++)
//		{
//			CRS->row_ptr[i]++;
//		}
//		for (int i = 0; i < Matrix->NNZ; i++)
//		{
//			CRS->col_ind[i]++;
//		}
//		result_mkl = new FPTYPE[Matrix->N];
//		startTime = getCPUTime();
//		mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
//		endTime = getCPUTime();
//		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nMKL: \t\t%lf\n", endTime - startTime);
//		fprintf(fp, "\ndifference %.15f with JD\n", CheckCorrectness(result_jd, result_mkl, CRS->N));
//
//	//	fclose(fp);
//		delete[] v;
//		delete[] result_mkl;
//		delete[] result_jd;
//
//	}
//	if (strcmp("MKL", typeMatrix) == 0)
//	{
//		long long st, end;
//		sprintf(fileOutput, "MKL_%s_res.dt", name);
//		fp = fopen(fileOutput, "w");
//		sprintf(fileInput, "CRS_%s.bin", name);
//		CRS = CRS->ReadFromBinaryFile(fileInput);
//		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", CRS->N, CRS->NNZ);
//		//CRS->PrintMatrix(CRS->NNZ, CRS->N);
//		FPTYPE* v = new FPTYPE[CRS->N];
//		for (int i = 0; i < CRS->N; i++)
//		{
//			v[i] = 1;
//		}
//		for (int i = 0; i < CRS->N + 1; i++)
//		{
//			CRS->row_ptr[i]++;
//		}
//		for (int i = 0; i < CRS->NNZ; i++)
//		{
//			CRS->col_ind[i]++;
//		}
//
//		result_mkl = new FPTYPE[CRS->N];
//		//startTime = getCPUTime();
//		st = ReadTSC();
//		mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
//		end = ReadTSC() - st;
//		//endTime = getCPUTime();
//		//fprintf(fp, "Time Matrix-Vector multiplication in \n\nMKL: \t%.15f\n", endTime - startTime);
//		fprintf(fp, "Time Matrix-Vector multiplication in \n\nMKL: \t%10I64i\n", end);
//	}
//
//
//	if (strcmp("SL", typeMatrix) == 0)
//	{
//
//		sprintf(fileInput, "COO_%s.bin", name);
//		Matrix = Matrix->ReadFromBinaryFile(fileInput);
//		//cout << "COO" << endl;
//		//Matrix->PrintMatrix(Matrix->NNZ);
//		sprintf(fileOutput, "SL_%s_res.dt", name);
//		fp = fopen(fileOutput, "w");
//		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
//		SLdiag = Matrix->diagSLMatrix(*Matrix);
//		FPTYPE* v = new FPTYPE[Matrix->N];
//
//		for (int i = 0; i < Matrix->N; i++)
//		{
//			v[i] = 1;
//		}
//
//		if (strcmp("wr", act) == 0)
//		{
//			*SL = SLMatrix(Matrix->NNZ, Matrix->N, SLdiag);
//			//			cout << "diag count" << SLdiag << endl;
//			//cout << "nnz" << SL->NNZ << " " << SL->N << " " << SL->diag << endl;
//			startTime = getCPUTime();
//			Converters::COOToSL(*Matrix, *SL);
//			//cout << "okkkkkkk" << endl;
//			//			cout << "SL" << endl;
//			//SL->PrintMatrix(SL->NNZ, SL->N, SL->diag);
//			endTime = getCPUTime();
//			fprintf(fp, "\nTime convert SL: \t\t%lf\n", endTime - startTime);
//			sprintf(fileInput, "SL_%s.bin", name);
//
//			SL->WriteInBinaryFile(*SL, fileInput);
//		}
//		if (strcmp("r", act) == 0)
//		{
//			sprintf(fileInput, "SL_%s.bin", name);
//			SL = SL->ReadFromBinaryFile(fileInput);
//		}
//
//
//		result_sl = new FPTYPE[Matrix->N];
//
//		startTime = getCPUTime();
//		SL->MatrixVectorMultSL(SL, v, Matrix->N, result_sl);
//		endTime = getCPUTime();
//		fprintf(fp, "Time Matrix-Vector multiplication in \n\nSL: \t%.15f\n", endTime - startTime);
//
//		//cout << "name " << name << endl;
//		sprintf(fileInput, "CRS_%s.bin", name);
//		CRS = CRS->ReadFromBinaryFile(fileInput);
//
//		for (int i = 0; i < Matrix->N + 1; i++)
//		{
//			CRS->row_ptr[i]++;
//		}
//		for (int i = 0; i < Matrix->NNZ; i++)
//		{
//			CRS->col_ind[i]++;
//		}
//
//		result_mkl = new FPTYPE[Matrix->N];
//		startTime = getCPUTime();
//		mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
//		endTime = getCPUTime();
//	/*	cout << "mkl" << endl;
//		for (int i = 0; i < 100; i++)
//		{
//			cout << "mkl "<<result_mkl[i] << endl;
//			cout << "sl " << result_mkl[i] << endl;
//
//		}*/
//		fprintf(fp, "Time Matrix-Vector multiplication in \n\nMKL: \t%.15f\n", endTime - startTime);
//		fprintf(fp, "\ndifference %.15f with SL\n", CheckCorrectness(result_sl, result_mkl, CRS->N));
//
//	/*	for (int i = 0; i < 100; i++)
//		{
//			fprintf(fp, "mkl %f - sl %f\n",result_mkl, result_sl);
//		}*/
//
//		delete[] v;
//		delete[] result_mkl;
//		delete[] result_sl;
//	}
//	delete[] fileName;
//	delete[] fileInput;
//	delete[] fileOutput;
//	delete[] typeMatrix;
//	delete[] act;
////	delete[] name;
//
//
//
//	system("pause");
//	return 0;
//}


#define _CRT_SECURE_NO_WARNINGS

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include <mkl.h>
//#include <asmlib.h>
#include <intrin.h> 

#include "CCS.h"
#include "CD.h"
#include "Converter.h"
#include "CRS.h"
#include "COO.h"
#include "JD.h"
#include "SL.h"

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

char* getFileNameFromPath(char* path)
{
	for (size_t i = strlen(path) - 1; i >= 0; i--)
	{
		if ((path[i] == '/') || (path[i] == '\\'))
		{
			return &path[i + 1];
		}
	}

	printf("%s \n", path);
	return path;
}

long long ReadTSC() { // Returns time stamp counter
	int dummy[4]; // For unused returns
	volatile int DontSkip; // Volatile to prevent optimizing
	long long clock; // Time
	__cpuid(dummy, 0); // Serialize
	DontSkip = dummy[0]; // Prevent optimizing away cpuid
	clock = __rdtsc(); // Read time
	return clock;
}
int main(int argc, char** argv)
{

	char* fileName;
	char *act;
	char *fileInput;
	char *fileOutput;
	char *typeMatrix;
	char *fileInputCoo;
	char *fileMKLRes;
	char *fn_pnt;

	FPTYPE* result_crs;
	FPTYPE* result_ccs;
	FPTYPE* result_cd;
	FPTYPE* result_jd;
	FPTYPE* result_sl;
	FPTYPE* result_mkl;

	FILE *fp = stdout;

	COOMatrix Matrix(0, 0);
	CRSMatrix CRS(0, 0);
	CCSMatrix CCS(0, 0);
	CDMatrix CD(0, 0, 0);
	JDMatrix JD(0, 0, 0);
	SLMatrix SL(0, 0, 0);

	INTTYPE CDdiag;
	INTTYPE maxvalJD;
	INTTYPE SLdiag;

	fileName = new char[300];
	fileInput = new char[300];
	fileOutput = new char[300];
	fileMKLRes = new char[300];
	fileInputCoo = new char[300];
	typeMatrix = new char[300];
	act = new char[3];

	FPTYPE startTime, endTime;

	strcpy(fileName, argv[1]);
	fprintf(fp, "Matrix file name: %s\n\n", fileName);
	char* name = new char[strlen(fileName) - 4];


	fn_pnt = getFileNameFromPath(fileName);
	strncpy(name, fn_pnt, strlen(fn_pnt) - 4);
	name[strlen(fileName) - 4] = '\0';
	//cout << "name is -" << name << endl;
	strcpy(act, argv[2]);
	strcpy(typeMatrix, argv[3]);


	if (strcmp("COO", typeMatrix) == 0)
	{

		sprintf(fileInput, "COO_%s.bin", name);
		startTime = getCPUTime();
		//matrix=?
		Matrix.ReadMatrix(fileName);

		endTime = getCPUTime();
		sprintf(fileOutput, "COO_%s_res.dt", name);
		fp = fopen(fileOutput, "w");
		if (strcmp("wr", act) == 0)
		{
			Matrix.WriteInBinaryFile(fileInput);
			fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix.N, Matrix.NNZ);
		}

		if (strcmp("r", act) == 0)
		{
			Matrix.ReadFromBinaryFile(fileInput);
		}
		//fclose(fp);
	}

	if (strcmp("CRS", typeMatrix) == 0)
	{

		long long st, end;
		sprintf(fileInput, "CRS_%s.bin", name);
		sprintf(fileInputCoo, "COO_%s.bin", name);
		sprintf(fileOutput, "CRS_%s_res.dt", name);
		fp = fopen(fileOutput, "w");
		//Matrix = Matrix->ReadFromBinaryFile(fileInputCoo);
		Matrix.ReadFromBinaryFile(fileInputCoo);
		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix.N, Matrix.NNZ);
		FPTYPE* v = new FPTYPE[Matrix.N];
		for (int i = 0; i < Matrix.N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{
			//	*CRS = CRSMatrix(Matrix->NNZ, Matrix->N);
			startTime = getCPUTime();
			Converters::COOToCRS(Matrix, CRS);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCRS: \t\t%.15f\n", endTime - startTime);
			//sprintf(fileInput, "CRS_%s.bin", name);
			CRS.WriteInBinaryFile(fileInput);
		}
		if (strcmp("r", act) == 0)
		{
			//sprintf(fileInput, "CRS_%s.bin", name);
			//CRS = CRS->ReadFromBinaryFile(fileInput);
			CRS.ReadFromBinaryFile(fileInput);
		}

		result_crs = new FPTYPE[Matrix.N];

		st = ReadTSC();
		CRS.MatrixVectorMultCRS(v, Matrix.N, result_crs);
		end = ReadTSC() - st;
		fprintf(fp, "Time Matrix-Vector multiplication in \n\nCRS: \t%10I64i\n", end);

		delete[] v;
		delete[] result_crs;

	}

	if (strcmp("CCS", typeMatrix) == 0)
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix.ReadFromBinaryFile(fileInput);
		sprintf(fileOutput, "CCS_%s_res.dt", name);
		fp = fopen(fileOutput, "w");
		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix.N, Matrix.NNZ);
		FPTYPE* v = new FPTYPE[Matrix.N];
		for (int i = 0; i < Matrix.N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{
			//*CCS = CCSMatrix(Matrix->NNZ, Matrix->N);
			startTime = getCPUTime();
			Converters::COOToCCS(Matrix, CCS);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCCS: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "CCS_%s.bin", name);
			CCS.WriteInBinaryFile(fileInput);
		}
		if (strcmp("r", act) == 0)
		{
			sprintf(fileInput, "CCS_%s.bin", name);
			CCS.ReadFromBinaryFile(fileInput);
		}

		result_ccs = new FPTYPE[Matrix.N];

		startTime = getCPUTime();
		CCS.MatrixVectorMultCCS(v, Matrix.N, result_ccs);
		endTime = getCPUTime();
		fprintf(fp, "Time Matrix-Vector multiplication in \n\nCCS: \t%.15f\n", endTime - startTime);

		delete[] v;
		delete[] result_ccs;

	}
	if (strcmp("CD", typeMatrix) == 0)
	{

		sprintf(fileInput, "CD_%s.bin", name);
		sprintf(fileInputCoo, "COO_%s.bin", name);
		sprintf(fileOutput, "CD_%s_res.dt", name);
		Matrix.ReadFromBinaryFile(fileInputCoo);
		CDdiag = Matrix.DiagCDMatrix(Matrix);
		fp = fopen(fileOutput, "w");
		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix.N, Matrix.NNZ);
		FPTYPE* v = new FPTYPE[Matrix.N];
		for (int i = 0; i < Matrix.N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{
			CD.FillDiagArray(Matrix);
			startTime = getCPUTime();
			Converters::COOToCD(Matrix, CD);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCD: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "CD_%s.bin", name);
			CD.WriteInBinaryFile(fileInput);
		}

		if (strcmp("r", act) == 0)
		{
			sprintf(fileInput, "CD_%s.bin", name);
			CD.ReadFromBinaryFile(fileInputCoo);
		}

		result_cd = new FPTYPE[Matrix.N];
		startTime = getCPUTime();
		CD.MatrixVectorMultCD(v, Matrix.N, result_cd);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCD: \t\t%lf\n", endTime - startTime);


		delete[] v;
		delete[] result_cd;
	}
	if (strcmp("JD", typeMatrix) == 0)
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix.ReadFromBinaryFile(fileInput);
		sprintf(fileOutput, "JD_%s_res.dt", name);
		fp = fopen(fileOutput, "w");
		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix.N, Matrix.NNZ);
		maxvalJD = Matrix.maxvalJDMatrix(Matrix);
		FPTYPE* v = new FPTYPE[Matrix.N];

		for (int i = 0; i < Matrix.N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{
			//maxvalJD = Matrix->maxvalJDMatrix(*Matrix);
			//*JD = JDMatrix(Matrix->NNZ, Matrix->N, maxvalJD);
			startTime = getCPUTime();
			Converters::COOToJD(Matrix, JD);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nJD: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "JD_%s.bin", name);
			JD.WriteInBinaryFile(fileInput);
		}
		if (strcmp("r", act) == 0)
		{
			sprintf(fileInput, "JD_%s.bin", name);
			JD.ReadFromBinaryFile(fileInput);

		}
		result_jd = new FPTYPE[Matrix.N];
		startTime = getCPUTime();
		JD.MatrixVectorMultJD(v, Matrix.N, result_jd);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nJD: \t\t%lf\n", endTime - startTime);

		delete[] v;
		delete[] result_jd;

	}
	if (strcmp("MKL", typeMatrix) == 0)
	{
		long long st, end;
		sprintf(fileOutput, "MKL_%s_res.dt", name);
		fp = fopen(fileOutput, "w");
		sprintf(fileInput, "CRS_%s.bin", name);
		CRS.ReadFromBinaryFile(fileInput);
		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", CRS.N, CRS.NNZ);
		FPTYPE* v = new FPTYPE[CRS.N];
		for (int i = 0; i < CRS.N; i++)
		{
			v[i] = 1;
		}
		for (int i = 0; i < CRS.N + 1; i++)
		{
			CRS.row_ptr[i]++;
		}
		for (int i = 0; i < CRS.NNZ; i++)
		{
			CRS.col_ind[i]++;
		}

		result_mkl = new FPTYPE[CRS.N];
		//startTime = getCPUTime();
		st = ReadTSC();
		mkl_dcsrgemv("N", &(CRS.N), CRS.val, CRS.row_ptr, CRS.col_ind, v, result_mkl);
		end = ReadTSC() - st;
		//endTime = getCPUTime();
		//fprintf(fp, "Time Matrix-Vector multiplication in \n\nMKL: \t%.15f\n", endTime - startTime);
		fprintf(fp, "Time Matrix-Vector multiplication in \n\nMKL: \t%10I64i\n", end);
	}


	if (strcmp("SL", typeMatrix) == 0)
	{

		sprintf(fileInput, "COO_%s.bin", name);
		Matrix.ReadFromBinaryFile(fileInput);
		//cout << "COO" << endl;
		//Matrix->PrintMatrix(Matrix->NNZ);
		sprintf(fileOutput, "SL_%s_res.dt", name);
		fp = fopen(fileOutput, "w");
		fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix.N, Matrix.NNZ);
		SLdiag = Matrix.diagSLMatrix(Matrix);
		FPTYPE* v = new FPTYPE[Matrix.N];

		for (int i = 0; i < Matrix.N; i++)
		{
			v[i] = 1;
		}

		if (strcmp("wr", act) == 0)
		{
			startTime = getCPUTime();
			Converters::COOToSL(Matrix, SL);
			endTime = getCPUTime();
			fprintf(fp, "\nTime convert SL: \t\t%lf\n", endTime - startTime);
			sprintf(fileInput, "SL_%s.bin", name);
			SL.WriteInBinaryFile(fileInput);
		}
		if (strcmp("r", act) == 0)
		{
			sprintf(fileInput, "SL_%s.bin", name);
			SL.ReadFromBinaryFile(fileInput);
		}


		result_sl = new FPTYPE[Matrix.N];

		startTime = getCPUTime();
		SL.MatrixVectorMultSL(v, Matrix.N, result_sl);
		endTime = getCPUTime();
		fprintf(fp, "Time Matrix-Vector multiplication in \n\nSL: \t%.15f\n", endTime - startTime);


		delete[] v;
		delete[] result_sl;
	}
	delete[] fileName;
	delete[] fileInput;
	delete[] fileOutput;
	delete[] typeMatrix;
	delete[] act;
	//	delete[] name;

	return 0;
}
