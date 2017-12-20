#define _CRT_SECURE_NO_WARNINGS

#include "CD.h"
int  compare(const void * x1, const void * x2)
{
	return (*(int*)x1 - *(int*)x2);

}
CDMatrix::CDMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _B)
{
	int i;
	int j;
	N = _N;
	NNZ = _NNZ;
	B = _B;
	//R = _R;
	if ((NNZ != 0) && (N != 0) && (B != 0))
	{
		diag = new INTTYPE[B];
		val = new FPTYPE*[N];
		for (i = 0; i < B; i++)
		{
			diag[i] = 0;
		}
		for (i = 0; i < N; i++)
		{
			val[i] = new FPTYPE[B];
		}
		for (i = 0; i < N; i++) {
			for (j = 0; j < B; j++)
				val[i][j] = 0;
		}
	}
}

	void CDMatrix::Print()
	{
		int i, j;
		printf(" \n diag:");
		for (i = 0; i < B; i++)
			printf("%d , ", diag[i]);

		printf("val:");
		for (i = 0; i < B; i++)
		{
			printf("diag  \n", i);
			for (j = 0; j < N; j++)
				printf("%lf , ", val[i][j], "\n");
			
		}
		
		printf("\n");
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
		for (i = 0; i < N; i++)
			for (int j = 0; j < B; j++)
			{
				val[i][j] = Matrix.val[i][j];
			}
	}

	CDMatrix& CDMatrix::operator=(const CDMatrix &Matrix)
	{
		int i, j;
		if (this != &Matrix) {
			delete[] diag;
			for (int i = 0; i < N; i++)
			{
				delete[] val[i];
			}
			delete[] val;

			N = Matrix.N;
			NNZ = Matrix.NNZ;
			B = Matrix.B;
			//R = Matrix.R;

			diag = new INTTYPE[B];
			val = new FPTYPE*[N];
			for (i = 0; i < B; i++)
			{
				diag[i] = Matrix.diag[i];
			}
			for (i = 0; i < N; i++)
			{
				val[i] = new FPTYPE[B];
			}
			for (i = 0; i < N; i++) {
				for (j = 0; j < B; j++)
					val[i][j] = Matrix.val[i][j];
			}

		}
		return *this;
	}
	void CDMatrix::ReadFromBinaryFile(char *filename)
	{
		FILE *CDmtx = NULL;
		int N, NNZ;
		CDmtx = fopen(filename, "rb");
		if (CDmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, CDmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, CDmtx);
		fread(&B, sizeof(INTTYPE), 1, CDmtx);
		fread(diag, sizeof(INTTYPE), B, CDmtx);
		for (int i = 0; i < B; i++)
		{
			fread(val[i], sizeof(FPTYPE),N, CDmtx);
		}
		fclose(CDmtx);
	}
	void CDMatrix::FillDiagArray(const COOMatrix Matrix)
	{
		int i, j, tmp_ind, m = 0, diag_numb;
		bool flag;

		INTTYPE *temp = new INTTYPE[2 * N - 1];

		for (i = 0; i < 2 * N - 1; i++)
		{
			//diag[i] = N + 1;
			temp[i] = N + 1;
		}
		for (i = 0; i < NNZ; i++)
		{
			tmp_ind = Matrix.col_ind[i] - Matrix.row_ind[i];

			for (j = 0; j < 2 * N - 1; j++)
			{
				//if ((tmp_ind) != diag[j])
				if ((tmp_ind) != temp[j])
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
				//diag[m++] = tmp_ind;
				temp[m++] = tmp_ind;
			}

		}

		diag_numb = m;
		qsort(&temp[0], m, sizeof(INTTYPE), compare);
		for (int i = 0; i < m; i++)
		{
			diag[i] = temp[i];
		}
		delete[] temp;
	}

	void CDMatrix::WriteInBinaryFile(char* filename)
	{
		FILE *CDmtx = NULL;
		CDmtx = fopen(filename, "wb");
		if (CDmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&N, sizeof(INTTYPE), 1, CDmtx);
		fwrite(&NNZ, sizeof(INTTYPE), 1, CDmtx);
		fwrite(&B, sizeof(INTTYPE), 1, CDmtx);
		fwrite(diag, sizeof(INTTYPE), B, CDmtx);
		fwrite(val, sizeof(INTTYPE), B * N, CDmtx);
		fclose(CDmtx);
	}
	void CDMatrix::MatrixVectorMultCD(FPTYPE *vec, INTTYPE vec_N, FPTYPE *result)
	{
		int i, j;
		int tmp;
		int max;
		//int B = Matrix->B;
		for (i = 0; i < N; i++)
		{
			result[i] = 0;
		}

		for (j = 0; j < B; j++) {
			tmp = diag[j];
			if (0 >(0 - tmp))
				i = 0;
			else i = 0 - tmp;

			if (0 > tmp)
				max = N - 0;
			else max = N - tmp;

			for (i; i < max; i++)
				result[i] += val[i][j] * vec[tmp + i];

		}
	}

