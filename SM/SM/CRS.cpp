#define _CRT_SECURE_NO_WARNINGS

#include "include\CRS.h"

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

CRSMatrix::~CRSMatrix() {
		delete[] col_ind;
		delete[] row_ptr;
		delete[] val;

	}

	CRSMatrix* CRSMatrix::ReadFromBinaryFile(char *filename)
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
		fread(Matrix->row_ptr, sizeof(INTTYPE), Matrix->N + 1, CRSmtx);
		fclose(CRSmtx);
		return Matrix;
	}

	void CRSMatrix::WriteInBinaryFile(const CRSMatrix& Matrix, char* filename)
	{
		FILE *CRSmtx = NULL;
		CRSmtx = fopen(filename, "wb");
		if (CRSmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&Matrix.N, sizeof(INTTYPE), 1, CRSmtx);
		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, CRSmtx);
		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, CRSmtx);
		fwrite(Matrix.col_ind, sizeof(INTTYPE), Matrix.NNZ, CRSmtx);
		fwrite(Matrix.row_ptr, sizeof(INTTYPE), Matrix.N + 1, CRSmtx);
		fclose(CRSmtx);
	}

	void CRSMatrix::PrintMatrix(int _NNZ, int _N)
	{
		int i;
		N = _N;
		NNZ = _NNZ;
		printf("val:");
		for (i = 0; i < NNZ; i++)
			printf("%lf , ", val[i]);
		printf(" \n col_ind:");
		for (i = 0; i < NNZ; i++)
			printf("%d , ", col_ind[i]);
		printf(" \n row_ptr:");
		for (i = 0; i < N + 1; i++)
			printf("%d , ", row_ptr[i]);
		printf("\n");
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

