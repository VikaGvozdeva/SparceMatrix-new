#define _CRT_SECURE_NO_WARNINGS

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include <iostream>
#include <vector>
#include <mkl.h>

using namespace std;

typedef int64_t INTTYPE;
typedef int32_t INTTYPE32;
typedef double FPTYPE;

#define MAX_LINE_LEN 1000000


void qs(INTTYPE* s_arr, INTTYPE* _s_arr, int first, int last)
{
	int i = first, j = last, x = s_arr[(first + last) / 2], tmp = 0;

	do {
		while (s_arr[i] > x) i++;
		while (s_arr[j] < x) j--;

		if (i <= j) {
			if (s_arr[i] < s_arr[j])
			{
				tmp = s_arr[i];
				s_arr[i] = s_arr[j];
				s_arr[j] = tmp;

				tmp = _s_arr[i];
				_s_arr[i] = _s_arr[j];
				_s_arr[j] = tmp;

			}
			i++;
			j--;
		}
	} while (i <= j);

	if (i < last)
		qs(s_arr, _s_arr, i, last);
	if (first < j)
		qs(s_arr, _s_arr, first, j);
}

int compare(const void * x1, const void * x2)   
{
	return (*(int*)x1 - *(int*)x2);    

}

class COOMatrix
{

public:

	INTTYPE N;
	INTTYPE NNZ;
	FPTYPE *val;
	INTTYPE *row_ind;
	INTTYPE *col_ind;
	INTTYPE *NNZ_row;

	COOMatrix::COOMatrix(INTTYPE  _NNZ, INTTYPE _N)
	{
		int i;
		N = _N;
		NNZ = _NNZ;
		val = new FPTYPE[NNZ];
		col_ind = new INTTYPE[NNZ];
		row_ind = new INTTYPE[NNZ];
		NNZ_row = new INTTYPE[N];
		for (i = 0; i < N; i++)
			NNZ_row[i] = 0;
	}
	COOMatrix::~COOMatrix()
	{
		delete[] col_ind;
		delete[] row_ind;
		delete[] val;
		delete[] NNZ_row;

	}
	COOMatrix::COOMatrix(const COOMatrix &Matrix)
	{
		N = Matrix.N;
		NNZ = Matrix.NNZ;
		val = new FPTYPE[NNZ];
		col_ind = new INTTYPE[NNZ];
		row_ind = new INTTYPE[NNZ];
		NNZ_row = new INTTYPE[N];
		for (int i = 0; i < N + 1; i++)
		{
			NNZ_row[i] = Matrix.NNZ_row[i];
		}

		for (int j = 0; j < NNZ; j++)
		{
			col_ind[j] = Matrix.col_ind[j];
			row_ind[j] = Matrix.row_ind[j];
			val[j] = Matrix.val[j];
		}
	}
	void COOMatrix::PrintMatrix(int NNZ)
	{
		int i;
		for (i = 0; i < NNZ; i++)
		{
			printf("(%lf,%d,%d) , ", val[i], row_ind[i], col_ind[i]);
		}
		printf("\n");
	}

	COOMatrix *COOMatrix::ReadFromBinaryFile(char *filename)
	{
		FILE *COOmtx = NULL;
		int N, NNZ;
		COOmtx = fopen("COOmtx.bin", "rb");
		if (COOmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, COOmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, COOmtx);
		COOMatrix * Matrix = new COOMatrix(NNZ, N);
		fread(Matrix->val, sizeof(FPTYPE), Matrix->NNZ, COOmtx);
		fread(Matrix->col_ind, sizeof(INTTYPE), Matrix->NNZ, COOmtx);
		fread(Matrix->row_ind, sizeof(INTTYPE), Matrix->NNZ, COOmtx);
		fclose(COOmtx);
		return Matrix;
	}

