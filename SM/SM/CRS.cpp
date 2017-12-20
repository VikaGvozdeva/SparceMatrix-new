#define _CRT_SECURE_NO_WARNINGS

#include "CRS.h"

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

void CRSMatrix::ReadFromBinaryFile(char *filename)
	{
		FILE *CRSmtx = NULL;
		int N, NNZ;
		CRSmtx = fopen(filename, "rb");
		if (CRSmtx == NULL)
		{
			printf("Error opening file");
		}
		fread(&N, sizeof(INTTYPE), 1, CRSmtx);
		fread(&NNZ, sizeof(INTTYPE), 1, CRSmtx);
		CRSMatrix * Matrix = new CRSMatrix(NNZ, N);
		fread(val, sizeof(FPTYPE), NNZ, CRSmtx);
		fread(col_ind, sizeof(INTTYPE), NNZ, CRSmtx);
		fread(row_ptr, sizeof(INTTYPE), N + 1, CRSmtx);
		fclose(CRSmtx);
	}

	void CRSMatrix::WriteInBinaryFile(char* filename)
	{
		FILE *CRSmtx = NULL;
		CRSmtx = fopen(filename, "wb");
		if (CRSmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&N, sizeof(INTTYPE), 1, CRSmtx);
		fwrite(&NNZ, sizeof(INTTYPE), 1, CRSmtx);
		fwrite(val, sizeof(FPTYPE), NNZ, CRSmtx);
		fwrite(col_ind, sizeof(INTTYPE), NNZ, CRSmtx);
		fwrite(row_ptr, sizeof(INTTYPE), N + 1, CRSmtx);
		fclose(CRSmtx);
	}

	std::ostream & operator<<(ostream &out, const CRSMatrix &Matrix)
	{
		out << "Val : " << endl;
		for (INTTYPE i = 0; i < Matrix.NNZ; i++)
		{
			out << Matrix.val[i] << "  ";
		}
		out << endl;
		out << "col_ind : " << endl;
		for (INTTYPE i = 0; i < Matrix.NNZ; i++)
		{
			out << Matrix.col_ind[i] << "  ";
		}
		out << endl;
		out << "row_ptr : " << endl;
		for (INTTYPE i = 0; i < Matrix.N; i++)
		{
			out << Matrix.row_ptr[i] << "  ";
		}
		out << endl;
		return out;
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
	void CRSMatrix::MatrixVectorMultCRS(FPTYPE *vec, INTTYPE vec_N, FPTYPE *result)
	{
		int i, j;
		double tmp;
		for (i = 0; i < vec_N; i++)
		{
			result[i] = 0;
		}
		if (vec_N == N)
		{
			for (i = 0; i < N; i++)
			{
				tmp = 0;
				for (j = row_ptr[i]; j < row_ptr[i + 1]; j++)
				{
					tmp += val[j] * vec[col_ind[j]];
				}
				result[i] = tmp;
			}
		}
	}

