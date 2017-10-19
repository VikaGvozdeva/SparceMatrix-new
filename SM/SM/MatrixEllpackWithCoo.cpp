#include "MatrixEllpackWithCoo.h"


MatrixEllpackWithCoo:: MatrixEllpackWithCoo(INTTYPE _nnz_max, INTTYPE _n, INTTYPE _p2) //конструктор инициализатор
{
	n = _n;
	nnz_max = _nnz_max;
	p2 = _p2;
	val_data = new FPTYPE [n*nnz_max];
	val = new FPTYPE*[n];
	col_ind_data = new INTTYPE[n*nnz_max];
	col_ind = new INTTYPE*[n];
	for (INTTYPE i = 0; i < n; i++)
	{
		val[i] = val_data + i*nnz_max;
		col_ind[i] = col_ind_data + i*nnz_max;
	}
	val_c = new FPTYPE[p2*n];
	col_ind_c = new INTTYPE[p2 * n + 1]; // на один больше,чтобы записать -1 как флаг
	row_ind_c = new INTTYPE[p2 * n + 1]; // на один больше,чтобы записать -1 как флаг
}


MatrixEllpackWithCoo:: MatrixEllpackWithCoo(const MatrixEllpackWithCoo & Matrix) //конструктор копирования
{
	n = Matrix.n;
	nnz_max = Matrix.nnz_max;
	p2 = Matrix.p2;
	val_data=new FPTYPE[n*nnz_max];
	col_ind_data = new INTTYPE[n*nnz_max];
	memcpy(val_data, Matrix.val_data, sizeof(FPTYPE)*n*nnz_max);
	memcpy(col_ind_data, Matrix.col_ind_data, sizeof(INTTYPE)*n*nnz_max);
	val= new FPTYPE*[n];
	col_ind = new INTTYPE*[n];
	memcpy(val, Matrix.val, sizeof(FPTYPE*)*n);
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE*)*n);
	val_c = new FPTYPE[p2*n];
	memcpy(val_c, Matrix.val_c, sizeof(FPTYPE)*p2*n);
	col_ind_c = new INTTYPE[p2 * n + 1]; // на один больше,чтобы записать -1 как флаг
	memcpy(col_ind_c, Matrix.col_ind_c, sizeof(INTTYPE)*(p2*n + 1));
	row_ind_c = new INTTYPE[p2 * n + 1]; // на один больше,чтобы записать -1 как флаг
	memcpy(row_ind_c, Matrix.row_ind_c, sizeof(INTTYPE)*(p2*n + 1));
}

MatrixEllpackWithCoo:: ~MatrixEllpackWithCoo() //деструктор
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
	if (p2*n != 0)
	{
		delete[] val_c;
	}
	delete[] col_ind_c;
	delete[] row_ind_c;
}

MatrixEllpackWithCoo & MatrixEllpackWithCoo:: operator=(const MatrixEllpackWithCoo &Matrix)    // присваивание
{
	if (this == &Matrix)
		return *this;
	DeleteAndAllocateMemory(Matrix.n, Matrix.nnz_max, Matrix.p2);
	memcpy(val_data, Matrix.val_data, sizeof(FPTYPE)*n*nnz_max);
	memcpy(col_ind_data, Matrix.col_ind_data, sizeof(INTTYPE)*n*nnz_max);
	memcpy(val, Matrix.val, sizeof(FPTYPE*)*n);
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE*)*n);
	memcpy(val_c, Matrix.val_c, sizeof(FPTYPE)*p2*n);
	memcpy(col_ind_c, Matrix.col_ind_c, sizeof(INTTYPE)*(p2*n + 1));
	memcpy(row_ind_c, Matrix.row_ind_c, sizeof(INTTYPE)*(p2*n + 1));
	return *this;
}