	void COOMatrix::WriteInBinaryFile(COOMatrix Matrix)
	{
		FILE *COOmtx = NULL;
		COOmtx = fopen("COOmtx.bin", "wb");
		if (COOmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&Matrix.N, sizeof(INTTYPE), 1, COOmtx);
		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, COOmtx);
		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, COOmtx);
		fwrite(Matrix.col_ind, sizeof(INTTYPE), Matrix.NNZ, COOmtx);
		fwrite(Matrix.row_ind, sizeof(INTTYPE), Matrix.NNZ, COOmtx);
		fclose(COOmtx);
	}

	INTTYPE COOMatrix::DiagCDMatrix(COOMatrix Matrix)
	{
		int i, tmp_ind, diag_numb, m, j;
		bool flag;
		NNZ = Matrix.NNZ;
		N = Matrix.N;

		INTTYPE *diag = new INTTYPE[2 * N - 1];

		for (i = 0; i < 2 * N - 1; i++)
		{
			diag[i] = N + 1;
		}
		for (i = 0; i < NNZ; i++)
		{
			tmp_ind = Matrix.col_ind[i] - Matrix.row_ind[i];
			for (j = 0; j < 2 * N - 1; j++)
			{
				if ((tmp_ind) != diag[j])
				{
					flag = true;
				}
				else
				{
					flag = false;
					break;
				}

			}

			if (flag == true)
			{
				diag[m++] = tmp_ind;
			}

		}

		diag_numb = m;
		return diag_numb;
	}

	INTTYPE COOMatrix::maxvalJDMatrix(COOMatrix Matrix)
	{
		INTTYPE maxval;
		maxval = Matrix.NNZ_row[0];
		for (int i = 1; i< N; i++)
		{
			if (Matrix.NNZ_row[i]>maxval)
			{
				maxval = Matrix.NNZ_row[i];
			}
		}
		return maxval;
	}
	INTTYPE COOMatrix::diagSLMatrix(COOMatrix Matrix)
	{
		int diagelem = 0;
		for (int i = 0; i < NNZ; i++)
		{
			if (Matrix.row_ind[i] == Matrix.col_ind[i])
				diagelem++;
		}
		return diagelem;
	}

};



class CRSMatrix
{
public:
	INTTYPE N;
	INTTYPE NNZ;
	FPTYPE *val;
	INTTYPE *col_ind;
	INTTYPE *row_ptr;


	CRSMatrix::CRSMatrix(INTTYPE _NNZ, INTTYPE _N)
	{
		N = _N;
		NNZ = _NNZ;
		val = new FPTYPE[NNZ];
		col_ind = new INTTYPE[NNZ];
		row_ptr = new INTTYPE[N + 1];
		for (int i = 0; i < N + 1; i++)
		{
			row_ptr[i] = 0;
		}
	}

	CRSMatrix::CRSMatrix(const CRSMatrix& Matrix)
	{
		N = Matrix.N;
		NNZ = Matrix.NNZ;
		val = new FPTYPE[NNZ];
		col_ind = new INTTYPE[NNZ];
		row_ptr = new INTTYPE[N + 1];
		for (int i = 0; i < N + 1; i++)
		{
			row_ptr[i] = Matrix.row_ptr[i];
		}

		for (int j = 0; j < NNZ; j++)
		{
			col_ind[j] = Matrix.col_ind[j];
			val[j] = Matrix.val[j];
		}

	}

	CRSMatrix:: ~CRSMatrix() {
		delete[] col_ind;
		delete[] row_ptr;
		delete[] val;

	}

	CRSMatrix *CRSMatrix::ReadFromBinaryFile(char *filename)
	{
		FILE *CRSmtx = NULL;
		int N, NNZ;
		CRSmtx = fopen("COOmtx.bin", "rb");
		if (CRSmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, CRSmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, CRSmtx);
		CRSMatrix * Matrix = new CRSMatrix(NNZ, N);
		fread(Matrix->val, sizeof(FPTYPE), Matrix->NNZ, CRSmtx);
		fread(Matrix->col_ind, sizeof(INTTYPE), Matrix->NNZ, CRSmtx);
		fread(Matrix->row_ptr, sizeof(INTTYPE), Matrix->N +1, CRSmtx);
		fclose(CRSmtx);
		return Matrix;
	}

