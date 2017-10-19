#include "MatrixSellC.h"

//конструктор инициализатор  
MatrixSellC::MatrixSellC(INTTYPE* _nnz_max, INTTYPE _n, INTTYPE _c)  //nnz_max -массив максимальных размеров строк в каждом блоке
{
	n = _n;
	c = _c;
	if (c != 0)
	{
		nnz_max = new INTTYPE[n / c + 1];
		if (_nnz_max != NULL)
			memcpy(nnz_max, _nnz_max, sizeof(INTTYPE)*(n / c + 1));
		sum = CalculateSum(nnz_max, n, c);
		sliceptr = new INTTYPE[n / c + 2];
	}
	else
		sum = 0;
	val = new FPTYPE[sum];
	col_ind = new INTTYPE[sum];
}

MatrixSellC::MatrixSellC(const MatrixSellC & Matrix) //конструктор копирования
{
	n = Matrix.n;
	c = Matrix.c;
	sum = Matrix.sum;
	if (c != 0)
	{
		nnz_max = new INTTYPE[n / c + 1];
		memcpy(nnz_max, Matrix.nnz_max, sizeof(INTTYPE)*(n / c + 1));
		sliceptr = new INTTYPE[n / c + 2];
		memcpy(sliceptr, Matrix.sliceptr, sizeof(INTTYPE)*(n / c + 2));
	}
	val = new FPTYPE[sum];
	memcpy(val, Matrix.val, sizeof(FPTYPE)*sum);
	col_ind = new INTTYPE[sum];
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE)*sum);
}

MatrixSellC:: ~MatrixSellC() //деструктор
{
	if (sum != 0)
	{
		delete[] val;
		delete[] col_ind;
	}
	if (c != 0)
	{
		delete[] sliceptr;
		delete[] nnz_max;
	}
}

MatrixSellC & MatrixSellC:: operator=(const MatrixSellC &Matrix)    // присваивание
{
	if (this == &Matrix)
		return *this;
	DeleteAndAllocateMemory(Matrix.n, Matrix.c, Matrix.sum);
	if (c != 0)
	{
		memcpy(nnz_max, Matrix.nnz_max, sizeof(INTTYPE)*(n / c + 1));
		memcpy(sliceptr, Matrix.sliceptr, sizeof(INTTYPE)*(n / c + 2));
	}
	memcpy(val, Matrix.val, sizeof(FPTYPE)*sum);
	memcpy(col_ind, Matrix.col_ind, sizeof(INTTYPE)*sum);
	return *this;
}


void MatrixSellC::ReadFromBinaryFile(char *filename)  // чтение из бинарного файла
{
	INTTYPE _n, _c, _sum;
	FILE *SELLmtx = NULL;
	SELLmtx = fopen(filename, "rb");
	if (SELLmtx == NULL)
	{
		printf("Error opening file");
	}
	fread(&_n, sizeof(INTTYPE), 1, SELLmtx);
	fread(&_c, sizeof(INTTYPE), 1, SELLmtx);
	fread(&_sum, sizeof(INTTYPE), 1, SELLmtx);
	DeleteAndAllocateMemory(_n, _c, _sum);
	if (c != 0)
	{
		fread(nnz_max, sizeof(INTTYPE), n / c + 1, SELLmtx);
		fread(sliceptr, sizeof(INTTYPE), n / c + 2, SELLmtx);
	}
	fread(val, sizeof(FPTYPE), sum, SELLmtx);
	fread(col_ind, sizeof(INTTYPE), sum, SELLmtx);
	fclose(SELLmtx);
}

