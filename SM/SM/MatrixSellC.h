#ifndef MATRIXSELLC_H_
#define MATRIXSELLC_H_

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

class MatrixSellC //матрица в формате SELL-S
{
public:
	INTTYPE n;
	INTTYPE c;
	INTTYPE sum;
	INTTYPE* nnz_max;
	FPTYPE* val;
	INTTYPE* col_ind;
	INTTYPE* sliceptr;
	//конструктор инициализатор  
	MatrixSellC(INTTYPE* _nnz_max, INTTYPE _n, INTTYPE _c);  //nnz_max -массив максимальных размеров строк в каждом блоке
	MatrixSellC(const MatrixSellC & Matrix); //конструктор копирования
	~MatrixSellC(); //деструктор
	MatrixSellC & operator=(const MatrixSellC &Matrix);   // присваивание
	void ReadFromBinaryFile(char *filename);  // чтение из бинарного файла
	void WriteInBinaryFile(char *filename);  //запись в бинарный файл
	void Matrix_VectorMultiplicationInSELL_C(FPTYPE* Vector, INTTYPE n_v, FPTYPE *result); //умножение матрицы на вектор
	friend ostream & operator<<(ostream &out, const MatrixSellC &Matrix);
	INTTYPE CalculateSum(INTTYPE *_nnz_max, INTTYPE _n, INTTYPE _c);
	void DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _c, INTTYPE _sum);
};

#endif  //  MATRIXSELLC_H_