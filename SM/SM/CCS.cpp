#define _CRT_SECURE_NO_WARNINGS

#include "CCS.h"

CCSMatrix::CCSMatrix(INTTYPE _NNZ, INTTYPE _N)
	{
		N = _N;
		NNZ = _NNZ;
		if ((NNZ != 0) && (N != 0))
		{
			val = new FPTYPE[NNZ];
			row_ind = new INTTYPE[NNZ];
			col_ptr = new INTTYPE[N + 1];
			for (int i = 0; i < N + 1; i++)
			{
				col_ptr[i] = 0;
			}
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

CCSMatrix::~CCSMatrix() {
		delete[] row_ind;
		delete[] col_ptr;
		delete[] val;

	}

	CCSMatrix* CCSMatrix::ReadFromBinaryFile(char *filename)
	{
		FILE *CCSmtx = NULL;
		int N, NNZ;
		CCSmtx = fopen(filename, "rb");
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

	void CCSMatrix::WriteInBinaryFile(CCSMatrix& Matrix, char* filename)
	{
		FILE *CCSmtx = NULL;
		CCSmtx = fopen(filename, "wb");
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

