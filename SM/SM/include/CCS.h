#ifndef MODULES_COOMATRIX_INCLUDE_CCSMATRIX_H_
#define MODULES_COOMATRIX_INCLUDE_CCSMATRIX_H_
#include <inttypes.h>
#include <iostream>
#include <stdlib.h>
#include <mkl.h>

using namespace std;

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
class CCSMatrix
{
public:
	INTTYPE N;
	INTTYPE NNZ;
	FPTYPE *val;
	INTTYPE *row_ind;
	INTTYPE *col_ptr;


	CCSMatrix(INTTYPE _NNZ, INTTYPE _N);
	CCSMatrix(const CCSMatrix& Matrix);
	~CCSMatrix();
	CCSMatrix* ReadFromBinaryFile(char *filename);
	void WriteInBinaryFile(CCSMatrix& Matrix, char* filename);
	CCSMatrix& operator=(const CCSMatrix &Matrix);
	FPTYPE* MatrixVectorMultCCS(CCSMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result);
	
};
#endif
