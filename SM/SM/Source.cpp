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
#include "CCS.h"
#include "CRS.h"
#include "COO.h"
#include "JD.h"
#include "CD.h"
#include "Converter.h"

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



//int compare(const void * x1, const void * x2)   
//{
//	return (*(int*)x1 - *(int*)x2);    
//
//}

//class COOMatrix
//{
//
//public:
//
//	INTTYPE N;
//	INTTYPE NNZ;
//	FPTYPE *val;
//	INTTYPE *row_ind;
//	INTTYPE *col_ind;
//	INTTYPE *NNZ_row;
//	INTTYPE diag_cd;
//	INTTYPE maxval_jd;
//
//	COOMatrix(INTTYPE  _NNZ, INTTYPE _N)
//	{
//		int i;
//		N = _N;
//		NNZ = _NNZ;
//		val = new FPTYPE[NNZ];
//		col_ind = new INTTYPE[NNZ];
//		row_ind = new INTTYPE[NNZ];
//		NNZ_row = new INTTYPE[N];
//		for (i = 0; i < N; i++)
//			NNZ_row[i] = 0;
//	}
//	~COOMatrix()
//	{
//		delete[] col_ind;
//		delete[] row_ind;
//		delete[] val;
//		delete[] NNZ_row;
//
//	}
//	COOMatrix(const COOMatrix &Matrix)
//	{
//		N = Matrix.N;
//		NNZ = Matrix.NNZ;
//		val = new FPTYPE[NNZ];
//		col_ind = new INTTYPE[NNZ];
//		row_ind = new INTTYPE[NNZ];
//		NNZ_row = new INTTYPE[N];
//		for (int i = 0; i < N + 1; i++)
//		{
//			NNZ_row[i] = Matrix.NNZ_row[i];
//		}
//
//		for (int j = 0; j < NNZ; j++)
//		{
//			col_ind[j] = Matrix.col_ind[j];
//			row_ind[j] = Matrix.row_ind[j];
//			val[j] = Matrix.val[j];
//		}
//	}
//	void PrintMatrix(int NNZ)
//	{
//		int i;
//		for (i = 0; i < NNZ; i++)
//		{
//			printf("(%lf,%d,%d) , ", val[i], row_ind[i], col_ind[i]);
//		}
//		printf("\n");
//	}
//
//	COOMatrix *ReadFromBinaryFile(char *filename)
//	{
//		FILE *COOmtx = NULL;
//		int N, NNZ;
//		COOmtx = fopen(filename, "rb");
//		if (COOmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fread(&N, sizeof(INTTYPE), 1, COOmtx);
//		fread(&NNZ, sizeof(INTTYPE), 1, COOmtx);
//		COOMatrix * Matrix = new COOMatrix(NNZ, N);
//		fread(Matrix->val, sizeof(FPTYPE), Matrix->NNZ, COOmtx);
//		fread(Matrix->col_ind, sizeof(INTTYPE), Matrix->NNZ, COOmtx);
//		fread(Matrix->row_ind, sizeof(INTTYPE), Matrix->NNZ, COOmtx);
//		fclose(COOmtx);
//		return Matrix;
//	}
//
//	void WriteInBinaryFile(const COOMatrix& Matrix, char* filename)
//	{
//		FILE *COOmtx = NULL;
//		COOmtx = fopen(filename, "wb");
//		if (COOmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fwrite(&Matrix.N, sizeof(INTTYPE), 1, COOmtx);
//		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, COOmtx);
//		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, COOmtx);
//		fwrite(Matrix.col_ind, sizeof(INTTYPE), Matrix.NNZ, COOmtx);
//		fwrite(Matrix.row_ind, sizeof(INTTYPE), Matrix.NNZ, COOmtx);
//		fclose(COOmtx);
//	}
//
//	INTTYPE DiagCDMatrix(const COOMatrix& Matrix)
//	{
//		int i, tmp_ind, m = 0, j;
//		bool flag;
//		NNZ = Matrix.NNZ;
//		N = Matrix.N;
//
//		INTTYPE *diag = new INTTYPE[2 * N - 1];
//
//		for (i = 0; i < 2 * N - 1; i++)
//		{
//			diag[i] = N + 1;
//		}
//		for (i = 0; i < NNZ; i++)
//		{
//			tmp_ind = Matrix.col_ind[i] - Matrix.row_ind[i];
//			for (j = 0; j < 2 * N - 1; j++)
//			{
//				if ((tmp_ind) != diag[j])
//				{
//					flag = true;
//				}
//				else
//				{
//					flag = false;
//					break;
//				}
//
//			}
//
//			if (flag == true)
//			{
//				diag[m] = tmp_ind;
//				m++;
//			}
//			
//
//		}
//
//		diag_cd = m;
//
//		delete[] diag;
//		return diag_cd;
//	}
//
//	INTTYPE maxvalJDMatrix(const COOMatrix& Matrix)
//	{
//		INTTYPE maxval;
//		maxval = Matrix.NNZ_row[0];
//		for (int i = 1; i< N; i++)
//		{
//			if (Matrix.NNZ_row[i]>maxval)
//			{
//				maxval = Matrix.NNZ_row[i];
//			}
//		}
//		maxval_jd = maxval;
//		return maxval;
//	}
//	INTTYPE diagSLMatrix(const COOMatrix& Matrix)
//	{
//		int diagelem = 0;
//		for (int i = 0; i < NNZ; i++)
//		{
//			if (Matrix.row_ind[i] == Matrix.col_ind[i])
//				diagelem++;
//		}
//		return diagelem;
//	}
//
//};
//
//
//
//class CRSMatrix
//{
//public:
//	INTTYPE N;
//	INTTYPE NNZ;
//	FPTYPE *val;
//	INTTYPE *col_ind;
//	INTTYPE *row_ptr;
//
//
//	CRSMatrix(INTTYPE _NNZ, INTTYPE _N)
//	{
//		N = _N;
//		NNZ = _NNZ;
//		val = new FPTYPE[NNZ];
//		col_ind = new INTTYPE[NNZ];
//		row_ptr = new INTTYPE[N + 1];
//		for (int i = 0; i < N + 1; i++)
//		{
//			row_ptr[i] = 0;
//		}
//	}
//
//	CRSMatrix(const CRSMatrix& Matrix)
//	{
//		N = Matrix.N;
//		NNZ = Matrix.NNZ;
//		val = new FPTYPE[NNZ];
//		col_ind = new INTTYPE[NNZ];
//		row_ptr = new INTTYPE[N + 1];
//		for (int i = 0; i < N + 1; i++)
//		{
//			row_ptr[i] = Matrix.row_ptr[i];
//		}
//
//		for (int j = 0; j < NNZ; j++)
//		{
//			col_ind[j] = Matrix.col_ind[j];
//			val[j] = Matrix.val[j];
//		}
//
//	}
//
//	~CRSMatrix() {
//		delete[] col_ind;
//		delete[] row_ptr;
//		delete[] val;
//
//	}
//
//	CRSMatrix *ReadFromBinaryFile(char *filename)
//	{
//		FILE *CRSmtx = NULL;
//		int N, NNZ;
//		CRSmtx = fopen("COOmtx.bin", "rb");
//		if (CRSmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fread(&N, sizeof(INTTYPE), 1, CRSmtx);
//		fread(&NNZ, sizeof(INTTYPE), 1, CRSmtx);
//		CRSMatrix * Matrix = new CRSMatrix(NNZ, N);
//		fread(Matrix->val, sizeof(FPTYPE), Matrix->NNZ, CRSmtx);
//		fread(Matrix->col_ind, sizeof(INTTYPE), Matrix->NNZ, CRSmtx);
//		fread(Matrix->row_ptr, sizeof(INTTYPE), Matrix->N +1, CRSmtx);
//		fclose(CRSmtx);
//		return Matrix;
//	}
//
//	void WriteInBinaryFile(const CRSMatrix& Matrix, char* filename)
//	{
//		FILE *CRSmtx = NULL;
//		CRSmtx = fopen(filename, "wb");
//		if (CRSmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fwrite(&Matrix.N, sizeof(INTTYPE), 1, CRSmtx);
//		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, CRSmtx);
//		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, CRSmtx);
//		fwrite(Matrix.col_ind, sizeof(INTTYPE), Matrix.NNZ, CRSmtx);
//		fwrite(Matrix.row_ptr, sizeof(INTTYPE), Matrix.N+1, CRSmtx);
//		fclose(CRSmtx);
//	}
//
//	void PrintMatrix(int _NNZ, int _N)
//	{
//		int i;
//		N = _N;
//		NNZ = _NNZ;
//		printf("val:");
//		for (i = 0; i < NNZ; i++)
//			printf("%lf , ", val[i]);
//		printf(" \n col_ind:");
//		for (i = 0; i < NNZ; i++)
//			printf("%d , ", col_ind[i]);
//		printf(" \n row_ptr:");
//		for (i = 0; i < N + 1; i++)
//			printf("%d , ", row_ptr[i]);
//		printf("\n");
//	}
//	CRSMatrix& operator=(const CRSMatrix &Matrix) 
//	{
//		int i;
//		if (this != &Matrix) {
//			delete[] val;
//			delete[] col_ind;
//			delete[] row_ptr;
//			N = Matrix.N;
//			NNZ = Matrix.NNZ;
//			
//			val = new FPTYPE[NNZ];
//			col_ind = new INTTYPE[NNZ];
//			row_ptr = new INTTYPE[N + 1];
//			for (i = 0; i < NNZ; i++)
//			{
//				col_ind[i] = Matrix.col_ind[i];
//				val[i] = Matrix.val[i];
//			}
//			for (i = 0; i < N + 1; i++)
//				row_ptr[i] = Matrix.row_ptr[i];
//
//		}
//		return *this;
//	}
//	FPTYPE* MatrixVectorMultCRS(CRSMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
//	{
//		int i, j;
//		double tmp;
//		for (i = 0; i < N; i++)
//		{
//			result[i] = 0;
//		}
//		if (N == Matrix->N)
//		{
//			for (i = 0; i < N; i++)
//			{
//				tmp = 0;
//				for (j = Matrix->row_ptr[i]; j < Matrix->row_ptr[i + 1]; j++)
//				{
//					tmp += Matrix->val[j] * vec[Matrix->col_ind[j]];
//				}
//				result[i] = tmp;
//			}
//		}
//		return result;
//	}
//
//};
//
//class CCSMatrix
//{
//public:
//	INTTYPE N;
//	INTTYPE NNZ;
//	FPTYPE *val;
//	INTTYPE *row_ind;
//	INTTYPE *col_ptr;
//
//
//	CCSMatrix(INTTYPE _NNZ, INTTYPE _N)
//	{
//		N = _N;
//		NNZ = _NNZ;
//		val = new FPTYPE[NNZ];
//		row_ind = new INTTYPE[NNZ];
//		col_ptr = new INTTYPE[N + 1];
//		for (int i = 0; i < N + 1; i++)
//		{
//			col_ptr[i] = 0;
//		}
//	}
//
//
//	CCSMatrix(const CCSMatrix& Matrix)
//	{
//		N = Matrix.N;
//		NNZ = Matrix.NNZ;
//		val = new FPTYPE[NNZ];
//		row_ind = new INTTYPE[NNZ];
//		col_ptr = new INTTYPE[N + 1];
//		for (int i = 0; i < N + 1; i++)
//		{
//			col_ptr[i] = Matrix.col_ptr[i];
//		}
//
//		for (int j = 0; j < NNZ; j++)
//		{
//			row_ind = &Matrix.row_ind[j];
//			val = &Matrix.val[j];
//		}
//
//	}
//
//	   ~CCSMatrix() {
//		delete[] row_ind;
//		delete[] col_ptr;
//		delete[] val;
//
//	}
//
//	CCSMatrix* ReadFromBinaryFile(char *filename)
//	{
//		FILE *CCSmtx = NULL;
//		int N, NNZ;
//		CCSmtx = fopen("CCSmtx.bin", "rb");
//		if (CCSmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fread(&N, sizeof(INTTYPE), 1, CCSmtx);
//		fread(&NNZ, sizeof(INTTYPE), 1, CCSmtx);
//		CCSMatrix * Matrix = new CCSMatrix(NNZ, N);
//		fread(Matrix->val, sizeof(FPTYPE), Matrix->NNZ, CCSmtx);
//		fread(Matrix->row_ind, sizeof(INTTYPE), Matrix->NNZ, CCSmtx);
//		fread(Matrix->col_ptr, sizeof(INTTYPE), Matrix->N + 1, CCSmtx);
//		fclose(CCSmtx);
//		return Matrix;
//	}
//
//	void WriteInBinaryFile(CCSMatrix& Matrix, char* filename)
//	{
//		FILE *CCSmtx = NULL;
//		CCSmtx = fopen(filename, "wb");
//		if (CCSmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fwrite(&Matrix.N, sizeof(INTTYPE), 1, CCSmtx);
//		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, CCSmtx);
//		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, CCSmtx);
//		fwrite(Matrix.row_ind, sizeof(INTTYPE), Matrix.NNZ, CCSmtx);
//		fwrite(Matrix.col_ptr, sizeof(INTTYPE), Matrix.N + 1, CCSmtx);
//		fclose(CCSmtx);
//	}
//	CCSMatrix& operator=(const CCSMatrix &Matrix) 
//	{
//		int i;
//		if (this != &Matrix) {
//			delete[] val;
//			delete[] row_ind;
//			delete[] col_ptr;
//			N = Matrix.N;
//			NNZ = Matrix.NNZ;
//
//			val = new FPTYPE[NNZ];
//			row_ind = new INTTYPE[NNZ];
//			col_ptr = new INTTYPE[N + 1];
//			for (i = 0; i < NNZ; i++)
//			{
//				row_ind[i] = Matrix.row_ind[i];
//				val[i] = Matrix.val[i];
//			}
//			for (i = 0; i < N + 1; i++)
//				col_ptr[i] = Matrix.col_ptr[i];
//
//		}
//		return *this;
//	}
//	FPTYPE* MatrixVectorMultCCS(CCSMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
//	{
//		int i, j, k;
//		for (k = 0; k < N; k++)
//		{
//			result[k] = 0;
//		}
//		if (N == Matrix->N)
//		{
//			for (i = 0; i < N; i++)
//			{
//				for (j = Matrix->col_ptr[i]; j < Matrix->col_ptr[i + 1]; j++)
//				{
//					result[Matrix->row_ind[j]] += vec[i] * Matrix->val[j];
//				}
//			}
//		}
//		return result;
//	}
//
//};
//
//class CDMatrix
//{
//public:
//	INTTYPE N;
//	INTTYPE NNZ;
//
//	INTTYPE B;
//	INTTYPE* diag;
//	FPTYPE** val;
//
//	CDMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _B)
//	{
//		int i;
//		int j;
//		N = _N;
//		NNZ = _NNZ;
//		B = _B;
//		diag = new INTTYPE[B];
//		val = new FPTYPE*[N];
//		for (i = 0; i < N; i++)
//		{
//			val[i] = new FPTYPE[B];
//		}
//		for (i = 0; i < N; i++) {
//			for (j = 0; j < B; j++)
//				val[i][j] = 0;
//		}
//
//	}
//	void Print()
//	{
//		int i, j;
//		printf("val:");
//		for (i = 0; i < B; i++)
//		{
//			printf("diag  \n", i);
//			for (j = 0; j <N; j++)
//				printf("%lf , ", val[i][j]);
//		}
//		printf(" \n diag:");
//		for (i = 0; i < B; i++)
//			printf("%d , ", diag[i]);
//
//		printf("\n");
//	}
//	~CDMatrix()
//	{
//		delete[] diag;
//		for (int i = 0; i < N; i++)
//			delete[] val[i];
//		delete[] val;
//	}
//	CDMatrix(const CDMatrix &Matrix)
//	{
//		int i;
//		N = Matrix.N;
//		NNZ = Matrix.NNZ;
//		B = Matrix.B;
//		diag = new INTTYPE[B];
//		val = new FPTYPE*[N];
//		for (i = 0; i < N; i++)
//		{
//			val[i] = new FPTYPE[Matrix.B];
//		}
//		for (i = 0; i < B; i++)
//		{
//			diag[i] = Matrix.diag[i];
//		}
//		for (i = 0; i < N ; i++)
//			for (int j = 0; j < B; j++)
//			{
//				val[i][j] = Matrix.val[i][j];
//			}
//	}
//	CDMatrix *ReadFromBinaryFile(char *filename)
//	{
//		FILE *CDmtx = NULL;
//		int N, NNZ;
//		CDmtx = fopen("CDmtx.bin", "rb");
//		if (CDmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fread(&N, sizeof(INTTYPE), 1, CDmtx);
//		fread(&NNZ, sizeof(INTTYPE), 1, CDmtx);
//		fread(&B, sizeof(INTTYPE), 1, CDmtx);
//		CDMatrix * Matrix = new CDMatrix(NNZ, N, B);
//		fread(Matrix->diag, sizeof(INTTYPE), Matrix->B, CDmtx);
//		fread(Matrix->val, sizeof(FPTYPE), Matrix->B * Matrix->N, CDmtx);
//		fclose(CDmtx);
//		return Matrix;
//	}
//	void FillDiagArray(COOMatrix Matrix)
//	{
//		int i, j, tmp_ind, m=0, diag_numb;
//		bool flag;
//
//		INTTYPE *temp = new INTTYPE[2 * N - 1];
//		
//		for (i = 0; i < 2 * N - 1; i++)
//		{
//			//diag[i] = N + 1;
//			temp[i] = N + 1;
//		}
//		for (i = 0; i < NNZ; i++)
//		{
//			tmp_ind = Matrix.col_ind[i] - Matrix.row_ind[i];
//
//			for (j = 0; j < 2 * N - 1; j++)
//			{
//				//if ((tmp_ind) != diag[j])
//				if ((tmp_ind) != temp[j])
//				{
//					flag = true;
//				}
//				else
//				{
//					flag = false;
//					break;
//				}
//
//			}
//
//			if (flag == true)
//			{
//				//diag[m++] = tmp_ind;
//				temp[m++] = tmp_ind;
//			}
//
//		}
//
//		diag_numb = m;
//		qsort(&temp[0], m, sizeof(INTTYPE), compare);
//		for (int i = 0; i < m; i++)
//		{
//			diag[i] = temp[i];
//		}
//	}
//	void WriteInBinaryFile(CDMatrix Matrix)
//	{
//		FILE *CDmtx = NULL;
//		CDmtx = fopen("CDmtx.bin", "wb");
//		if (CDmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fwrite(&Matrix.N, sizeof(INTTYPE), 1, CDmtx);
//		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, CDmtx);
//		fwrite(&Matrix.B, sizeof(INTTYPE), 1, CDmtx);
//		fwrite(Matrix.diag, sizeof(INTTYPE), Matrix.B, CDmtx);
//		fwrite(Matrix.val, sizeof(INTTYPE), Matrix.B * Matrix.N, CDmtx);
//		fclose(CDmtx);
//	}
//	FPTYPE* MatrixVectorMultCD(CDMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
//	{
//		int i, j;
//		int tmp;
//		int max;
//		int B = Matrix->B;
//		for (i = 0; i < N; i++)
//		{
//			result[i] = 0;
//		}
//
//		for (j = 0; j < B; j++) {
//			tmp = Matrix->diag[j];
//			if (0 >(0 - tmp))
//				i = 0;
//			else i = 0 - tmp;
//
//			if (0 > tmp)
//				max = Matrix->N - 0;
//			else max = Matrix->N - tmp;
//
//			for (i; i < max; i++)
//				result[i] += Matrix->val[i][j] * vec[tmp + i];
//
//		}
//		return result;
//	}
//
//};
//
//class JDMatrix
//{
//public:
//	INTTYPE NNZ;
//	INTTYPE N;
//	INTTYPE numbdiag;
//	INTTYPE *perm;
//	FPTYPE *jdiag;
//	INTTYPE *col_ind;
//	INTTYPE *jd_ptr;
//	INTTYPE MaxNNZ;
//
//	JDMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _MaxNNZ)
//	{
//		int i;
//		N = _N;
//		NNZ = _NNZ;
//		MaxNNZ = _MaxNNZ;
//		numbdiag = _MaxNNZ;
//		jdiag = new FPTYPE[NNZ];
//		perm = new INTTYPE[N];
//		col_ind = new INTTYPE[NNZ];
//		jd_ptr = new INTTYPE[N + 1];
//		for (i = 0; i < N + 1; i++)
//		{
//			jd_ptr[i] = 0;
//		}
//		for (i = 0; i < NNZ; i++)
//		{
//			jdiag[i] = 0;
//		}
//	}
//	~JDMatrix()
//	{
//		delete[] perm;
//		delete[] jdiag;
//		delete[] col_ind;
//		delete[] jd_ptr;
//
//	}
//	JDMatrix(const JDMatrix &Matrix)
//	{
//		//
//	}
//	JDMatrix *ReadFromBinaryFile(char *filename)
//	{
//		FILE *JDmtx = NULL;
//		int N, NNZ;
//		JDmtx = fopen("JDmtx.bin", "rb");
//		if (JDmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fread(&N, sizeof(INTTYPE), 1, JDmtx);
//		fread(&NNZ, sizeof(INTTYPE), 1, JDmtx);
//		fread(&MaxNNZ, sizeof(INTTYPE), 1, JDmtx);
//		JDMatrix * Matrix = new JDMatrix(NNZ, N, MaxNNZ);
//		fread(Matrix->jdiag, sizeof(FPTYPE), Matrix->NNZ, JDmtx);
//		fread(Matrix->col_ind, sizeof(INTTYPE), Matrix->NNZ, JDmtx);
//		fread(Matrix->jd_ptr, sizeof(INTTYPE), Matrix->N + 1, JDmtx);
//		fread(Matrix->perm, sizeof(INTTYPE), Matrix->N, JDmtx);
//		fclose(JDmtx);
//		return Matrix;
//	
//	}
//	void WriteInBinaryFile(JDMatrix Matrix)
//	{
//
//		FILE *JDmtx = NULL;
//		JDmtx = fopen("JDmtx.bin", "wb");
//		if (JDmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fwrite(&Matrix.N, sizeof(INTTYPE), 1, JDmtx);
//		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, JDmtx);
//		fwrite(&Matrix.MaxNNZ, sizeof(INTTYPE), 1, JDmtx);
//		fwrite(Matrix.jdiag, sizeof(FPTYPE), Matrix.NNZ, JDmtx);
//		fwrite(Matrix.col_ind, sizeof(INTTYPE), Matrix.NNZ, JDmtx);
//		fwrite(Matrix.jd_ptr, sizeof(INTTYPE), Matrix.N + 1, JDmtx);
//		fwrite(Matrix.perm, sizeof(INTTYPE), Matrix.N, JDmtx);
//		fclose(JDmtx);
//	}
//	FPTYPE* MatrixVectorMultJD(JDMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
//	{
//		int i = 0, j = 0, k = 0, NNZ = 0, tmp_ind = 0, maxval = 0, upper = 0;
//		NNZ = Matrix->NNZ;
//
//		FPTYPE* temp = new FPTYPE[N];
//		for (i = 0; i < N; i++)
//		{
//			result[i] = 0;
//		}
//
//		int disp = 0;
//		for (int j = 0; j < Matrix->MaxNNZ; j++)
//		{
//
//			for (int i = 0; i < (Matrix->jd_ptr[j + 1] - Matrix->jd_ptr[j]); i++)
//			{
//				result[i] += Matrix->jdiag[disp] * vec[Matrix->col_ind[disp]];
//				disp++;
//			}
//		}
//
//
//		for (i = 0; i < N; i++)
//		{
//			temp[Matrix->perm[i]] = result[i];
//		}
//		for (i = 0; i < N; i++)
//		{
//			result[i] = temp[i];
//		}
//
//		free(temp);
//		return result;
//	}
//
//
//};
//
//class SLMatrix
//{
//public:
//
//	INTTYPE N;
//	INTTYPE NNZ;
//	INTTYPE diag;
//	FPTYPE *adiag;
//	FPTYPE *altr;
//	FPTYPE *autr;
//	INTTYPE *jptr;
//	INTTYPE *iptr;
//
//	SLMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _diag)
//	{
//		int i;
//		N = _N;
//		NNZ = _NNZ;
//		diag = _diag;
//		adiag = new FPTYPE[N];
//		altr = new FPTYPE[(NNZ - diag) / 2];
//		autr = new FPTYPE[(NNZ - diag) / 2];
//		jptr = new INTTYPE[(NNZ - diag) / 2];
//		iptr = new INTTYPE[N + 1];
//		for (i = 0; i < (NNZ - diag) / 2; i++)
//		{
//			altr[i] = 0;
//			autr[i] = 0;
//			jptr[i] = 0;
//		}
//		for (i = 0; i < N; i++)
//		{
//			adiag[i] = 0;
//		}
//		for (i = 0; i < N + 1; i++)
//		{
//			iptr[i] = 0;
//		}
//	}
//	~SLMatrix()
//	{
//		delete[] autr;
//		delete[] altr;
//		delete[] iptr;
//		delete[] jptr;
//		delete[] adiag;
//	}
//	SLMatrix(const COOMatrix &Matrix)
//	{
//		
//	}
//	SLMatrix *ReadFromBinaryFile(char *filename)
//	{
//
//		FILE *SLmtx = NULL;
//		int N, NNZ;
//		SLmtx = fopen("SLmtx.bin", "rb");
//		if (SLmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fread(&N, sizeof(INTTYPE), 1, SLmtx);
//		fread(&NNZ, sizeof(INTTYPE), 1, SLmtx);
//		fread(&diag, sizeof(INTTYPE), 1, SLmtx);
//		SLMatrix * Matrix = new SLMatrix(NNZ, N, diag);
//		fread(Matrix->adiag, sizeof(FPTYPE), Matrix->N, SLmtx);
//		fread(Matrix->altr, sizeof(FPTYPE), (Matrix->NNZ - Matrix->diag) / 2, SLmtx);
//		fread(Matrix->autr, sizeof(FPTYPE), (Matrix->NNZ - Matrix->diag) / 2, SLmtx);
//		fread(Matrix->jptr, sizeof(INTTYPE), (Matrix->NNZ - Matrix->diag) / 2, SLmtx);
//		fread(Matrix->iptr, sizeof(INTTYPE), Matrix->N + 1, SLmtx);
//		fclose(SLmtx);
//		return Matrix;
//	
//	}
//	void WriteInBinaryFile(SLMatrix Matrix)
//	{
//		FILE *SLmtx = NULL;
//		SLmtx = fopen("SLmtx.bin", "wb");
//		if (SLmtx == NULL)
//		{
//			printf("Error opening file");
//		}
//		fwrite(&Matrix.N, sizeof(INTTYPE), 1, SLmtx);
//		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, SLmtx);
//		fwrite(&Matrix.diag, sizeof(INTTYPE), 1, SLmtx);
//		fwrite(Matrix.adiag, sizeof(FPTYPE), Matrix.N, SLmtx);
//		fwrite(Matrix.altr, sizeof(FPTYPE), (Matrix.NNZ - Matrix.diag)/2, SLmtx);
//		fwrite(Matrix.autr, sizeof(FPTYPE), (Matrix.NNZ - Matrix.diag) / 2, SLmtx);
//		fwrite(Matrix.jptr, sizeof(INTTYPE), (Matrix.NNZ - Matrix.diag) / 2, SLmtx);
//		fwrite(Matrix.iptr, sizeof(INTTYPE), Matrix.N + 1, SLmtx);
//		fclose(SLmtx);
//	}
//	FPTYPE* MatrixVectorMultSL(SLMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
//	{
//		int i, j;
//		for (i = 0; i < N; i++)
//		{
//			result[i] = 0;
//		}
//		for (i = 0; i < N; i++)
//		{
//			result[i] = vec[i] * Matrix->adiag[i];
//		}
//		for (i = 1; i < N; i++)
//		{
//
//			for (j = Matrix->iptr[i]; j < Matrix->iptr[i + 1] - 1; j++)
//			for (j = Matrix->iptr[i]; j < Matrix->iptr[i + 1]; j++)
//			{
//				result[i] += vec[Matrix->jptr[j]] * Matrix->altr[j];
//				result[Matrix->jptr[j]] += vec[i] * Matrix->autr[j];
//			}
//		}
//	return result;
//	}
//};

