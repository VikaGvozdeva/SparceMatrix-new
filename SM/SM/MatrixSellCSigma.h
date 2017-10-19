#ifndef MATRIXSELLCSIGMA_H_
#define MATRIXSELLCSIGMA_H_

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

class MatrixSellCSigma //матрица в формате SELL-S-sigma
{
public:
	INTTYPE n;
	INTTYPE c;
	INTTYPE sum;
	INTTYPE* nnz_max;
	FPTYPE* val;
	INTTYPE* col_ind;
	INTTYPE* sliceptr;
	INTTYPE* trpos;

	//конструктор инициализатор 
	MatrixSellCSigma(INTTYPE* _nnz_max, INTTYPE* _trpos, INTTYPE _n, INTTYPE _c); //блоки  строятся в соответсвии с перестановкой
	MatrixSellCSigma(const MatrixSellCSigma & Matrix); //конструктор копирования
	~MatrixSellCSigma(); //деструктор
	MatrixSellCSigma & operator=(const MatrixSellCSigma &Matrix);    // присваивание
	void ReadFromBinaryFile(char *filename);  // чтение из бинарного файла
	void WriteInBinaryFile(char *filename);  //запись в бинарный файл
	void Matrix_VectorMultiplicationInSELL_C_sigma(FPTYPE* Vector, INTTYPE n_v, FPTYPE *result); //умножение матрицы на вектор
	friend ostream & operator<<(ostream &out, const MatrixSellCSigma &Matrix);
	INTTYPE CalculateSum(INTTYPE *_nnz_max, INTTYPE _n, INTTYPE _c);
	void DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _c, INTTYPE _sum);
	static void TransposeRow(INTTYPE* nnz_row, INTTYPE n, INTTYPE* trpos); //перестановка строк в зависимости от числа ненулевых элементов
};
#endif  // MATRIXSELLCSIGMA_H_