	void CRSMatrix::WriteInBinaryFile(CRSMatrix Matrix)
	{
		FILE *CRSmtx = NULL;
		CRSmtx = fopen("CRSmtx.bin", "wb");
		if (CRSmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&Matrix.N, sizeof(INTTYPE), 1, CRSmtx);
		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, CRSmtx);
		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, CRSmtx);
		fwrite(Matrix.col_ind, sizeof(INTTYPE), Matrix.NNZ, CRSmtx);
		fwrite(Matrix.row_ptr, sizeof(INTTYPE), Matrix.N+1, CRSmtx);
		fclose(CRSmtx);
	}
	CRSMatrix& CRSMatrix::operator=(const CRSMatrix &Matrix) 
	{
		int i;
		if (this != &Matrix) {
			delete[] val;
			delete[] col_ind;
			delete[] row_ptr;
			N = Matrix.N;
			NNZ = Matrix.NNZ;
			
			val = new FPTYPE[NNZ];
			col_ind = new INTTYPE[NNZ];
			row_ptr = new INTTYPE[N + 1];
			for (i = 0; i < NNZ; i++)
			{
				col_ind[i] = Matrix.col_ind[i];
				val[i] = Matrix.val[i];
			}
			for (i = 0; i < N + 1; i++)
				row_ptr[i] = Matrix.row_ptr[i];

		}
		return *this;
	}
	FPTYPE* CRSMatrix::MatrixVectorMultCRS(CRSMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
	{
		int i, j;
		double tmp;
		for (i = 0; i < N; i++)
		{
			result[i] = 0;
		}
		if (N == Matrix->N)
		{
			for (i = 0; i < N; i++)
			{
				tmp = 0;
				for (j = Matrix->row_ptr[i]; j < Matrix->row_ptr[i + 1]; j++)
				{
					tmp += Matrix->val[j] * vec[Matrix->col_ind[j]];
				}
				result[i] = tmp;
			}
		}
		return result;
	}

};

class CCSMatrix
{
public:
	INTTYPE N;
	INTTYPE NNZ;
	FPTYPE *val;
	INTTYPE *row_ind;
	INTTYPE *col_ptr;


	CCSMatrix::CCSMatrix(INTTYPE _NNZ, INTTYPE _N)
	{
		N = _N;
		NNZ = _NNZ;
		val = new FPTYPE[NNZ];
		row_ind = new INTTYPE[NNZ];
		col_ptr = new INTTYPE[N + 1];
		for (int i = 0; i < N + 1; i++)
		{
			col_ptr[i] = 0;
		}
	}


	CCSMatrix::CCSMatrix(const CCSMatrix& Matrix)
	{
		N = Matrix.N;
		NNZ = Matrix.NNZ;
		val = new FPTYPE[NNZ];
		row_ind = new INTTYPE[NNZ];
		col_ptr = new INTTYPE[N + 1];
		for (int i = 0; i < N + 1; i++)
		{
			col_ptr[i] = Matrix.col_ptr[i];
		}

		for (int j = 0; j < NNZ; j++)
		{
			row_ind = &Matrix.row_ind[j];
			val = &Matrix.val[j];
		}

	}

	CCSMatrix:: ~CCSMatrix() {
		delete[] row_ind;
		delete[] col_ptr;
		delete[] val;

	}

	CCSMatrix *CCSMatrix::ReadFromBinaryFile(char *filename)
	{
		FILE *CCSmtx = NULL;
		int N, NNZ;
		CCSmtx = fopen("CCSmtx.bin", "rb");
		if (CCSmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, CCSmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, CCSmtx);
		CCSMatrix * Matrix = new CCSMatrix(NNZ, N);
		fread(Matrix->val, sizeof(FPTYPE), Matrix->NNZ, CCSmtx);
		fread(Matrix->row_ind, sizeof(INTTYPE), Matrix->NNZ, CCSmtx);
		fread(Matrix->col_ptr, sizeof(INTTYPE), Matrix->N + 1, CCSmtx);
		fclose(CCSmtx);
		return Matrix;
	}

