#ifndef MODULES_COOMATRIX_INCLUDE_CDMATRIX_H_
#define MODULES_COOMATRIX_INCLUDE_CDMATRIX_H_
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <string.h>
#include <iostream>
#include <inttypes.h>
#include "COO.h"

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

/*int compare(const void * x1, const void * x2)
 {
 	return (*(int*)x1 - *(int*)x2);
 }*/
class CDMatrix
{
public:
	
	INTTYPE N;
	INTTYPE NNZ;

	INTTYPE B;
	INTTYPE* diag;
	FPTYPE** val;

	CDMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _B);
	void Print();
	~CDMatrix();
	CDMatrix(const CDMatrix &Matrix);
	//int compare(const void * x1, const void * x2);
	void ReadFromBinaryFile(char *filename);
	void FillDiagArray(const COOMatrix Matrix);
	void WriteInBinaryFile(char *filename);	
	void MatrixVectorMultCD(FPTYPE *vec, INTTYPE vec_N, FPTYPE *result);
	CDMatrix& operator=(const CDMatrix &Matrix);
	friend ostream & operator<<(ostream &out, const CDMatrix &Matrix);

};
#endif