#define _CRT_SECURE_NO_WARNINGS

//#include "include\COO.h"
#include "COO.h"

//#include "COO.h"

COOMatrix::COOMatrix(INTTYPE  _NNZ, INTTYPE _N)
	{
		int i;
		N = _N;
		NNZ = _NNZ;
		if ((NNZ != 0) && (N != 0))
		{
			val = new FPTYPE[NNZ];
			col_ind = new INTTYPE[NNZ];
			row_ind = new INTTYPE[NNZ];
			NNZ_row = new INTTYPE[N];
			for (i = 0; i < N; i++)
				NNZ_row[i] = 0;
		}
	}
COOMatrix& COOMatrix::operator=(const COOMatrix &Matrix)
{
	int i;
	//if (this != &Matrix) {
		delete[] val;
		delete[] row_ind;
		delete[] col_ind;
		N = Matrix.N;
		NNZ = Matrix.NNZ;

		val = new FPTYPE[NNZ];
		row_ind = new INTTYPE[NNZ];
		col_ind = new INTTYPE[NNZ];
		std::cout << "was here" << std::endl;
		for (i = 0; i < NNZ; i++)
		{
			row_ind[i] = Matrix.row_ind[i];
			val[i] = Matrix.val[i];
			col_ind[i] = Matrix.col_ind[i];
		}
		/*for (i = 0; i < N + 1; i++)
			col_ptr[i] = Matrix.col_ptr[i];*/

	//}
	return *this;
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
		COOmtx = fopen(filename, "rb");
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
		fread(Matrix->NNZ_row, sizeof(INTTYPE), Matrix->N, COOmtx);
		fclose(COOmtx);
		return Matrix;
	}

	void COOMatrix::WriteInBinaryFile(const COOMatrix& Matrix, char* filename)
	{
		FILE *COOmtx = NULL;
		COOmtx = fopen(filename, "wb");
		if (COOmtx == NULL)
		{
			printf("Error opening file");
		}
		fwrite(&Matrix.N, sizeof(INTTYPE), 1, COOmtx);
		fwrite(&Matrix.NNZ, sizeof(INTTYPE), 1, COOmtx);
		fwrite(Matrix.val, sizeof(FPTYPE), Matrix.NNZ, COOmtx);
		fwrite(Matrix.col_ind, sizeof(INTTYPE), Matrix.NNZ, COOmtx);
		fwrite(Matrix.row_ind, sizeof(INTTYPE), Matrix.NNZ, COOmtx);
		fwrite(Matrix.NNZ_row, sizeof(INTTYPE), Matrix.N, COOmtx); 
		fclose(COOmtx);
	}

	/*COOMatrix & COOMatrix::operator=(const COOMatrix & Matrix)
	{
		{
			int i;
			if (this != &Matrix) {
				delete[] val;
				delete[] row_ind;
				delete[] col_ind;
				N = Matrix.N;
				NNZ = Matrix.NNZ;

				val = new FPTYPE[NNZ];
				row_ind = new INTTYPE[NNZ];
				col_ind = new INTTYPE[NNZ];
				for (i = 0; i < NNZ; i++)
				{
					row_ind[i] = Matrix.row_ind[i];
					val[i] = Matrix.val[i];
					col_ind[i] = Matrix.col_ind[i];
				}

			}
			return *this;
		}
	}
*/
	INTTYPE COOMatrix::DiagCDMatrix(const COOMatrix& Matrix)
	{
		int i, tmp_ind, m = 0, j;
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
				diag[m] = tmp_ind;
				m++;
			}


		}

		diag_cd = m;

		delete[] diag;
		return diag_cd;
	}
	INTTYPE COOMatrix::nonZeroRows(const COOMatrix& Matrix)
	{
		INTTYPE row = Matrix.N;
		for (int i = 1; i< N; i++)
		{
			if (Matrix.NNZ_row[i]==0)
			{
				N--;
			}
		}
		return row;
	}
	INTTYPE COOMatrix::maxvalJDMatrix(const COOMatrix& Matrix)
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
		maxval_jd = maxval;
		return maxval;
	}
	INTTYPE COOMatrix::diagSLMatrix(const COOMatrix& Matrix)
	{
		int diagelem = 0;
		for (int i = 0; i < NNZ; i++)
		{
			if (Matrix.row_ind[i] == Matrix.col_ind[i])
				diagelem++;
		}
		return diagelem;
	}

	void  COOMatrix::ReadMatrixInfo(COOMatrix& Matrix, char *name)
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
	COOMatrix * COOMatrix::ReadMatrix(char *name)
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
	void COOMatrix::ReadNumberForMatrix(int& N, int& NNZ, char *name)
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