//class Converters
//{
//public:
//	
//		static void COOToCRS(const COOMatrix &Mtx, CRSMatrix &Matrix)
//		{
//			int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0, n = 0, m = 0;
//			NNZ = Mtx.NNZ;
//			N = Mtx.N;
//			vector< vector<int> > col_ind_;
//			col_ind_.resize(N);
//			vector< vector<double> > val_;
//			val_.resize(N);
//
//			for (i = 0; i < NNZ; i++)
//			{
//				val_[Mtx.row_ind[i]].push_back(Mtx.val[i]);
//				col_ind_[Mtx.row_ind[i]].push_back(Mtx.col_ind[i]);
//				Matrix.row_ptr[Mtx.row_ind[i] + 1]++;
//			}
//
//			for (k = 2; k < N + 1; k++)
//			{
//				Matrix.row_ptr[k] += Matrix.row_ptr[k - 1];
//			}
//
//			for (m = 0; m < N; m++) {
//				k = 0;
//				for (n = Matrix.row_ptr[m]; n < Matrix.row_ptr[m + 1]; n++)
//				{
//					Matrix.val[n] = val_[m][k];
//					Matrix.col_ind[n] = col_ind_[m][k];
//					k++;
//				}
//			}
//		}
//
//		static void COOToCCS(const COOMatrix &Mtx, CCSMatrix &Matrix)
//		{
//			int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0;
//			NNZ = Mtx.NNZ;
//			N = Mtx.N;
//
//			for (i = 0; i < NNZ; i++)
//			{
//				Matrix.val[i] = Mtx.val[i];
//				Matrix.row_ind[i] = Mtx.row_ind[i];
//			}
//
//			for (j = 0; j < NNZ; j++)
//			{
//				tmp_ind = Mtx.col_ind[j];
//				Matrix.col_ptr[++tmp_ind]++;
//			}
//			while (j < NNZ);
//
//			for (k = 2; k < N + 1; k++)
//			{
//				Matrix.col_ptr[k] += Matrix.col_ptr[k - 1];
//			}
//		}
//
//		static void COOToCD(const COOMatrix &Mtx, CDMatrix &Matrix)
//		{
//			int i = 0, j = 0, n, k = 0, l = 0, NNZ = 0, N = 0, diag_ind = 0, B = 0, tmp_ind = 0, m = 0, diag_numb = 0;
//			NNZ = Mtx.NNZ;
//			N = Mtx.N;
//			diag_numb = Matrix.B;
//
//			vector< vector<FPTYPE> > val_;
//			val_.resize(diag_numb);
//
//			vector< vector<INTTYPE> > col_ind_;
//			col_ind_.resize(diag_numb);
//			vector< vector<INTTYPE> > row_ind_;
//			row_ind_.resize(diag_numb);
//
//			for (int i = 0; i < diag_numb; i++)
//			{
//				val_[i].push_back(Matrix.diag[i]);
//			}
//
//			for (i = 0; i < NNZ; i++)
//			{
//				tmp_ind = Mtx.col_ind[i] - Mtx.row_ind[i];
//				for (j = 0; j < diag_numb; j++)
//				{
//					if (tmp_ind == val_[j][0])
//					{
//						val_[j].push_back(Mtx.val[i]);
//						if (tmp_ind >= 0) {
//							//val_[j].push_back(Mtx.val[i]);
//							//new
//							col_ind_[j].push_back(Mtx.col_ind[i]);
//						}
//						else
//						{
//							row_ind_[j].push_back(Mtx.row_ind[i]);
//						}
//					}
//				}
//			}
//			INTTYPE row_ind;
//			for (m = 0; m < diag_numb; m++) {
//				//for (n = diag_ptr[m]; n < diag_ptr[m + 1]; n++)
//				for (n = 1; n < val_[m].size(); n++)
//				{
//					if (Matrix.diag[m] < 0)
//					{
//						row_ind = row_ind_[m][n - 1];
//						Matrix.val[row_ind][m] = val_[m][n];
//
//					}
//					else
//					{
//						row_ind = col_ind_[m][n - 1];
//						Matrix.val[row_ind][m] = val_[m][n];
//					}
//					//p++;
//					//k++;
//				}
//			}
//
//		//	delete[] diag_ptr;
//		}
//
//		static void COOToJD(const COOMatrix &Mtx, JDMatrix &Matrix)
//		{
//			int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0, maxval = 0;
//			NNZ = Mtx.NNZ;
//			N = Mtx.N;
//			maxval = Matrix.MaxNNZ;
//			vector< vector<INTTYPE> > col_ind_;
//			col_ind_.resize(N);
//			vector< vector<FPTYPE> > val_;
//			val_.resize(N);
//			INTTYPE *nnz_row = new INTTYPE[N];
//			INTTYPE *nnz_col = new INTTYPE[N];
//			INTTYPE *nnz_row_ind = new INTTYPE[N];
//
//			for (i = 0; i < N; i++)
//			{
//				nnz_row[i] = Mtx.NNZ_row[i];
//				nnz_row_ind[i] = i;
//				nnz_col[i] = 0;
//			}
//
//			qs(nnz_row, nnz_row_ind, 0, N - 1);
//
//			for (i = 0; i < N; i++)
//			{
//				Matrix.perm[i] = nnz_row_ind[i];
//			}
//			for (i = 0; i < NNZ; i++)
//			{
//				val_[Mtx.row_ind[i]].push_back(Mtx.val[i]);
//				col_ind_[Mtx.row_ind[i]].push_back(Mtx.col_ind[i]);
//			}
//			for (int i = 0; i < N; i++)
//			{
//				int temp_nnz_col = val_[i].size();
//				for (int k = 0; k < temp_nnz_col; k++)
//				{
//					nnz_col[k]++;
//				}
//			}
//
//			int l = 0;
//			int m = 0;
//			for (i = 0; i < N; i++)
//			{
//				//m++;
//				for (j = 0; j < nnz_col[i]; j++)
//				{
//					Matrix.jdiag[l] = val_[Matrix.perm[j]][i];
//					Matrix.col_ind[l] = col_ind_[Matrix.perm[j]][i];
//					l++;
//				}
//			}
//			Matrix.jd_ptr[0] = 0;
//			for (i = 1; i < N + 1; i++)
//			{
//				Matrix.jd_ptr[i] = nnz_col[i - 1];
//			}
//
//			for (i = 2; i < N + 1; i++)
//			{
//				Matrix.jd_ptr[i] += Matrix.jd_ptr[i - 1];
//			}
//
//			delete[] nnz_row;
//			delete[] nnz_col;
//			delete[] nnz_row_ind;
//
//		}
//
//		static void COOToSL(const COOMatrix &Mtx, SLMatrix &Matrix)
//		{
//			int j = 0, k = 0, l = 0, tmp_ind = 0, m = 0, p = 0;
//			int NNZ = Mtx.NNZ;
//			int N = Mtx.N;
//			vector< vector<FPTYPE> > vec;
//			vec.resize(N-1);
//
//			INTTYPE* elem_before_diag = new INTTYPE[N-1];
//			for (int i = 0; i < N-1; i++)
//			{
//				elem_before_diag[i] = 0;
//			}
//			int g = 0, h = 0, o = 0;
//
//			j = 0; l = 0;
//			int pos = 0;
//			for (int i = 0; i < NNZ; i++)
//			{
//				if (Mtx.col_ind[i] < Mtx.row_ind[i])
//				{
//					vec[Mtx.row_ind[i] - 1].push_back(Mtx.val[i]);
//					elem_before_diag[Mtx.row_ind[i] - 1]++;
//					o++;
//				}
//				else
//				{
//					if (Mtx.col_ind[i] == Mtx.row_ind[i])
//					{
//						Matrix.adiag[Mtx.col_ind[i]] = Mtx.val[i];
//						g++;
//					}
//					else
//					{
//						Matrix.autr[pos] = Mtx.val[i];
//						Matrix.jptr[pos] = Mtx.row_ind[i];
//						h++; pos++;
//					}
//				}
//			}
//			printf("diag %d, upper %d, low %d", g, h, o);
//			system("pause");
//			for (int i = 0; i < N-1; i++)
//			{
//				if (vec[i].size()!= 0)
//				{
//					for (int j = 0; j < vec.size(); j++)
//					{
//						Matrix.altr[m] = vec[i][j];
//						m++;
//					}
//				}
//			}
//
//			for (p = 2; p < N + 1; p++)
//			{
//				Matrix.iptr[p] += Matrix.iptr[p - 1];
//			}
//			delete[] elem_before_diag;
//		}
//};

