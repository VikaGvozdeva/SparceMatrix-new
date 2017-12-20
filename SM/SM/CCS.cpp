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

	void CCSMatrix::ReadFromBinaryFile(char *filename)
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
		fread(val, sizeof(FPTYPE), NNZ, CCSmtx);
		fread(row_ind, sizeof(INTTYPE), NNZ, CCSmtx);
		fread(col_ptr, sizeof(INTTYPE), N + 1, CCSmtx);
		fclose(CCSmtx);
	}

	void CCSMatrix::WriteInBinaryFile(char* filename)
	{
		FILE *CCSmtx = NULL;
		CCSmtx = fopen(filename, "wb");
		if (CCSmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&N, sizeof(INTTYPE), 1, CCSmtx);
		fwrite(&NNZ, sizeof(INTTYPE), 1, CCSmtx);
		fwrite(val, sizeof(FPTYPE),NNZ, CCSmtx);
		fwrite(row_ind, sizeof(INTTYPE), NNZ, CCSmtx);
		fwrite(col_ptr, sizeof(INTTYPE), N + 1, CCSmtx);
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
	void CCSMatrix::MatrixVectorMultCCS(FPTYPE *vec, INTTYPE vec_N, FPTYPE *result)
	{
		int i, j, k;
		for (k = 0; k < vec_N; k++)
		{
			result[k] = 0;
		}
		if (vec_N == N)
		{
			for (i = 0; i < N; i++)
			{
				for (j = col_ptr[i]; j < col_ptr[i + 1]; j++)
				{
					result[row_ind[j]] += vec[i] * val[j];
				}
			}
		}
	}

	std::ostream & operator<<(ostream &out, const CCSMatrix &Matrix)
	{
		out << "Val : " << endl;
		for (INTTYPE i = 0; i < Matrix.NNZ; i++)
		{
			out << Matrix.val[i] << "  ";
		}
		out << endl;
		out << "row_ind : " << endl;
		for (INTTYPE i = 0; i < Matrix.NNZ; i++)
		{
			out << Matrix.row_ind[i] << "  ";
		}
		out << endl;
		out << "col_ptr : " << endl;
		for (INTTYPE i = 0; i < Matrix.N; i++)
		{
			out << Matrix.col_ptr[i] << "  ";
		}
		out << endl;
		return out;
	}