void MatrixSellC::WriteInBinaryFile(char *filename)  //запись в бинарный файл
{
	INTTYPE tmp;
	INTTYPE sum = 0;
	FILE *SELLmtx = NULL;
	SELLmtx = fopen(filename, "wb");
	if (SELLmtx == NULL)
	{
		printf("Error opening file");
	}
	fwrite(&n, sizeof(INTTYPE), 1, SELLmtx);
	fwrite(&c, sizeof(INTTYPE), 1, SELLmtx);
	fwrite(&sum, sizeof(INTTYPE), 1, SELLmtx);
	if (c != 0)
	{
		fwrite(nnz_max, sizeof(INTTYPE), n / c + 1, SELLmtx);
		fwrite(sliceptr, sizeof(INTTYPE), n / c + 2, SELLmtx);
	}
	fwrite(val, sizeof(FPTYPE), sum, SELLmtx);
	fwrite(col_ind, sizeof(INTTYPE), sum, SELLmtx);
	fclose(SELLmtx);
}

void MatrixSellC::Matrix_VectorMultiplicationInSELL_C(FPTYPE* Vector, INTTYPE n_v, FPTYPE *result) //умножение матрицы на вектор
{
	INTTYPE k = 0;
	INTTYPE m = 0;
	INTTYPE n_block;
	FPTYPE tmp;
	for (INTTYPE i = 0; i < n; i++)
		result[i] = 0;
	if (n_v == n)
	{
		n_block = n / c + 1;
		for (INTTYPE i = 0; i < n; i++)
		{
			tmp = 0;
			if (n_block != 0)
			{
				if (i < (n_block - 1) * c)
					if (i == (m + 1) * c)
						m++;
				if (i == (n_block - 1) *c)
					m++;
			}
			if (m != n_block - 1)
			{
				k = sliceptr[i / c] + i % c;
				while (k < sliceptr[(i / c) + 1])
				{
					if (col_ind[k] != -1)
					{
						tmp = tmp + val[k] * Vector[col_ind[k]];
					}
					k += c;
				}
				result[i] = tmp;
			}
			else
			{
				k = sliceptr[i / c] + i % c;
				while (k < sliceptr[(i / c) + 1])
				{
					if (col_ind[k] != -1)
					{
						tmp = tmp + val[k] * Vector[col_ind[k]];
					}
					k += n % c;
				}
				result[i] = tmp;
			}
		}
	}
}

std::ostream & operator<<(ostream &out, const MatrixSellC &Matrix)
{
	out << "Val : " << endl;
	for (INTTYPE i = 0; i < Matrix.sum; i++)
		out << Matrix.val[i] << "  ";
	out << endl;
	out << "col_ind : " << endl;
	for (INTTYPE i = 0; i < Matrix.sum; i++)
		out << Matrix.col_ind[i] << "  ";
	out << endl;
	out << "sliceptr : " << endl;
	if (Matrix.c != 0)
		for (INTTYPE i = 0; i < Matrix.n / Matrix.c + 2; i++)
			out << Matrix.sliceptr[i] << "  ";
	out << endl;
	return out;
}

INTTYPE MatrixSellC::CalculateSum(INTTYPE *_nnz_max, INTTYPE _n, INTTYPE _c)
{
	INTTYPE tmp;
	INTTYPE _sum = 0;
	if ((_c != 0) && _nnz_max != NULL)
	{
		for (INTTYPE i = 0; i < _n / _c; i++)
		{
			tmp = _c * _nnz_max[i];
			_sum += tmp;
		}
		_sum = _sum + _n % _c * _nnz_max[_n / _c]; // чтобы обработывать "хвост" в случае,когда размерность матрицы и размер блока не делятся нацело
	}
	return _sum;
}

void MatrixSellC::DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _c, INTTYPE _sum)
{
	if ((n != _n) || (c != _c))
	{
		if (c != 0)
		{
			delete[] sliceptr;
			delete[] nnz_max;
		}
		if (_c != 0)
		{
			nnz_max = new INTTYPE[_n / _c + 1];
			sliceptr = new INTTYPE[_n / _c + 2];
		}
		n = _n;
		c = _c;
	}
	if (sum != _sum)
	{
		if (sum != 0)
		{
			delete[] val;
			delete[] col_ind;
		}
		val = new FPTYPE[_sum];
		col_ind = new INTTYPE[_sum];
		sum = _sum;
	}
}