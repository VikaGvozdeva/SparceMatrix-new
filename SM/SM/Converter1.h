//#ifndef CONVERTER_H_
//#define CONVERTER_H_
//#define _CRT_SECURE_NO_WARNINGS
//#include <stdlib.h>
//#include <stdio.h>
//#include <cmath>
//#include <time.h>
//#include <string.h>
//#include <mkl.h>
//#include <iostream>
//#include <vector>
//#include <inttypes.h>
//
//using namespace std;
//
////#define USE_INT_64
//#define USE_DOUBLE
//
//#ifdef USE_INT_64
//typedef int64_t INTTYPE;
//#else
//typedef int32_t INTTYPE;
//#endif
//
//#ifdef USE_DOUBLE
//typedef double FPTYPE;
//#define ZERO 0.0
//#define ABS fabs
//#else
//typedef float FPTYPE;
//#define ZERO 0.0f
//#define ABS abs
//#endif
//
//INTTYPE SearchMax_int(INTTYPE* arr, INTTYPE n); // ïîèñê ìàêñèìóìà äëÿ INTTYPE
//
//FPTYPE SearchMax_double(FPTYPE* arr, INTTYPE n); // ïîèñê ìàêñèìóìà äëÿ FPTYPE
//
//FPTYPE CheckCorrectness(FPTYPE* my_mult, FPTYPE* mkl_mult, INTTYPE n); //ïðîâåðêà êîððåêòíîñòè óìíîæåíèÿ,íóæíî åùå ïîëó÷èòü ðåçóëüòàò â MKL
//
//class Converter
//{
//public:
//	static void CooToEll(const CooMatrix& coo, MatrixEllpack& ell);
//	static void CooToEllWithCoo(const CooMatrix& coo, MatrixEllpackWithCoo& ellc , INTTYPE p1); 
//	static void CooToSellC(const CooMatrix& coo, MatrixSellC& sell, INTTYPE c);
//	static void CooToSellCSigma(const CooMatrix& coo, MatrixSellCSigma& sells, INTTYPE c, INTTYPE sigma);
//};
//#endif  //  CONVERTER_H_