#define _CRT_SECURE_NO_WARNINGS
#include "SL.h"

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

	}
SLMatrix *SLMatrix::ReadFromBinaryFile(char *filename)
	{

		FILE *SLmtx = NULL;
		int N, NNZ;
		SLmtx = fopen("SLmtx.bin", "rb");
		if (SLmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, SLmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, SLmtx);
		fread(&diag, sizeof(INTTYPE), 1, SLmtx);
		SLMatrix * Matrix = new SLMatrix(NNZ, N, diag);
		fread(Matrix->adiag, sizeof(FPTYPE), Matrix->N, SLmtx);
		fread(Matrix->altr, sizeof(FPTYPE), (Matrix->NNZ - Matrix->diag) / 2, SLmtx);
		fread(Matrix->autr, sizeof(FPTYPE), (Matrix->NNZ - Matrix->diag) / 2, SLmtx);
		fread(Matrix->jptr, sizeof(INTTYPE), (Matrix->NNZ - Matrix->diag) / 2, SLmtx);
		fread(Matrix->iptr, sizeof(INTTYPE), Matrix->N + 1, SLmtx);
		fclose(SLmtx);
		return Matrix;

	}
	void SLMatrix::WriteInBinaryFile(SLMatrix Matrix)
	{
		FILE *SLmtx = NULL;
		SLmtx = fopen("SLmtx.bin", "wb");
		if (SLmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&Matrix.N, sizeof(INTTYPE), 1, SLmtx);
		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, SLmtx);
		fwrite(&Matrix.diag, sizeof(INTTYPE), 1, SLmtx);
		fwrite(Matrix.adiag, sizeof(FPTYPE), Matrix.N, SLmtx);
		fwrite(Matrix.altr, sizeof(FPTYPE), (Matrix.NNZ - Matrix.diag) / 2, SLmtx);
		fwrite(Matrix.autr, sizeof(FPTYPE), (Matrix.NNZ - Matrix.diag) / 2, SLmtx);
		fwrite(Matrix.jptr, sizeof(INTTYPE), (Matrix.NNZ - Matrix.diag) / 2, SLmtx);
		fwrite(Matrix.iptr, sizeof(INTTYPE), Matrix.N + 1, SLmtx);
		fclose(SLmtx);
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

			for (j = Matrix->iptr[i]; j < Matrix->iptr[i + 1] - 1; j++)
				for (j = Matrix->iptr[i]; j < Matrix->iptr[i + 1]; j++)
				{
					result[i] += vec[Matrix->jptr[j]] * Matrix->altr[j];
					result[Matrix->jptr[j]] += vec[i] * Matrix->autr[j];
				}
		}
		return result;
	}