	void CCSMatrix::WriteInBinaryFile(CCSMatrix Matrix)
	{
		FILE *CCSmtx = NULL;
		CCSmtx = fopen("CCSmtx.bin", "wb");
		if (CCSmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&Matrix.N, sizeof(INTTYPE), 1, CCSmtx);
		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, CCSmtx);
		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, CCSmtx);
		fwrite(Matrix.row_ind, sizeof(INTTYPE), Matrix.NNZ, CCSmtx);
		fwrite(Matrix.col_ptr, sizeof(INTTYPE), Matrix.N + 1, CCSmtx);
		fclose(CCSmtx);
	}
	CCSMatrix& CCSMatrix::operator=(const CCSMatrix &Matrix) 
	{
		int i;
		if (this != &Matrix) {
			delete[] val;
			delete[] row_ind;
			delete[] col_ptr;
			N = Matrix.N;
			NNZ = Matrix.NNZ;

			val = new FPTYPE[NNZ];
			row_ind = new INTTYPE[NNZ];
			col_ptr = new INTTYPE[N + 1];
			for (i = 0; i < NNZ; i++)
			{
				row_ind[i] = Matrix.row_ind[i];
				val[i] = Matrix.val[i];
			}
			for (i = 0; i < N + 1; i++)
				col_ptr[i] = Matrix.col_ptr[i];

		}
		return *this;
	}
	FPTYPE* CCSMatrix::MatrixVectorMultCCS(CCSMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
	{
		int i, j, k;
		for (k = 0; k < N; k++)
		{
			result[k] = 0;
		}
		if (N == Matrix->N)
		{
			for (i = 0; i < N; i++)
			{
				for (j = Matrix->col_ptr[i]; j < Matrix->col_ptr[i + 1]; j++)
				{
					result[Matrix->row_ind[j]] += vec[i] * Matrix->val[j];
				}
			}
		}
		return result;
	}

};

class CDMatrix
{
public:
	INTTYPE N;
	INTTYPE NNZ;

	INTTYPE B;
	INTTYPE* diag;
	FPTYPE** val;

	CDMatrix::CDMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _B)
	{
		int i;
		int j;
		N = _N;
		NNZ = _NNZ;
		B = _B;
		diag = new INTTYPE[B];
		val = new FPTYPE*[N];
		for (i = 0; i < N; i++)
		{
			val[i] = new FPTYPE[B];
		}
		for (i = 0; i < N; i++) {
			for (j = 0; j < B; j++)
				val[i][j] = 0;
		}

	}
	CDMatrix::~CDMatrix()
	{
		delete[] diag;
		for (int i = 0; i < N; i++)
			delete[] val[i];
		delete[] val;
	}
	CDMatrix::CDMatrix(const CDMatrix &Matrix)
	{
		int i;
		N = Matrix.N;
		NNZ = Matrix.NNZ;
		B = Matrix.B;
		diag = new INTTYPE[B];
		val = new FPTYPE*[N];
		for (i = 0; i < N; i++)
		{
			val[i] = new FPTYPE[Matrix.B];
		}
		for (i = 0; i < B; i++)
		{
			diag[i] = Matrix.diag[i];
		}
		for (i = 0; i < N ; i++)
			for (int j = 0; j < B; j++)
			{
				val[i][j] = Matrix.val[i][j];
			}
	}
	CDMatrix *CDMatrix::ReadFromBinaryFile(char *filename)
	{
		//
	}
	void CDMatrix::FillDiagArray(COOMatrix Matrix)
	{
		int i, j, tmp_ind, m, diag_numb;
		bool flag;

		for (i = 0; i < 2 * N - 1; i++)
		{
			diag[i] = N + 1;
		}
		for (i = 0; i < NNZ; i++)
		{
			tmp_ind = Matrix.col_ind[i] - Matrix.row_ind[i];
			for (j = 0; j < 2 * N - 1; j++)
			{
				if ((tmp_ind) != diag[j])
				{
					flag = true;
				}
				else
				{
					flag = false;
					break;
				}

			}

			if (flag == true)
			{
				diag[m++] = tmp_ind;
			}

		}

		diag_numb = m;

		qsort(&diag[0], m, sizeof(INTTYPE), compare);
	}
	void CDMatrix::WriteInBinaryFile(CDMatrix Matrix)
	{
		//
	}
	FPTYPE* CDMatrix::MatrixVectorMultCD(CDMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
	{
		int i, j;
		int tmp;
		int max;
		int B = Matrix->B;
		for (i = 0; i < N; i++)
		{
			result[i] = 0;
		}

		for (j = 0; j < B; j++) {
			tmp = Matrix->diag[j];
			if (0 >(0 - tmp))
				i = 0;
			else i = 0 - tmp;

			if (0 > tmp)
				max = Matrix->N - 0;
			else max = Matrix->N - tmp;

			for (i; i < max; i++)
				result[i] += Matrix->val[i][j] * vec[tmp + i];

		}
		return result;
	}

};

class JDMatrix
{
public:
	INTTYPE NNZ;
	INTTYPE N;
	INTTYPE numbdiag;
	INTTYPE *perm;
	FPTYPE *jdiag;
	INTTYPE *col_ind;
	INTTYPE *jd_ptr;
	INTTYPE MaxNNZ;