//void ReadMatrixInfo(COOMatrix& Matrix, char *name)
//{
//	FILE* f;
//	int i;
//	char* line;
//	char* p = NULL;
//
//
//	f = fopen(name, "r");
//	if (f == NULL)
//	{
//		printf("%s- File Not Found!\n", name);
//	}
//	line = new char[MAX_LINE_LEN];
//	do
//		fgets(line, MAX_LINE_LEN, f);
//	while (line[0] == '%');
//	p = strtok(line, " ");
//	p = strtok(NULL, " ");
//	p = strtok(NULL, " ");
//	for (i = 0; i < Matrix.NNZ; i++)
//	{
//		fgets(line, MAX_LINE_LEN, f);
//		p = strtok(line, " ");
//		Matrix.row_ind[i] = atoi(p) - 1;
//		Matrix.NNZ_row[Matrix.row_ind[i]]++;
//		p = strtok(NULL, " ");
//		Matrix.col_ind[i] = atoi(p) - 1;
//		p = strtok(NULL, " ");
//		Matrix.val[i] = atof(p);
//	}
//	delete[] line;
//	fclose(f);
//}
//COOMatrix *ReadMatrix(char *name)
//{
//	FILE* f;
//	char* line;
//	char* p = NULL;
//	int N, NNZ, i;
//	f = fopen(name, "r");
//	if (f == NULL)
//		printf("%s- File Not Found!\n", name);
//	line = new char[MAX_LINE_LEN];
//	do
//		fgets(line, MAX_LINE_LEN, f);
//	while (line[0] == '%');
//	p = strtok(line, " ");
//	N = atoi(p);
//	p = strtok(NULL, " ");
//	p = strtok(NULL, " ");
//	NNZ = atoi(p);
//	COOMatrix * Matrix = new COOMatrix(NNZ, N);
//	for (i = 0; i < Matrix->NNZ; i++)
//	{
//		fgets(line, MAX_LINE_LEN, f);
//		p = strtok(line, " ");
//		Matrix->row_ind[i] = atoi(p) - 1;
//		Matrix->NNZ_row[Matrix->row_ind[i]]++;
//		p = strtok(NULL, " ");
//		Matrix->col_ind[i] = atoi(p) - 1;
//		p = strtok(NULL, " ");
//		Matrix->val[i] = atof(p);
//	}
//	delete[] line;
//	fclose(f);
//	return Matrix;
//}
//void ReadNumberForMatrix(int& N, int& NNZ, char *name)
//{
//	FILE* f;
//	char* line;
//	char* p = NULL;
//
//	f = fopen(name, "r");
//	if (f == NULL)
//		printf("%s- File Not Found!\n", name);
//	line = new char[MAX_LINE_LEN];
//	do
//		fgets(line, MAX_LINE_LEN, f);
//	while (line[0] == '%');
//	p = strtok(line, " ");
//	N = atoi(p);
//	p = strtok(NULL, " ");
//	p = strtok(NULL, " ");
//	NNZ = atoi(p);
//	delete[] line;
//}
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