void MatrixEllpackWithCoo:: ReadFromBinaryFile(char *filename)  // чтение из бинарного файла
{
	FILE *ELL_Cmtx = NULL;
	INTTYPE _n, _nnz_max , _p2;
	ELL_Cmtx = fopen(filename, "rb");
	if (ELL_Cmtx == NULL)
	{
		printf("Error opening file");
	}
	fread(&_n, sizeof(INTTYPE), 1, ELL_Cmtx);
	fread(&_nnz_max, sizeof(INTTYPE), 1, ELL_Cmtx);
	fread(&_p2, sizeof(INTTYPE), 1, ELL_Cmtx);
	DeleteAndAllocateMemory(_n, _nnz_max, _p2);
	fread(val_data, sizeof(FPTYPE), n*nnz_max, ELL_Cmtx);
	fread(val, sizeof(FPTYPE*), n, ELL_Cmtx);
	fread(col_ind_data, sizeof(INTTYPE), n*nnz_max, ELL_Cmtx);
	fread(col_ind, sizeof(INTTYPE*), n, ELL_Cmtx);
	fread(val_c, sizeof(FPTYPE), p2*n, ELL_Cmtx);
	fread(col_ind_c, sizeof(INTTYPE), p2*n + 1, ELL_Cmtx);
	fread(row_ind_c, sizeof(INTTYPE), p2*n + 1, ELL_Cmtx);
	fclose(ELL_Cmtx);
}

void MatrixEllpackWithCoo:: WriteInBinaryFile(char *filename)  //запись в бинарный файл
{
	FILE *ELL_Cmtx = NULL;
	ELL_Cmtx = fopen(filename, "wb");
	if (ELL_Cmtx == NULL)
	{
		printf("Error opening file");
	}
	fwrite(&n, sizeof(INTTYPE), 1, ELL_Cmtx);
	fwrite(&nnz_max, sizeof(INTTYPE), 1, ELL_Cmtx);
	fwrite(&p2, sizeof(INTTYPE), 1, ELL_Cmtx);
	fwrite(val_data, sizeof(FPTYPE), n*nnz_max, ELL_Cmtx);
	fwrite(val, sizeof(FPTYPE*), n, ELL_Cmtx);
	fwrite(col_ind_data, sizeof(INTTYPE), n*nnz_max, ELL_Cmtx);
	fwrite(col_ind, sizeof(INTTYPE*), n, ELL_Cmtx);
	fwrite(val_c, sizeof(FPTYPE), p2*n, ELL_Cmtx);
	fwrite(col_ind_c, sizeof(INTTYPE), p2*n + 1, ELL_Cmtx);
	fwrite(row_ind_c, sizeof(INTTYPE), p2*n + 1, ELL_Cmtx);
	fclose(ELL_Cmtx);
}

void MatrixEllpackWithCoo:: Matrix_VectorMultiplicationInELLPACKwithCOO( FPTYPE* Vector, INTTYPE n_v, FPTYPE *result)//умножение матрицы на вектор
{
	INTTYPE k = 0;
	FPTYPE tmp;
	for (INTTYPE i = 0; i < n_v; i++)
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
			while ((k<p2*n) && row_ind_c[k] == i)
			{
				tmp = tmp + val_c[k] * Vector[col_ind_c[k]];
				k++;
			}
			result[i] = tmp;
		}
	}
}

std:: ostream & operator<<(ostream &out, const MatrixEllpackWithCoo &Matrix)
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
	INTTYPE i = 0;
	while ((Matrix.row_ind_c[i] != -1) && (i != Matrix.p2 * Matrix.n))
	{
		out << "(" << Matrix.val_c[i] << "," << Matrix.row_ind_c[i] << "," << Matrix.col_ind_c[i] << ") ,";
		i++;
	}
	out << endl;
	return out;
}

void MatrixEllpackWithCoo:: DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _nnz_max, INTTYPE _p2)
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
	
	if (p2 != _p2)
	{
		if(p2*n!=0)
			delete[]val_c;
		delete[] col_ind_c;
		delete[] row_ind_c;
		val_c = new FPTYPE [_p2*n];
		col_ind_c = new INTTYPE[_p2 * n + 1];
		row_ind_c = new INTTYPE[_p2 * n + 1];
		p2 = _p2;
	}
}