	JDMatrix::JDMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _MaxNNZ)
	{
		int i;
		N = _N;
		NNZ = _NNZ;
		MaxNNZ = _MaxNNZ;
		jdiag = new FPTYPE[NNZ];
		perm = new INTTYPE[N];
		col_ind = new INTTYPE[NNZ];
		jd_ptr = new INTTYPE[N + 1];
		for (i = 0; i < N + 1; i++)
		{
			jd_ptr[i] = 0;
		}
		for (i = 0; i < NNZ; i++)
		{
			jdiag[i] = 0;
		}
	}
	JDMatrix::~JDMatrix()
	{
		delete[] perm;
		delete[] jdiag;
		delete[] col_ind;
		delete[] jd_ptr;

	}
	JDMatrix::JDMatrix(const JDMatrix &Matrix)
	{
		//
	}
	JDMatrix *JDMatrix::ReadFromBinaryFile(char *filename)
	{
		//
	}
	void JDMatrix::WriteInBinaryFile(JDMatrix Matrix)
	{
		//
	}
	FPTYPE* JDMatrix::MatrixVectorMultCOO(JDMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, tmp_ind = 0, maxval = 0, upper = 0;
		NNZ = Matrix->NNZ;

		FPTYPE* temp = new FPTYPE[N];
		for (i = 0; i < N; i++)
		{
			result[i] = 0;
		}

		int disp = 0;
		for (int j = 0; j < Matrix->MaxNNZ; j++)
		{

			for (int i = 0; i < (Matrix->jd_ptr[j + 1] - Matrix->jd_ptr[j]); i++)
			{
				result[i] += Matrix->jdiag[disp] * vec[Matrix->col_ind[disp]];
				disp++;
			}
		}


		for (i = 0; i < N; i++)
		{
			temp[Matrix->perm[i]] = result[i];
		}
		for (i = 0; i < N; i++)
		{
			result[i] = temp[i];
		}

		free(temp);
		return result;
	}


};

class SLMatrix
{
public:

	INTTYPE N;
	INTTYPE NNZ;
	INTTYPE diag;
	FPTYPE *adiag;
	FPTYPE *altr;
	FPTYPE *autr;
	INTTYPE *jptr;
	INTTYPE *iptr;

	SLMatrix::SLMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _diag)
	{
		int i;
		N = _N;
		NNZ = _NNZ;
		diag = _diag;
		adiag = new FPTYPE[N];
		altr = new FPTYPE[(NNZ - diag) / 2];
		autr = new FPTYPE[(NNZ - diag) / 2];
		jptr = new INTTYPE[(NNZ - diag) / 2];
		iptr = new INTTYPE[N + 1];
		for (i = 0; i < (NNZ - diag) / 2; i++)
		{
			altr[i] = 0;
			autr[i] = 0;
			jptr[i] = 0;
		}
		for (i = 0; i < N; i++)
		{
			adiag[i] = 0;
		}
		for (i = 0; i < N + 1; i++)
		{
			iptr[i] = 0;
		}
	}
	SLMatrix::~SLMatrix()
	{
		delete[] autr;
		delete[] altr;
		delete[] iptr;
		delete[] jptr;
		delete[] adiag;
	}
	SLMatrix::SLMatrix(const COOMatrix &Matrix)
	{
		//
	}
	SLMatrix *SLMatrix::ReadFromBinaryFile(char *filename)
	{
		//
	}
	void SLMatrix::WriteInBinaryFile(SLMatrix Matrix)
	{
		//
	}
	FPTYPE* SLMatrix::MatrixVectorMultSL(SLMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
	{
		int i, j;
		for (i = 0; i < N; i++)
		{
			result[i] = 0;
		}
		for (i = 0; i < N; i++)
		{
			result[i] = vec[i] * Matrix->adiag[i];
		}
		for (i = 1; i < N; i++)
		{

			//for (j = Matrix->iptr[i]; j < Matrix->iptr[i + 1] - 1; j++)
			for (j = Matrix->iptr[i]; j < Matrix->iptr[i + 1]; j++)
			{
				result[i] += vec[Matrix->jptr[j]] * Matrix->altr[j];
				result[Matrix->jptr[j]] += vec[i] * Matrix->autr[j];
			}
		}
	return result;
	}
};