////int main(int argc, char** argv)
//int main()
//{
//	FPTYPE startTime, endTime;
//	static bool COORead = false;
//	static bool COOBinary = false;
//	static bool CRSBinary = false;
//	static bool CCSBinary = false;
//	static bool CDBinary = false;
//	static bool JDBinary = false;
//	static bool SLBinary = false;
//
//	char *fileName;
//	//double timer;
//	INTTYPE CDdiag;
//	INTTYPE maxvalJD;
//	INTTYPE diagelemSL;
//
//	FILE *fp = fopen("data.dt", "w");
//	FPTYPE* v;
//	FPTYPE* result_crs;
//	FPTYPE* result_ccs;
//	FPTYPE* result_cd;
//	FPTYPE* result_jd;
//	FPTYPE* result_sl;
//	FPTYPE* result_mkl;
//
//	COOMatrix* Matrix;
//	CRSMatrix *CRS;
//	CCSMatrix *CCS;
//	CDMatrix *CD;
//	JDMatrix *JD;
//	SLMatrix *SL;
//
//	fileName = new char[256];
//	fileName = "C:/Users/Vika/Documents/GitHub/SparceMatrix-new/SM/SM/Lin.mtx";
////	strcpy(fileName, argv[1]);
//	fprintf(fp, "Matrix file name: %s\n\n", fileName);
//
//
//	//	Matrix->ReadFromBinaryFile("COO.bin");
//
//	/*	if (COORead == false)
//		{*/
//	startTime = getCPUTime();
//	Matrix = ReadMatrix(fileName);
//	endTime = getCPUTime();
//	fprintf(fp, "Time Read Matrix Info: \t%lf\n\n", endTime - startTime);
//	fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
//	
//	//Matrix->WriteInBinaryFile(*Matrix);
//	//COORead = true;
//	//}
//
//
//
//	CRS = new CRSMatrix(Matrix->NNZ, Matrix->N);
//	CCS = new CCSMatrix(Matrix->NNZ, Matrix->N);
//	//CDdiag = Matrix->DiagCDMatrix(*Matrix);
//	maxvalJD = Matrix->maxvalJDMatrix(*Matrix);
//	//CD = new CDMatrix(Matrix->NNZ, Matrix->N, CDdiag);
//	JD = new JDMatrix(Matrix->NNZ, Matrix->N, maxvalJD);
//	diagelemSL = Matrix->diagSLMatrix(*Matrix);
//	SL = new SLMatrix(Matrix->NNZ, Matrix->N, diagelemSL);
//
//	v = new FPTYPE[Matrix->N];
//	for (int i = 0; i < Matrix->N; i++)
//	{
//		v[i] = 1;
//	}
//	result_crs = new FPTYPE[Matrix->N];
//	result_ccs = new FPTYPE[Matrix->N];
//	result_jd = new FPTYPE[Matrix->N];
//	result_cd = new FPTYPE[Matrix->N];
//	result_sl = new FPTYPE[Matrix->N];
//	result_mkl = new FPTYPE[Matrix->N];
//
//
//	//Converters *Conv = new Converters();
//
//	//char name[100] = "matrix.mtx";
//	//char name2[100];
//	//char name3[100];
//	//strncpy(name3, name, strchr(name, '.') - name);
//	//sprintf(name2, "COO_%s.bin", name3);
//
//	//if (CRSBinary == false)
//	//{
//		startTime = getCPUTime();
//		Converters::COOToCRS(*Matrix, *CRS);
//		//fprintf(fp, CRS->PrintMatrix);
//		/*CRS->PrintMatrix(Matrix->NNZ,Matrix->N);
//		system("pause");*/
//		endTime = getCPUTime();
//		fprintf(fp, "Time Convert in \n\nCRS: \t\t%.15f\n", endTime - startTime);
//	//	CRS->WriteInBinaryFile(*CRS);
//	//	CRSBinary = true;
//	//}
//	//else
//	//{
//	//	CRS->ReadFromBinaryFile("CRS.bin");
//	//}
//
//	/*if (CCSBinary == false)
//	{*/
//		//startTime = getCPUTime();
//		//Converters::COOToCCS(*Matrix, *CCS);
//		//endTime = getCPUTime();
//		//fprintf(fp, "Time Convert in \n\nCCS: \t\t%lf\n", endTime - startTime);
//	//	//CCS->WriteInBinaryFile(*CCS);
//	//	//CCSBinary = true;
//	//}
//	//else
//	//{
//	//	CCS->ReadFromBinaryFile("CCS.bin");
//	//}
//
//	//if (JDBinary == false)
//	//{
//		//startTime = getCPUTime();
//		//Converters::COOToJD(*Matrix, *JD);
//		//endTime = getCPUTime();
//		//fprintf(fp, "Time Convert in \n\njagged diagonal: \t\t%lf\n", endTime - startTime);
//		//JD->WriteInBinaryFile(*JD);
//		//JDBinary = true;
//	/*}
//	else
//	{
//		JD->ReadFromBinaryFile("JD.bin");
//	}*/
//
//	//if (CDBinary == false)
//	//{
//		//startTime = getCPUTime();
//		//CD->FillDiagArray(*Matrix);	
//		//Conv->COOToCD(*Matrix, *CD);
//		///*CD->Print();
//		//system("pause");*/
//		//endTime = getCPUTime();
//		//fprintf(fp, "Time Convert in \n\ncompressed diagonal: \t\t%lf\n", endTime - startTime);
//	//	//CD->WriteInBinaryFile(*CD);
//	//	//CDBinary = true;
//	//}
//	//else
//	//{
//	//	CD->ReadFromBinaryFile("CD.bin");
//	//}
//
//	//if (SLBinary == false)
//	//{
//		//startTime = getCPUTime();
//		//Converters::COOToSL(*Matrix, *SL);
//		//endTime = getCPUTime();
//		//fprintf(fp, "Time Convert in \n\nSL: \t\t%lf\n", endTime - startTime);
//	//	//SL->WriteInBinaryFile(*SL);
//	//	//SLBinary = true;
//	//}
//	//else
//	//{
//	//	SL->ReadFromBinaryFile("SL.bin");
//	//}
//
//
//	//startTime = getCPUTime();
//	//CCS->MatrixVectorMultCCS(CCS, v, Matrix->N,result_ccs);
//	//endTime = getCPUTime();
//	//fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCCS: \t\t%lf\n", endTime - startTime);
//
//	//
//	startTime = getCPUTime();
//	CRS->MatrixVectorMultCRS(CRS, v, Matrix->N, result_crs);
//	endTime = getCPUTime();
//	fprintf(fp, "Time Matrix-Vector multiplication in \n\nCRS: \t%.15f\n", endTime - startTime);
//	for (int i = 0; i<Matrix->N; i++)
//	fprintf(fp, "result: \t%f\n",result_crs[i]);
//
//
//	//startTime = getCPUTime();
//	////SL->MatrixVectorMultSL(SL, v, Matrix->N, result_sl);
//	//endTime = getCPUTime();
//	//fprintf(fp, "Time Matrix-Vector multiplication in \n\nSL: \t%lf\n", endTime - startTime);
//
//
//	//startTime = getCPUTime();
//	//JD->MatrixVectorMultJD(JD, v, Matrix->N, result_jd);
//	//endTime = getCPUTime();
//	//fprintf(fp, "Time Matrix-Vector multiplication in \n\nJD: \t%lf\n", endTime - startTime);
//	
//	
//
//	//startTime = getCPUTime();
//	//CD->MatrixVectorMultCD(CD, v, Matrix->N, result_cd);
//	//endTime = getCPUTime();
//	//fprintf(fp, "Time Matrix-Vector multiplication in \n\nCD: \t%lf\n", endTime - startTime);
//	//for (int i = 0; i<Matrix->N; i++)
//	//fprintf(fp, "result: \t%f\n",result_cd[i]);
//	for (int i = 0; i < Matrix->N + 1; i++)
//	{
//		CRS->row_ptr[i]++;
//	}
//	for (int i = 0; i < Matrix->NNZ; i++)
//	{
//		CRS->col_ind[i]++;
//	}
//	startTime = getCPUTime();
//	mkl_dcsrgemv("N", &(CRS->N), CRS->val, CRS->row_ptr, CRS->col_ind, v, result_mkl);
////	mkl_dcoogemv("N", &Matrix->N, Matrix->val, Matrix->row_ind, Matrix->col_ind, &Matrix->NNZ, v, result_mkl);
//	endTime = getCPUTime();
//	//for (int i =0; i<Matrix->N; i++)
//	//fprintf(fp, "result: \t%f\n", result_mkl[i]);
//
//	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nMKL CRS: \t\t%.15f\n", endTime - startTime);
//	fprintf(fp, "\ndifference %.15f with JDIAG\n", CheckCorrectness(result_jd, result_mkl, Matrix->N));
//	fprintf(fp,"\ndifference %.15f with CCS\n", CheckCorrectness(result_ccs, result_mkl, Matrix->N));
//	fprintf(fp,"\ndifference %.15f with CRS\n", CheckCorrectness(result_crs, result_mkl, Matrix->N));
//	//fprintf(fp, "\ndifference %.15f with SKYLINE\n", CheckCorrectness(result_sl, result_mkl, Matrix->N));
//	fprintf(fp, "\ndifference %.15f with COMPDIAG\n", CheckCorrectness(result_cd, result_mkl, Matrix->N));
//
//
//	delete[] result_crs;
//	delete[] result_ccs;
//	delete[] result_cd;
//	delete[] result_jd;
//	delete[] result_sl;
//	delete[] result_mkl;
//	return 0;
//}

