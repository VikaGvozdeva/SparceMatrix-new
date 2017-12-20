#define _CRT_SECURE_NO_WARNINGS
#include "SL.h"

SLMatrix::SLMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _diag)
	{
		int i;
		N = _N;
		NNZ = _NNZ;
		diag = _diag;
		if ((NNZ != 0) && (N != 0) &&(diag!=0))
		{
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
	}
SLMatrix::SLMatrix(const SLMatrix &Matrix)
{
	int i;
	N = Matrix.N;
	NNZ = Matrix.NNZ;
	diag = Matrix.diag;
	//numbdiag = Matrix.MaxNNZ;
	adiag = new FPTYPE[N];
	altr = new FPTYPE[(NNZ - diag) / 2];
	autr = new FPTYPE[(NNZ - diag) / 2];
	jptr = new INTTYPE[(NNZ - diag) / 2];
	iptr = new INTTYPE[N + 1];
	for (i = 0; i < (NNZ - diag) / 2; i++)
	{
		altr[i] = Matrix.altr[i];
		autr[i] = Matrix.autr[i];;
		jptr[i] = Matrix.jptr[i];;
	}
	for (i = 0; i < N; i++)
	{
		adiag[i] = Matrix.adiag[i];;
	}
	for (i = 0; i < N + 1; i++)
	{
		iptr[i] = Matrix.iptr[i];;
	}

}

SLMatrix& SLMatrix::operator=(const SLMatrix &Matrix)
{
	int i;
	if (this != &Matrix) {
		delete[] autr;
		delete[] altr;
		delete[] iptr;
		delete[] jptr;
		delete[] adiag;
		N = Matrix.N;
		NNZ = Matrix.NNZ;
		diag = Matrix.diag;

		adiag = new FPTYPE[N];
		altr = new FPTYPE[(NNZ - diag) / 2];
		autr = new FPTYPE[(NNZ - diag) / 2];
		jptr = new INTTYPE[(NNZ - diag) / 2];
		iptr = new INTTYPE[N + 1];

		for (i = 0; i < (NNZ - diag) / 2; i++)
		{
			altr[i] = Matrix.altr[i];
			autr[i] = Matrix.autr[i];;
			jptr[i] = Matrix.jptr[i];;
		}
		for (i = 0; i < N; i++)
		{
			adiag[i] = Matrix.adiag[i];;
		}
		for (i = 0; i < N + 1; i++)
		{
			iptr[i] = Matrix.iptr[i];;
		}

	}
	return *this;
}
SLMatrix::~SLMatrix()
	{
		delete[] autr;
		delete[] altr;
		delete[] iptr;
		delete[] jptr;
		delete[] adiag;
	}

void SLMatrix::PrintMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _diag)
	{
		int i;
		printf("adiag:");
		for (i = 0; i < _N; i++)
			printf("%lf , ", adiag[i]);
		printf("altr:");
		for (i = 0; i < (_NNZ - _diag) / 2; i++)
			printf("%lf , ", altr[i]);
			printf("autr:");
		for (i = 0; i < (_NNZ - _diag) / 2; i++)
			printf("%lf , ", autr[i]);
			printf("jptr:");
		for (i = 0; i < (_NNZ - _diag) / 2; i++)
			printf("%d , ", jptr[i]);
					printf("iptr:");
		for (i = 0; i < _N + 1; i++)
			printf("%d , ", iptr[i]);
	

	}

void SLMatrix::ReadFromBinaryFile(char *filename)
	{

		FILE *SLmtx = NULL;
		int N, NNZ;
		SLmtx = fopen(filename, "rb");
		if (SLmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, SLmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, SLmtx);
		fread(&diag, sizeof(INTTYPE), 1, SLmtx);
		fread(adiag, sizeof(FPTYPE),N, SLmtx);
		fread(altr, sizeof(FPTYPE), (NNZ - diag) / 2, SLmtx);
		fread(autr, sizeof(FPTYPE), (NNZ - diag) / 2, SLmtx);
		fread(jptr, sizeof(INTTYPE), (NNZ - diag) / 2, SLmtx);
		fread(iptr, sizeof(INTTYPE), N + 1, SLmtx);
		fclose(SLmtx);

	}
	void SLMatrix::WriteInBinaryFile(char* filename)
	{
		FILE *SLmtx = NULL;
		SLmtx = fopen(filename, "wb");
		if (SLmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&N, sizeof(INTTYPE), 1, SLmtx);
		fwrite(&NNZ, sizeof(INTTYPE), 1, SLmtx);
		fwrite(&diag, sizeof(INTTYPE), 1, SLmtx);
		fwrite(adiag, sizeof(FPTYPE), N, SLmtx);
		fwrite(altr, sizeof(FPTYPE), (NNZ - diag) / 2, SLmtx);
		fwrite(autr, sizeof(FPTYPE), (NNZ - diag) / 2, SLmtx);
		fwrite(jptr, sizeof(INTTYPE), (NNZ - diag) / 2, SLmtx);
		fwrite(iptr, sizeof(INTTYPE), N + 1, SLmtx);
		fclose(SLmtx);
	}

	void SLMatrix::MatrixVectorMultSL(FPTYPE *vec, INTTYPE vec_N, FPTYPE *result)
	{
		int i, j;
		int m = 0;
		int up = 0, low = 0;
		for (i = 0; i < vec_N; i++)
		{
			result[i] = 0;
		}
		for (i = 0; i < N; i++)
		{
			result[i] = vec[i] * adiag[i];
		}
		for (i = 1; i < N; i++)
		{
				for (j = iptr[i]; j < iptr[i + 1]; j++)
				{
					result[i] += vec[jptr[up]] * altr[up];
					result[jptr[low]] += vec[i] * autr[low];
					up++; low++;
				}
		}
	}