class Converters
{
public:
	static CRSMatrix COOToCRS(const COOMatrix &Mtx, CRSMatrix Matrix)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0, n = 0, m = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		vector< vector<int> > col_ind_;
		col_ind_.resize(N);
		vector< vector<double> > val_;
		val_.resize(N);

		for (i = 0; i < NNZ; i++)
		{
			val_[Mtx.row_ind[i]].push_back(Mtx.val[i]);
			col_ind_[Mtx.row_ind[i]].push_back(Mtx.col_ind[i]);
			Matrix.row_ptr[Mtx.row_ind[i] + 1]++;
		}

		for (k = 2; k < N + 1; k++)
		{
			Matrix.row_ptr[k] += Matrix.row_ptr[k - 1];
		}

		for (m = 0; m < N; m++) {
			k = 0;
			for (n = Matrix.row_ptr[m]; n < Matrix.row_ptr[m + 1]; n++)
			{
				Matrix.val[n] = val_[m][k];
				Matrix.col_ind[n] = col_ind_[m][k];
				k++;
			}
		}
	}

	static CCSMatrix COOToCCS(const COOMatrix &Mtx, CCSMatrix Matrix)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;

		for (i = 0; i < NNZ; i++)
		{
			Matrix.val[i] = Mtx.val[i];
			Matrix.row_ind[i] = Mtx.row_ind[i];
		}

		for (j = 0; j < NNZ; j++)
		{
			tmp_ind = Mtx.col_ind[j];
			Matrix.col_ptr[++tmp_ind]++;
		}
		while (j < NNZ);

		for (k = 2; k < N + 1; k++)
		{
			Matrix.col_ptr[k] += Matrix.col_ptr[k - 1];
		}
	}

	static CDMatrix COOToCD(const COOMatrix &Mtx, CDMatrix Matrix)
	{
		int i = 0, j = 0, n, k = 0, l = 0, NNZ = 0, N = 0, diag_ind = 0, B = 0, tmp_ind = 0, m = 0, diag_numb = 0;
		bool flag;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		diag_numb = Matrix.B;

		vector< vector<int> > val_;
		val_.resize(diag_numb);
		for (int i = 0; i < diag_numb; i++)
		{
			val_[i].push_back(Matrix.diag[i]);
		}

		for (i = 0; i < NNZ; i++)
		{
			tmp_ind = Mtx.col_ind[i] - Mtx.row_ind[i];
			for (j = 0; j < diag_numb; j++)
			{
				if (tmp_ind == val_[j][0])
					val_[j].push_back(Mtx.val[i]);
			}
		}
		INTTYPE *diag_ptr = new INTTYPE[diag_numb+1];
		diag_ptr[0] = 0;
		for (int j = 1; j < diag_numb+1; j++)
		{
			diag_ptr[j] = val_[j-1].size() - 1;
		}
		for (k = 2; k < diag_numb + 1; k++)
		{
			diag_ptr[k] += diag_ptr[k - 1];
		}
		for (m = 0; m < diag_numb; m++) {
			k = 0;
			int p = 0;
			for (n = diag_ptr[m]; n < diag_ptr[m + 1]; n++)
			{
				Matrix.val[k][p] = val_[k][p + 1];
				p++;
			}
		}

		delete[] diag_ptr;
	}

	static JDMatrix COOToJD(const COOMatrix &Mtx, JDMatrix Matrix)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0, maxval = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		maxval = Matrix.MaxNNZ;
		vector< vector<int> > col_ind_;
		col_ind_.resize(N);
		vector< vector<int> > val_;
		val_.resize(N);
		INTTYPE *nnz_row = new INTTYPE[N];
		INTTYPE *nnz_col = new INTTYPE[N];
		INTTYPE *nnz_row_ind = new INTTYPE[N];

		for (i = 0; i < N; i++)
		{
			nnz_row[i] = Mtx.NNZ_row[i];
			nnz_row_ind[i] = i;
			nnz_col[i] = 0;
		}

		qs(nnz_row, nnz_row_ind, 0, N - 1);

		for (i = 0; i < N; i++)
		{
			Matrix.perm[i] = nnz_row_ind[i];
		}
		for (i = 0; i < NNZ; i++)
		{
			val_[Mtx.row_ind[i]].push_back(Mtx.val[i]);
			col_ind_[Mtx.row_ind[i]].push_back(Mtx.col_ind[i]);
		}
		for (int i = 0; i < N; i++)
		{
			int temp_nnz_col = val_[i].size();
			for (int k = 0; k < temp_nnz_col; k++)
			{
				nnz_col[k]++;
			}
		}

		int l = 0;

		for (i = 0; i < N; i++)
		{
			for (j = 0; j < nnz_col[i]; j++)
			{
				Matrix.jdiag[l] = val_[Matrix.perm[j]][l];
				Matrix.col_ind[l] = col_ind_[Matrix.perm[j]][l];
				l++;
			}
		}
		Matrix.jd_ptr[0] = 0;
		for (i = 1; i < N + 1; i++)
		{
			Matrix.jd_ptr[i] = nnz_col[i - 1];
		}

		for (i = 2; i < N + 1; i++)
		{
			Matrix.jd_ptr[i] += Matrix.jd_ptr[i - 1];
		}

		delete[] nnz_row;
		delete[] nnz_col;
		delete[] nnz_row_ind;

	}

	static SLMatrix COOToSL(const COOMatrix &Mtx, SLMatrix Matrix)
	{
		int i = 0, j = 0, k = 0, l = 0, NNZ = 0, N = 0, tmp_ind = 0, ad = 0, p = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		//int diag = _diag;
		vector< vector<double> > vec;
		vec.resize(N);

		INTTYPE* elem_before_diag = new INTTYPE[N];
		for (int i = 0; i < N; i++)
		{
			elem_before_diag[i] = 0;
		}

		for (i = 0; i < NNZ; i++)
		{
			if (Mtx.col_ind[i] == Mtx.row_ind[i])
			{
				Matrix.adiag[Mtx.col_ind[i]] = Mtx.val[i];
			}
			else if (Mtx.col_ind[i] > Mtx.row_ind[i])
			{
				Matrix.autr[j++] = Mtx.val[i];
				Matrix.jptr[l++] = Mtx.row_ind[i];
			}
			else if ((Mtx.col_ind[i]) < (Mtx.row_ind[i]))
			{
				vec[Mtx.row_ind[i]].push_back(Mtx.val[i]);
				elem_before_diag[Mtx.row_ind[i] + 1]++;
			}
		}
		int m = 0;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < elem_before_diag[j]; j++)
			{
				Matrix.altr[m++] = vec[i][j];
			}

		for (p = 2; p < N + 1; p++)
		{
			Matrix.iptr[p] += Matrix.iptr[p - 1];
		}
		delete[] elem_before_diag;
	}
};

