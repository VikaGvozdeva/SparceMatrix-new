#define _CRT_SECURE_NO_WARNINGS
#include "JD.h"
JDMatrix::JDMatrix(INTTYPE  _NNZ, INTTYPE _N, INTTYPE _MaxNNZ)
	{
		int i;
		N = _N;
		NNZ = _NNZ;
		MaxNNZ = _MaxNNZ;
		if ((NNZ != 0) && (N != 0) && (MaxNNZ != 0))
		{
			jdiag = new FPTYPE[NNZ];
			perm = new INTTYPE[N];
			col_ind = new INTTYPE[NNZ];
			jd_ptr = new INTTYPE[MaxNNZ + 1];
			for (i = 0; i < MaxNNZ + 1; i++)
			{
				jd_ptr[i] = 0;
			}
			for (i = 0; i < N; i++)
			{
				perm[i] = 0;
			}
			for (i = 0; i < NNZ; i++)
			{
				jdiag[i] = 0;
				col_ind[i] = 0;
			}
		}
	}
JDMatrix::~JDMatrix()
	{
		delete[] perm;
		delete[] jdiag;
		delete[] col_ind;
		delete[] jd_ptr;

	}
JDMatrix& JDMatrix::operator=(const JDMatrix &Matrix)
{
	int i;
if (this != &Matrix) {
		delete[] perm;
		delete[] jdiag;
		delete[] col_ind;
		delete[] jd_ptr;
		N = Matrix.N;
		NNZ = Matrix.NNZ;
		MaxNNZ = Matrix.MaxNNZ;


		jdiag = new FPTYPE[NNZ];
		perm = new INTTYPE[N];
		col_ind = new INTTYPE[NNZ];
		jd_ptr = new INTTYPE[MaxNNZ + 1];
		for (i = 0; i < MaxNNZ + 1; i++)
		{
			jd_ptr[i] = Matrix.jd_ptr[i];
		}
		for (i = 0; i < N ; i++)
		{
			perm[i] = Matrix.perm[i];
		}
		for (i = 0; i < NNZ; i++)
		{
			jdiag[i] = Matrix.jdiag[i];
			col_ind[i] = Matrix.col_ind[i];
		}

	}
	return *this;
}
JDMatrix::JDMatrix(const JDMatrix &Matrix)
	{
	int i;
	N = Matrix.N;
	NNZ = Matrix.NNZ;
	MaxNNZ = Matrix.MaxNNZ;
	//numbdiag = Matrix.MaxNNZ;

	jdiag = new FPTYPE[NNZ];
	perm = new INTTYPE[N];
	col_ind = new INTTYPE[NNZ];
	jd_ptr = new INTTYPE[MaxNNZ + 1];
	for (i = 0; i < MaxNNZ + 1; i++)
	{
		jd_ptr[i] = Matrix.jd_ptr[i];
	}
	for (i = 0; i < N; i++)
	{
		perm[i] = Matrix.perm[i];
	}
	for (i = 0; i < NNZ; i++)
	{
		jdiag[i] = Matrix.jdiag[i];
		col_ind[i] = Matrix.col_ind[i];
	}
	}
	void JDMatrix::ReadFromBinaryFile(char *filename)
	{
		FILE *JDmtx = NULL;
		int N, NNZ, MaxNNZ;
		JDmtx = fopen(filename, "rb");
		if (JDmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, JDmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, JDmtx);
		fread(&MaxNNZ, sizeof(INTTYPE), 1, JDmtx);
		fread(jdiag, sizeof(FPTYPE), NNZ, JDmtx);
		fread(col_ind, sizeof(INTTYPE), NNZ, JDmtx);
		fread(jd_ptr, sizeof(INTTYPE), MaxNNZ + 1, JDmtx);
		fread(perm, sizeof(INTTYPE), N, JDmtx);
		fclose(JDmtx);

	}

	void JDMatrix::WriteInBinaryFile(char* filename)
	{

		FILE *JDmtx = NULL;
		JDmtx = fopen(filename, "wb");
		if (JDmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&N, sizeof(INTTYPE), 1, JDmtx);
		fwrite(&NNZ, sizeof(INTTYPE), 1, JDmtx);
		fwrite(&MaxNNZ, sizeof(INTTYPE), 1, JDmtx);
		fwrite(jdiag, sizeof(FPTYPE), NNZ, JDmtx);
		fwrite(col_ind, sizeof(INTTYPE), NNZ, JDmtx);
		fwrite(jd_ptr, sizeof(INTTYPE), MaxNNZ + 1, JDmtx);
		fwrite(perm, sizeof(INTTYPE), N, JDmtx);
		fclose(JDmtx);
	}
	void JDMatrix::Print()
	{
		int i;
		printf("jdiag:");
		for (i = 0; i < NNZ; i++)
			printf("%lf , ", jdiag[i]);
		printf("col_ind:");
		for (i = 0; i < NNZ; i++)
			printf("%d , ", col_ind[i]);
		printf("perm:");
		for (i = 0; i < N; i++)
			printf("%d , ", perm[i]);
		printf("jd_ptr:");
		for (i = 0; i < MaxNNZ +1; i++)
			printf("%d , ", jd_ptr[i]);

	}
	void JDMatrix::MatrixVectorMultJD(FPTYPE *vec, INTTYPE vec_N, FPTYPE *result)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, tmp_ind = 0, maxval = 0, upper = 0;

		FPTYPE* temp = new FPTYPE[N];
		for (i = 0; i < N; i++)
		{
			result[i] = 0;
		}

		int disp = 0;
		for (int j = 0; j < MaxNNZ; j++)
		{

			for (int i = 0; i < (jd_ptr[j + 1] - jd_ptr[j]); i++)
			{
				result[i] += jdiag[disp] * vec[col_ind[disp]];
				disp++;
			}
		}


		for (i = 0; i < N; i++)
		{
			temp[perm[i]] = result[i];
		}
		for (i = 0; i < N; i++)
		{
			result[i] = temp[i];
		}

		free(temp);
	}

