#include "MatrixEllpack.h"

MatrixEllpack::MatrixEllpack(INTTYPE _nnz_max, INTTYPE _n) //конструктор инициализатор
{
	n = _n;
	nnz_max = _nnz_max;
	val_data = new FPTYPE [n*nnz_max];
	val = new FPTYPE*[n];
	col_ind_data = new INTTYPE[n*nnz_max];
	col_ind = new INTTYPE*[n];
	for (INTTYPE i = 0; i < n; i++)
	{
		val[i] = val_data + i*nnz_max;
		col_ind[i] = col_ind_data + i*nnz_max;
	}
}

MatrixEllpack::MatrixEllpack(const MatrixEllpack& Matrix) //конструктор копирования
{
	n = Matrix.n;
	nnz_max = Matrix.nnz_max;
	val_data=new FPTYPE[n*nnz_max];
	col_ind_data = new INTTYPE[n*nnz_max];
	memcpy(val_data, Matrix.val_data, sizeof(FPTYPE)*n*nnz_max);
	memcpy(col_ind_data, Matrix.col_ind_data, sizeof(INTTYPE)*n*nnz_max);
	val= new FPTYPE*[n];
	col_ind = new INTTYPE*[n];
	memcpy(val, Matrix.val, sizeof(FPTYPE*)*n);
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE*)*n);
}

MatrixEllpack :: ~MatrixEllpack() //деструктор
{
	if ((nnz_max != 0) && (n != 0))
	{
		delete[] col_ind_data;
		delete[] val_data;
	}
	if (n != 0)
	{
		delete[] col_ind;
		delete[] val;
	}
}

MatrixEllpack& MatrixEllpack:: operator=(const MatrixEllpack &Matrix)    // присваивание
{
	if (this == &Matrix)
		return *this;
	DeleteAndAllocateMemory(Matrix.n, Matrix.nnz_max);
	memcpy(val_data, Matrix.val_data, sizeof(FPTYPE)*n*nnz_max);
	memcpy(col_ind_data, Matrix.col_ind_data, sizeof(INTTYPE)*n*nnz_max);
	memcpy(val, Matrix.val, sizeof(FPTYPE*)*n);
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE*)*n);
	return *this;
}

void MatrixEllpack::ReadFromBinaryFile(char *filename)  // чтение из бинарного файла
{
	FILE *ELLmtx = NULL;
	INTTYPE _n, _nnz_max;
	ELLmtx = fopen(filename, "rb");
	if (ELLmtx == NULL)
	{
		printf("Error opening file");
	}
	fread(&_n, sizeof(INTTYPE), 1, ELLmtx);
	fread(&_nnz_max, sizeof(INTTYPE), 1, ELLmtx);
	DeleteAndAllocateMemory(_n, _nnz_max);
	fread(val_data, sizeof(FPTYPE), n*nnz_max, ELLmtx);
	fread(val, sizeof(FPTYPE*), n, ELLmtx);
	fread(col_ind_data, sizeof(INTTYPE), n*nnz_max, ELLmtx);
	fread(col_ind, sizeof(INTTYPE*), n, ELLmtx);
	fclose(ELLmtx);
}

void MatrixEllpack::WriteInBinaryFile(char *filename)  //запись в бинарный файл
{
	FILE *ELLmtx = NULL;
	ELLmtx = fopen(filename, "wb");
	if (ELLmtx == NULL)
	{
		printf("Error opening file");
	}
	fwrite(&n, sizeof(INTTYPE), 1, ELLmtx);
	fwrite(&nnz_max, sizeof(INTTYPE), 1, ELLmtx);
	fwrite(val_data, sizeof(FPTYPE), n*nnz_max, ELLmtx);
	fwrite(val, sizeof(FPTYPE*), n, ELLmtx);
	fwrite(col_ind_data, sizeof(INTTYPE), n*nnz_max, ELLmtx);
	fwrite(col_ind, sizeof(INTTYPE*), n, ELLmtx);
	fclose(ELLmtx);
}

void MatrixEllpack::Matrix_VectorMultiplicationInEllpack(FPTYPE* Vector, INTTYPE n_v, FPTYPE *result) //умножение матрицы на вектор
{
	FPTYPE tmp;
	for (INTTYPE i = 0; i < n; i++)
		result[i] = 0;
	if (n_v == n)
	{
		for (INTTYPE i = 0; i < n; i++)
		{
			tmp = 0;
			for (INTTYPE j = 0; j < nnz_max; j++)
			{
				if (col_ind[i][j] != -1)
					tmp = tmp + val[i][j] * Vector[col_ind[i][j]];
			}
			result[i] = tmp;
		}
	}
}

std::ostream & operator<<(ostream &out, const MatrixEllpack &Matrix)
{
	out << "Val : " << endl;
	for (INTTYPE i = 0; i < Matrix.n; i++)
	{
		for (INTTYPE j = 0; j < Matrix.nnz_max; j++)
			out << Matrix.val[i][j] << "  ";
		out << endl;
	}
	out << "col_ind : " << endl;
	for (INTTYPE i = 0; i < Matrix.n; i++)
	{
		for (INTTYPE j = 0; j < Matrix.nnz_max; j++)
			out << Matrix.col_ind[i][j] << "  ";
		out << endl;
	}
	return out;
}
void MatrixEllpack::DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _nnz_max)
{
	if ((n != _n) || (nnz_max != _nnz_max))
	{ 
		if ((nnz_max != 0) && (n != 0))
		{
			delete[] col_ind_data;
			delete[] val_data;
		}
		if (n != 0)
		{
			delete[] col_ind;
			delete[] val;
		}

		val_data = new FPTYPE[_n*_nnz_max];
		val = new FPTYPE*[_n];
		col_ind_data = new INTTYPE[_n*_nnz_max];
		col_ind = new INTTYPE*[_n];
		for (INTTYPE i = 0; i < _n; i++)
		{
			val[i] = val_data + i*_nnz_max;
			col_ind[i] = col_ind_data + i*_nnz_max;
		}
		n = _n;
		nnz_max = _nnz_max;
	}
}