void ReadMatrixInfo(COOMatrix& Matrix, char *name)
{
	FILE* f;
	int i;
	char* line;
	char* p = NULL;


	f = fopen(name, "r");
	if (f == NULL)
	{
		printf("%s- File Not Found!\n", name);
	}
	line = new char[MAX_LINE_LEN];
	do
		fgets(line, MAX_LINE_LEN, f);
	while (line[0] == '%');
	p = strtok(line, " ");
	p = strtok(NULL, " ");
	p = strtok(NULL, " ");
	for (i = 0; i < Matrix.NNZ; i++)
	{
		fgets(line, MAX_LINE_LEN, f);
		p = strtok(line, " ");
		Matrix.row_ind[i] = atoi(p) - 1;
		Matrix.NNZ_row[Matrix.row_ind[i]]++;
		p = strtok(NULL, " ");
		Matrix.col_ind[i] = atoi(p) - 1;
		p = strtok(NULL, " ");
		Matrix.val[i] = atof(p);
	}
	delete[] line;
	fclose(f);
}
COOMatrix *ReadMatrix(char *name)
{
	FILE* f;
	char* line;
	char* p = NULL;
	int N, NNZ, i;
	f = fopen(name, "r");
	if (f == NULL)
		printf("%s- File Not Found!\n", name);
	line = new char[MAX_LINE_LEN];
	do
		fgets(line, MAX_LINE_LEN, f);
	while (line[0] == '%');
	p = strtok(line, " ");
	N = atoi(p);
	p = strtok(NULL, " ");
	p = strtok(NULL, " ");
	NNZ = atoi(p);
	COOMatrix * Matrix = new COOMatrix(NNZ, N);
	for (i = 0; i < Matrix->NNZ; i++)
	{
		fgets(line, MAX_LINE_LEN, f);
		p = strtok(line, " ");
		Matrix->row_ind[i] = atoi(p) - 1;
		Matrix->NNZ_row[Matrix->row_ind[i]]++;
		p = strtok(NULL, " ");
		Matrix->col_ind[i] = atoi(p) - 1;
		p = strtok(NULL, " ");
		Matrix->val[i] = atof(p);
	}
	delete[] line;
	fclose(f);
	return Matrix;
}
void ReadNumberForMatrix(int& N, int& NNZ, char *name)
{
	FILE* f;
	char* line;
	char* p = NULL;

	f = fopen(name, "r");
	if (f == NULL)
		printf("%s- File Not Found!\n", name);
	line = new char[MAX_LINE_LEN];
	do
		fgets(line, MAX_LINE_LEN, f);
	while (line[0] == '%');
	p = strtok(line, " ");
	N = atoi(p);
	p = strtok(NULL, " ");
	p = strtok(NULL, " ");
	NNZ = atoi(p);
	delete[] line;
}
double SearchMax_double(double* arr, int N) 
{
	int i;
	double max_arr = arr[0];
	for (i = 1; i < N; i++)
	{
		if (max_arr < arr[i])
			max_arr = arr[i];
	}
	return max_arr;
}
double CheckCorrectness(double* my_mult, double* mkl_mult, int N) 
{
	int i;
	double res;
	double* arr_abs;
	arr_abs = (double*)malloc(N * sizeof(double));
	for (i = 0; i < N; i++)
		arr_abs[i] = abs(my_mult[i] - mkl_mult[i]);
	res = SearchMax_double(arr_abs, N);
	free(arr_abs);
	return res;
}

