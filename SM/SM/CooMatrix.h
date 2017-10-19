#ifndef COOMATRIX_H_
#define COOMATRIX_H_

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <string.h>
#include <mkl.h>
#include <iostream>
#include <vector>
#include <inttypes.h>

using namespace std;

//#define USE_INT_64
#define USE_DOUBLE

#ifdef USE_INT_64
typedef int64_t INTTYPE;
#else
typedef int32_t INTTYPE;
#endif

#ifdef USE_DOUBLE
typedef double FPTYPE;
#define ZERO 0.0
#define ABS fabs
#else
typedef float FPTYPE;
#define ZERO 0.0f
#define ABS abs
#endif


#define MAX_LINE_LEN 1000

class CooMatrix  // ������� � ������������ �������
{
public:
	INTTYPE n;
	INTTYPE nnz;
	FPTYPE* val;
	INTTYPE* col_ind;
	INTTYPE* row_ind;
	INTTYPE* nnz_row;

	CooMatrix(INTTYPE _nnz, INTTYPE _n); //����������� �������������
	CooMatrix(const CooMatrix &Matrix); //����������� �����������
	~CooMatrix();//����������
	CooMatrix& operator=(const CooMatrix &Matrix);   // ������������
	void ReadMatrix(char * filename); //������ �������
	void ReadFromBinaryFile(char *filename); // ������ �� ��������� �����
	void WriteInBinaryFile(char *filename); //������ � �������� ����
	friend ostream & operator<<(ostream &out, const CooMatrix &Matrix);
	void DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _nnz);
	void SortWithVectors();

};
#endif  //  COOMATRIX_H_
