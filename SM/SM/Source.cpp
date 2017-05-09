#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

typedef int64_t INTTYPE;
typedef int32_t INTTYPE32;
typedef double FPTYPE;
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
			col_ind = &Matrix.col_ind[j];
			row_ind = &Matrix.row_ind[j];
			val = &Matrix.val[j];
		}
	}
	COOMatrix *COOMatrix::ReadFromBinaryFile(char *filename)
	{
		//
	}
	void COOMatrix::WriteInBinaryFile(COOMatrix Matrix)
	{
		//
	}
	FPTYPE* COOMatrix::MatrixVectorMultCOO(COOMatrix *Matrix, FPTYPE *vec, INTTYPE N, FPTYPE *result)
	{
		//
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
			col_ind = &Matrix.col_ind[j];
			val = &Matrix.val[j];
		}

	}

	CRSMatrix:: ~CRSMatrix() {
		delete[] col_ind;
		delete[] row_ptr;
		delete[] val;

	}

	CRSMatrix *CRSMatrix::ReadFromBinaryFile(char *filename)
	{
		//
	}

	void CRSMatrix::WriteInBinaryFile(CRSMatrix Matrix)
	{
		//
	}
	//CRSMatrix& CRSMatrix::operator=(const CRSMatrix &Matrix) {}
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
		//
	}

	void CCSMatrix::WriteInBinaryFile(CCSMatrix Matrix)
	{
		//
	}
	//CCSMatrix& CCSMatrix::operator=(const CCSMatrix &Matrix) {}
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
	}
	CDMatrix *CDMatrix::ReadFromBinaryFile(char *filename)
	{
		//
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

	SLMatrix::SLMatrix(INTTYPE  _NNZ, INTTYPE _N)
	{
		//
	}
	SLMatrix::~SLMatrix()
	{
		//
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
		//
	}
};

class Converters
{
	static CRSMatrix COOToCRS(const COOMatrix Mtx, CRSMatrix Matrix)
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

	static CCSMatrix COOToCCS(const COOMatrix Mtx, CCSMatrix Matrix)
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

	static CDMatrix COOToCD(const COOMatrix Mtx, CDMatrix Matrix)
	{
		//
		int i = 0, j = 0, k = 0, l = 0, NNZ = 0, N = 0, diag_ind = 0, B = 0, tmp_ind = 0, m = 0, diag_numb = 0;
		bool flag;
		NNZ = Matrix->NNZ;
		N = Matrix->N;

		int *diag = (int*)malloc((2 * N - 1) * sizeof(int*));
		for (i = 0; i < 2 * N - 1; i++)
		{
			diag[i] = N + 1;
		}
		for (i = 0; i < NNZ; i++)
		{
			tmp_ind = Matrix->col_ind[i] - Matrix->row_ind[i];
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
		//âûçâàòü ñòàíäàðòíûé êóñîðò
		bubblesort(diag, m);

		COMPDIAGMATRIX* Mtx = new COMPDIAGMATRIX(N, NNZ, diag_numb);
		for (i = 0; i < diag_numb; i++)
		{
			Mtx->diag[i] = diag[i];
		}


		Matrix->Sort(Matrix->NNZ);

		for (i = 0; i < NNZ; i++)
		{
			tmp_ind = Matrix->col_ind[i] - Matrix->row_ind[i];
			for (j = 0; j < diag_numb; j++)
			{
				if (tmp_ind == Mtx->diag[j])
					Mtx->val[Matrix->row_ind[i]][j] = Matrix->val[i];

			}
		}
		free(diag);
		return Mtx;
	}

	static JDMatrix COOToJD(const COOMatrix Mtx, JDMatrix Matrix)
	{
		//
	}

	static SLMatrix COOToSL(const COOMatrix Mtx, INTTYPE _diag, SLMatrix Matrix)
	{
		int i = 0, j = 0, k = 0, l = 0, NNZ = 0, N = 0, tmp_ind = 0, ad = 0, p = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		int diag = _diag;
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

int main()
{
	return 0;
}