int main(int argc, char** argv)

{
	FPTYPE startTime, endTime;
	static bool COORead = false;
	static bool COOBinary = false;
	static bool CRSBinary = false;
	static bool CCSBinary = false;
	static bool CDBinary = false;
	static bool JDBinary = false;
	static bool SLBinary = false;

	char *fileName;
	char *act;
	char *fileInput;
	char *fileOutput;
	char *typeMatrix;
	INTTYPE CDdiag;
	INTTYPE maxvalJD;
	INTTYPE diagelemSL;

	FILE *fp = fopen("data.dt", "w");
	FPTYPE* v;
	FPTYPE* result_crs;
	FPTYPE* result_ccs;
	FPTYPE* result_cd;
	FPTYPE* result_jd;
	FPTYPE* result_sl;
	FPTYPE* result_mkl;

	COOMatrix* Matrix;
	CRSMatrix *CRS;
	CCSMatrix *CCS;
	CDMatrix *CD;
	JDMatrix *JD;
	SLMatrix *SL;



	fileName = new char[256];
	fileInput = new char[256];
	typeMatrix = new char[5];
	act = new char[5];
	char name[256];
	strncpy(name, fileName, strchr(fileName, '.') - fileName);
	//fileName = "C:/Users/Vika/Documents/GitHub/SparceMatrix-new/SM/SM/Lin.mtx";
	strcpy(fileName, argv[1]);
	strcpy(act, argv[2]);
	strcpy(typeMatrix, argv[3]);
	fprintf(fp, "Matrix file name: %s\n\n", fileName);

	startTime = getCPUTime();
	Matrix->ReadMatrix(fileName);
	endTime = getCPUTime();
	fprintf(fp, "Time Read Matrix Info: \t%lf\n\n", endTime - startTime);
	if (typeMatrix == "COO")
	{
		/*startTime = getCPUTime();
		Matrix->ReadMatrix(fileName);
		endTime = getCPUTime();
		fprintf(fp, "Time Read Matrix Info: \t%lf\n\n", endTime - startTime);*/
		if (act == "wr")
		{
			/*startTime = getCPUTime();
			Matrix -> ReadMatrix(fileName);
			endTime = getCPUTime();
			fprintf(fp, "Time Read Matrix Info: \t%lf\n\n", endTime - startTime);*/
			//char name[256];
			//strncpy(name, fileName, strchr(fileName, '.') - fileName);
			sprintf(fileInput, "COO_%s.bin", name);
			Matrix->WriteInBinaryFile(*Matrix,fileInput);
			//fprintf(fp, "Matrix size: N %d NNZ %d\n\n", Matrix->N, Matrix->NNZ);
		}

		if (act == "r")
		{
			sprintf(fileInput, "COO_%s.bin", name);
			Matrix->ReadFromBinaryFile(fileInput);
		}

	}
	if (typeMatrix == "CRS")
	{
	//	char name[256];
		//strncpy(name, fileName, strchr(fileName, '.') - fileName);
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix->ReadFromBinaryFile(fileInput);
		if (act == "wr")
		{
			CRS = new CRSMatrix(Matrix->NNZ, Matrix->N);
			startTime = getCPUTime();
			Converters::COOToCRS(*Matrix, *CRS);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCRS: \t\t%.15f\n", endTime - startTime);
			//char name[256];
			//strncpy(name, fileName, strchr(fileName, '.') - fileName);
			sprintf(fileInput, "CRS_%s.bin", name);
			CRS->WriteInBinaryFile(*CRS, fileInput);
		}
		if (act == "r")
		{
			sprintf(fileInput, "CRS_%s.bin", name);
			CRS->ReadFromBinaryFile(fileInput);
		}
		v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}
		result_crs = new FPTYPE[Matrix->N];
		
		startTime = getCPUTime();
		CRS->MatrixVectorMultCRS(CRS, v, Matrix->N, result_crs);
		endTime = getCPUTime();
		fprintf(fp, "Time Matrix-Vector multiplication in \n\nCRS: \t%.15f\n", endTime - startTime);
	}
	if (typeMatrix == "CCS")
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix->ReadFromBinaryFile(fileInput);
		if (act == "wr")
		{
			CCS = new CCSMatrix(Matrix->NNZ, Matrix->N);
			startTime = getCPUTime();
			Converters::COOToCCS(*Matrix, *CCS);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCCS: \t\t%.15f\n", endTime - startTime);
			//char name[256];
			//strncpy(name, fileName, strchr(fileName, '.') - fileName);
			sprintf(fileInput, "CCS_%s.bin", name);
			CCS->WriteInBinaryFile(*CCS, fileInput);
		}
		if (act == "r")
		{
			sprintf(fileInput, "CCS_%s.bin", name);
			CCS->ReadFromBinaryFile(fileInput);
		}
		v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}
		result_ccs = new FPTYPE[Matrix->N];
		startTime = getCPUTime();
		CCS->MatrixVectorMultCCS(CCS, v, Matrix->N, result_ccs);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCCS: \t\t%lf\n", endTime - startTime);

	}
	if (typeMatrix == "CD")
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix->ReadFromBinaryFile(fileInput);
		if (act == "wr")
		{
			CDdiag = Matrix->DiagCDMatrix(*Matrix);
			CD = new CDMatrix(Matrix->NNZ, Matrix->N, CDdiag);
			startTime = getCPUTime();
			Converters::COOToCD(*Matrix, *CD);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCCS: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "CD_%s.bin", name);
			CD->WriteInBinaryFile(*CD, fileInput);
		}
		if (act == "r")
		{
			sprintf(fileInput, "CD_%s.bin", name);
			CD->ReadFromBinaryFile(fileInput);
		}
		v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}
		result_cd = new FPTYPE[Matrix->N];
		startTime = getCPUTime();
		CD->MatrixVectorMultCD(CD, v, Matrix->N, result_cd);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCD: \t\t%lf\n", endTime - startTime);
	}
	if (typeMatrix == "JD")
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix->ReadFromBinaryFile(fileInput);
		if (act == "wr")
		{
			maxvalJD = Matrix->maxvalJDMatrix(*Matrix);
			JD = new JDMatrix(Matrix->NNZ, Matrix->N, maxvalJD);
			startTime = getCPUTime();
			Converters::COOToJD(*Matrix, *JD);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nCRS: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "JD_%s.bin", name);
			JD->WriteInBinaryFile(*JD, fileInput);
		}
		if (act == "r")
		{
			sprintf(fileInput, "JD_%s.bin", name);
			JD->ReadFromBinaryFile(fileInput);
		}
		v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}
		result_jd = new FPTYPE[Matrix->N];
		startTime = getCPUTime();
		JD->MatrixVectorMultJD(JD, v, Matrix->N, result_jd);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nJD: \t\t%lf\n", endTime - startTime);
	}
	if (typeMatrix == "SL")
	{
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix->ReadFromBinaryFile(fileInput);
		if (act == "wr")
		{
			diagelemSL = Matrix->diagSLMatrix(*Matrix);
			SL = new SLMatrix(Matrix->NNZ, Matrix->N, diagelemSL);
			startTime = getCPUTime();
			Converters::COOToSL(*Matrix, *SL);
			endTime = getCPUTime();
			fprintf(fp, "Time Convert in \n\nSL: \t\t%.15f\n", endTime - startTime);
			sprintf(fileInput, "SL_%s.bin", name);
			JD->WriteInBinaryFile(*JD, fileInput);
		}
		if (act == "r")
		{
			sprintf(fileInput, "SL_%s.bin", name);
			SL->ReadFromBinaryFile(fileInput);
		}
		v = new FPTYPE[Matrix->N];
		for (int i = 0; i < Matrix->N; i++)
		{
			v[i] = 1;
		}
		result_sl = new FPTYPE[Matrix->N];
		startTime = getCPUTime();
		SL->MatrixVectorMultSL(SL, v, Matrix->N, result_sl);
		endTime = getCPUTime();
		fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nCD: \t\t%lf\n", endTime - startTime);
	}

	if (typeMatrix == "MKL")
	{
		//	char name[256];
		//strncpy(name, fileName, strchr(fileName, '.') - fileName);
		sprintf(fileInput, "COO_%s.bin", name);
		Matrix->ReadFromBinaryFile(fileInput);
		if (act == "crs")
		{
			CRS = new CRSMatrix(Matrix->NNZ, Matrix->N);
			v = new FPTYPE[Matrix->N];
			for (int i = 0; i < Matrix->N; i++)
			{
				v[i] = 1;
			}
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
			fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nMKL CRS: \t\t%lf\n", endTime - startTime);

		}
		if (act == "coo")
		{
			//
		}

	}



	
	//char name[100] = "matrix.mtx";
	//char name2[100];
	//char name3[100];
	//strncpy(name3, name, strchr(name, '.') - name);
	//sprintf(name2, "COO_%s.bin", name3);

	

	//	mkl_dcoogemv("N", &Matrix->N, Matrix->val, Matrix->row_ind, Matrix->col_ind, &Matrix->NNZ, v, result_mkl);
	

	fprintf(fp, "\nTime Matrix-Vector multiplication in \n\nMKL CRS: \t\t%.15f\n", endTime - startTime);
	fprintf(fp, "\ndifference %.15f with JDIAG\n", CheckCorrectness(result_jd, result_mkl, Matrix->N));
	fprintf(fp, "\ndifference %.15f with CCS\n", CheckCorrectness(result_ccs, result_mkl, Matrix->N));
	fprintf(fp, "\ndifference %.15f with CRS\n", CheckCorrectness(result_crs, result_mkl, Matrix->N));
	//fprintf(fp, "\ndifference %.15f with SKYLINE\n", CheckCorrectness(result_sl, result_mkl, Matrix->N));
	fprintf(fp, "\ndifference %.15f with COMPDIAG\n", CheckCorrectness(result_cd, result_mkl, Matrix->N));


	delete[] result_crs;
	delete[] result_ccs;
	delete[] result_cd;
	delete[] result_jd;
	delete[] result_sl;
	delete[] result_mkl;
	return 0;
}



