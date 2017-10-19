#ifndef MATRIXELLPACK_H_
#define MATRIXELLPACK_H_

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <string.h>
#include <iostream>
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

class MatrixEllpack //������� � ������� ELLPACK
{
public:
	INTTYPE n;
	INTTYPE nnz_max;
	FPTYPE* val_data;
	FPTYPE** val;
	INTTYPE* col_ind_data;
	INTTYPE** col_ind;

	MatrixEllpack(INTTYPE _nnz_max, INTTYPE _n); //����������� �������������
	MatrixEllpack(const MatrixEllpack& Matrix); //����������� �����������
	~MatrixEllpack(); //����������
	MatrixEllpack& operator=(const MatrixEllpack &Matrix);    // ������������
	void ReadFromBinaryFile(char *filename); // ������ �� ��������� �����
	void WriteInBinaryFile(char *filename);  //������ � �������� ����
	void Matrix_VectorMultiplicationInEllpack(FPTYPE* Vector, INTTYPE n_v, FPTYPE *result);//��������� ������� �� ������
	friend ostream & operator<<(ostream &out, const MatrixEllpack &Matrix);
	void DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _nnz_max);
};
#endif  //  MATRIXELLPACK_H_