int main(int argc, char** argv)
{

	char *fileName;
	//double timer;
	INTTYPE i;
	INTTYPE CDdiag;
	INTTYPE maxvalJD;
	INTTYPE diagelemSL;

	FILE *fp = fopen("data.dt", "w");
	FPTYPE* v;
	FPTYPE* result_crs;
	FPTYPE* result_ccs;
	FPTYPE* result_cd;
	FPTYPE* result_jd;
	FPTYPE* result_sl;
	FPTYPE* result_mkl;

	fileName = new char[256];
	strcpy(fileName, argv[1]);
	fprintf(fp, "Matrix file name: %s\n\n", fileName);


	COOMatrix* Matrix = ReadMatrix(fileName);

	//fprintf(fp, "Time Read Matrix Info: \t%lf\n\n", timer);

	CRSMatrix *CRS = new CRSMatrix(Matrix->NNZ, Matrix->N);
	//fprintf(fp, "Time Convert in \n\nCRS: \t\t%lf\n", timer);
	CCSMatrix *CCS = new CCSMatrix(Matrix->NNZ, Matrix->N);
	//fprintf(fp, "Time Convert in \n\nCCS: \t\t%lf\n", timer);
	CDdiag = Matrix->DiagCDMatrix;
	maxvalJD = Matrix->maxvalJDMatrix;
	CDMatrix *CD = new CDMatrix(Matrix->NNZ, Matrix->N, CDdiag);
	//fprintf(fp, "Time Convert in \n\ncompressed diagonal: \t\t%lf\n", timer);
	JDMatrix *JD = new JDMatrix(Matrix->NNZ, Matrix->N, maxvalJD);
	//fprintf(fp, "Time Convert in \n\njagged diagonal: \t\t%lf\n", timer);
	diagelemSL = Matrix->diagSLMatrix;
	SLMatrix *SL = new SLMatrix(Matrix->NNZ, Matrix->N, diagelemSL);

	v = new FPTYPE[Matrix->N];
	for (int i = 0; i < Matrix->N; i++)
	{
		v[i] = 1;
	}
	result_crs = new FPTYPE[Matrix->N];
	result_ccs = new FPTYPE[Matrix->N];
	result_jd = new FPTYPE[Matrix->N];
	result_cd = new FPTYPE[Matrix->N];
	result_sl = new FPTYPE[Matrix->N];

	Converters::COOToCCS(*Matrix, *CCS);
	Converters::COOToCRS(*Matrix, *CRS);
	Converters::COOToJD(*Matrix, *JD);
	Converters::COOToCD(*Matrix, *CD);
	Converters::COOToSL(*Matrix, *SL);
		
	



	delete[] result_crs;
	delete[] result_ccs;
	delete[] result_cd;
	delete[] result_jd;
	delete[] result_sl;
	delete[] result_mkl;
	return 0;
}