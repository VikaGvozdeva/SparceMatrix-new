#pragma once
#ifndef MODULES_COOMATRIX_INCLUDE_CRSMATRIX_H_
#define MODULES_COOMATRIX_INCLUDE_CRSMATRIX_H_
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
class CRSMatrix
{
public:
	INTTYPE N;
	INTTYPE NNZ;
	FPTYPE *val;
	INTTYPE *col_ind;
	INTTYPE *row_ptr;

	CRSMatrix(INTTYPE _NNZ, INTTYPE _N);
	CRSMatrix(const CRSMatrix& Matrix);
	~CRSMatrix();
	CRSMatrix *ReadFromBinaryFile(char *filename);
	void WriteInBinaryFile(const CRSMatrix& Matrix, char* filename);
	void PrintMatrix(int _NNZ, int _N);
	CRSMatrix& operator=(const CRSMatrix &Matrix);
	FPTYPE* MatrixVectorMultCRS(CRSMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result);
	
};
#endif