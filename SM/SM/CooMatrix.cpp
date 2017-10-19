#define _CRT_SECURE_NO_WARNINGS
#include "CooMatrix.h"

CooMatrix::CooMatrix(INTTYPE _nnz, INTTYPE _n) //конструктор инициализатор
{
	n = _n;
	nnz = _nnz;

	if (nnz != 0)
	{
		val = new FPTYPE[nnz];
		col_ind = new INTTYPE[nnz];
		row_ind = new INTTYPE[nnz];
	}
	if (n != 0)
	{
		nnz_row = new INTTYPE[n];
		memset(nnz_row, 0, sizeof(INTTYPE)*n);
	}
}

CooMatrix::CooMatrix(const CooMatrix &Matrix) //конструктор копирования
{
	n = Matrix.n;
	nnz = Matrix.nnz;
	val = new FPTYPE[nnz];
	col_ind = new INTTYPE[nnz];
	row_ind = new INTTYPE[nnz];
	nnz_row = new INTTYPE[n];
	memcpy(nnz_row, Matrix.nnz_row, sizeof(INTTYPE)*n);
	memcpy(val, Matrix.val, sizeof(FPTYPE)*nnz);
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE)*nnz);
	memcpy(row_ind, Matrix.row_ind, sizeof(INTTYPE)*nnz);
}

CooMatrix :: ~CooMatrix()//деструктор
{
	if (n != 0)
		delete[] nnz_row;
	if (nnz != 0)
	{
		delete[] val;
		delete[] col_ind;
		delete[] row_ind;
	}
}

CooMatrix& CooMatrix :: operator=(const CooMatrix &Matrix)    // присваивание
{
	if (this == &Matrix)
		return *this;
	DeleteAndAllocateMemory(Matrix.n, Matrix.nnz);
	memcpy(nnz_row, Matrix.nnz_row, sizeof(INTTYPE)*n);
	memcpy(val, Matrix.val, sizeof(FPTYPE)*nnz);
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE)*nnz);
	memcpy(row_ind, Matrix.row_ind, sizeof(INTTYPE)*nnz);
	return *this;
}

void CooMatrix::ReadMatrix(char * filename) //чтение матрицы
{
	FILE* f;
	char* line;
	char* p = NULL;
	INTTYPE _n, _nnz;
	f = fopen(filename, "r");
	if (f == NULL)
		printf("%s- File Not Found!\n", filename);
	line = new char[MAX_LINE_LEN];
	do
		fgets(line, MAX_LINE_LEN, f);
	while (line[0] == '%');
	p = strtok(line, " ");
	_n = atoi(p);
	p = strtok(NULL, " ");
	p = strtok(NULL, " ");
	_nnz = atoi(p);

	DeleteAndAllocateMemory(_n, _nnz);
	for (INTTYPE i = 0; i < nnz; i++)
	{
		fgets(line, MAX_LINE_LEN, f);
		p = strtok(line, " ");
		row_ind[i] = atoi(p) - 1;
		nnz_row[row_ind[i]]++;
		p = strtok(NULL, " ");
		col_ind[i] = atoi(p) - 1;
		p = strtok(NULL, " ");
		val[i] = atof(p);
	}
	delete[]line;
	fclose(f);
}

void CooMatrix::ReadFromBinaryFile(char *filename)  // чтение из бинарного файла
{
	FILE *COOmtx = NULL;
	INTTYPE _n, _nnz;
	COOmtx = fopen(filename, "rb");
	if (COOmtx == NULL)
	{
		printf("Error opening file");
	}
	fread(&_n, sizeof(INTTYPE), 1, COOmtx);
	fread(&_nnz, sizeof(INTTYPE), 1, COOmtx);
	DeleteAndAllocateMemory(_n, _nnz);
	fread(val, sizeof(FPTYPE), nnz, COOmtx);
	fread(col_ind, sizeof(INTTYPE), nnz, COOmtx);
	fread(row_ind, sizeof(INTTYPE), nnz, COOmtx);
	fread(nnz_row, sizeof(INTTYPE), n, COOmtx);
	fclose(COOmtx);
}

void CooMatrix::WriteInBinaryFile(char *filename)  //запись в бинарный файл
{
	FILE *COOmtx = NULL;
	COOmtx = fopen(filename, "wb");
	if (COOmtx == NULL)
	{
		printf("Error opening file");
	}
	fwrite(&n, sizeof(INTTYPE), 1, COOmtx);
	fwrite(&nnz, sizeof(INTTYPE), 1, COOmtx);
	fwrite(val, sizeof(FPTYPE), nnz, COOmtx);
	fwrite(col_ind, sizeof(INTTYPE), nnz, COOmtx);
	fwrite(row_ind, sizeof(INTTYPE), nnz, COOmtx);
	fwrite(nnz_row, sizeof(INTTYPE), n, COOmtx);
	fclose(COOmtx);
}

std::ostream & operator<<(ostream &out, const CooMatrix &Matrix)
{
	for (INTTYPE i = 0; i < Matrix.nnz; i++)
	{
		out << "(" << Matrix.val[i] << "," << Matrix.row_ind[i] << "," << Matrix.col_ind[i] << ") ,";
	}
	out << endl;
	return out;
}
void CooMatrix::DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _nnz)
{
	if (n != _n)
	{
		if (n != 0)
			delete[]nnz_row;
		nnz_row = new INTTYPE[_n];
		memset(nnz_row, 0, sizeof(INTTYPE)*_n);
		n = _n;
	}
	if (nnz != _nnz)
	{
		if (nnz != 0)
		{
			delete[]val;
			delete[]col_ind;
			delete[]row_ind;
		}
		val = new FPTYPE[_nnz];
		col_ind = new INTTYPE[_nnz];
		row_ind = new INTTYPE[_nnz];
		nnz = _nnz;
	}
}
void CooMatrix::SortWithVectors()
{
	INTTYPE k = 0;
	vector < vector<INTTYPE> > vec_col_ind;
	vec_col_ind.resize(n);
	vector< vector<FPTYPE> > vec_val;
	vec_val.resize(n);
	for (INTTYPE i = 0; i < nnz; i++)
	{
		vec_col_ind[row_ind[i]].push_back(col_ind[i]);
		vec_val[row_ind[i]].push_back(val[i]);
	}
	for (INTTYPE i = 0; i < n; i++)
	{
		for (INTTYPE j = 0; j < vec_col_ind[i].size(); j++)
		{
			row_ind[k] = i;
			col_ind[k] = vec_col_ind[i][j];
			val[k] = vec_val[i][j];
			k++;
		}
	}
}