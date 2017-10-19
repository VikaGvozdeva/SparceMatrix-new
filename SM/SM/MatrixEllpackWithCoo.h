#ifndef MATRIXELLPACKWITHCOO_H_
#define MATRIXELLPACKWITHCOO_H_

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

class MatrixEllpackWithCoo //матрица в формате ELLPACK with COO
{
public:
    INTTYPE n;
    INTTYPE nnz_max;
    INTTYPE p2;
    FPTYPE* val_data;
    FPTYPE** val;
    INTTYPE* col_ind_data;
    INTTYPE** col_ind;
    FPTYPE* val_c;
    INTTYPE* col_ind_c;
    INTTYPE* row_ind_c;

    MatrixEllpackWithCoo(INTTYPE _nnz_max, INTTYPE _n, INTTYPE _p2); //конструктор инициализатор
    MatrixEllpackWithCoo(const MatrixEllpackWithCoo & Matrix); //конструктор копирования
    ~MatrixEllpackWithCoo(); //деструктор
    MatrixEllpackWithCoo & operator=(const MatrixEllpackWithCoo &Matrix);    // присваивание
    void ReadFromBinaryFile(char *filename);  // чтение из бинарного файла
    void WriteInBinaryFile(char *filename); //запись в бинарный файл
    void Matrix_VectorMultiplicationInELLPACKwithCOO( FPTYPE* Vector, INTTYPE n_v, FPTYPE *result); //умножение матрицы на вектор
    friend ostream & operator<<(ostream &out, const MatrixEllpackWithCoo &Matrix);
    void DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _nnz_max, INTTYPE _p2);
};
#endif  //  MATRIXELLPACKWITHCOO_H_