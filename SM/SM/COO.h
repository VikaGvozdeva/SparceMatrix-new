#ifndef MODULES_COOMATRIX_INCLUDE_COOMATRIX_H_
#define MODULES_COOMATRIX_INCLUDE_COOMATRIX_H_
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <string.h>
#include <iostream>
#include <inttypes.h>

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
class COOMatrix
{

public:

	INTTYPE N;
	INTTYPE NNZ;
	FPTYPE *val;
	INTTYPE *row_ind;
	INTTYPE *col_ind;
	INTTYPE *NNZ_row;
	INTTYPE diag_cd;
	INTTYPE maxval_jd;

	COOMatrix(INTTYPE  _NNZ, INTTYPE _N);
	~COOMatrix();
	COOMatrix(const COOMatrix &Matrix);
	void PrintMatrix(int NNZ);
	COOMatrix* ReadFromBinaryFile(char *filename);
	void WriteInBinaryFile(const COOMatrix& Matrix, char* filename);
	COOMatrix &operator=(const COOMatrix & Matrix);
	void ReadMatrixInfo(COOMatrix& Matrix, char *name);
	COOMatrix* ReadMatrix(char *name);
	void ReadNumberForMatrix(int& N, int& NNZ, char *name);


	INTTYPE DiagCDMatrix(const COOMatrix& Matrix);
	INTTYPE maxvalJDMatrix(const COOMatrix& Matrix);
	INTTYPE nonZeroRows(const COOMatrix& Matrix);
	INTTYPE diagSLMatrix(const COOMatrix& Matrix);

};
#endif  //  MODULES_COOMATRIX_INCLUDE_COOMATRIX_H_
