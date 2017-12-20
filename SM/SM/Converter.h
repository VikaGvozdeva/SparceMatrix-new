#ifndef MODULES_COOMATRIX_INCLUDE_CONVMATRIX_H_
#define MODULES_COOMATRIX_INCLUDE_CONVMATRIX_H_
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <time.h>
#include <string.h>
#include <iostream>
#include <inttypes.h>
#include "COO.h"
#include "CCS.h"
#include "CRS.h"
#include "JD.h"
#include "SL.h"
#include "CD.h"

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

class Converters
{
public:

	//static void SymCOOToCOO(const COOMatrix &Mtx, COOMatrix &Matrix);
	static void COOToCRS(const COOMatrix &Mtx, CRSMatrix &Matrix);
	static void COOToCCS(const COOMatrix &Mtx, CCSMatrix &Matrix);	
	static void COOToCD(const COOMatrix &Mtx, CDMatrix &Matrix);
	static void COOToJD(const COOMatrix &Mtx, JDMatrix &Matrix);
	static void COOToSL(const COOMatrix &Mtx, SLMatrix &Matrix);
	static void qs(INTTYPE* s_arr, INTTYPE* _s_arr, int first, int last);
};
#endif