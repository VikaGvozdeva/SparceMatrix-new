#include "MatrixSellCSigma.h"

//конструктор инициализатор 
MatrixSellCSigma::MatrixSellCSigma(INTTYPE* _nnz_max, INTTYPE* _trpos, INTTYPE _n, INTTYPE _c)//блоки  строятся в соответсвии с перестановкой
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
	trpos = new INTTYPE[n];
	if (_trpos != NULL)
		memcpy(trpos, _trpos, sizeof(INTTYPE)*n);
}

MatrixSellCSigma::MatrixSellCSigma(const MatrixSellCSigma & Matrix) //конструктор копирования
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
	trpos = new INTTYPE[n];
	memcpy(trpos, Matrix.trpos, sizeof(INTTYPE)*n);
}

MatrixSellCSigma:: ~MatrixSellCSigma() //деструктор
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
	if (n != 0)
		delete[] trpos;
}

MatrixSellCSigma & MatrixSellCSigma:: operator=(const MatrixSellCSigma &Matrix)    // присваивание
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
	memcpy(trpos, Matrix.trpos, sizeof(INTTYPE)*n);
	return *this;
}

void MatrixSellCSigma::ReadFromBinaryFile(char *filename)  // чтение из бинарного файла
{
	INTTYPE _n, _c, _sum;
	FILE *SELL_Smtx = NULL;
	SELL_Smtx = fopen(filename, "rb");
	if (SELL_Smtx == NULL)
	{
		printf("Error opening file");
	}
	fread(&_n, sizeof(INTTYPE), 1, SELL_Smtx);
	fread(&_c, sizeof(INTTYPE), 1, SELL_Smtx);
	fread(&_sum, sizeof(INTTYPE), 1, SELL_Smtx);
	DeleteAndAllocateMemory(_n, _c, _sum);
	if (c != 0)
	{
		fread(nnz_max, sizeof(INTTYPE), n / c + 1, SELL_Smtx);
		fread(sliceptr, sizeof(INTTYPE), n / c + 2, SELL_Smtx);
	}
	fread(val, sizeof(FPTYPE), sum, SELL_Smtx);
	fread(col_ind, sizeof(INTTYPE), sum, SELL_Smtx);
	fread(trpos, sizeof(INTTYPE), n, SELL_Smtx);
	fclose(SELL_Smtx);
}

void MatrixSellCSigma::WriteInBinaryFile(char *filename)  //запись в бинарный файл
{
	FILE *SELL_Smtx = NULL;
	SELL_Smtx = fopen(filename, "wb");
	if (SELL_Smtx == NULL)
	{
		printf("Error opening file");
	}
	fwrite(&n, sizeof(INTTYPE), 1, SELL_Smtx);
	fwrite(&c, sizeof(INTTYPE), 1, SELL_Smtx);
	fwrite(&sum, sizeof(INTTYPE), 1, SELL_Smtx);
	if (c != 0)
	{
		fwrite(nnz_max, sizeof(INTTYPE), n / c + 1, SELL_Smtx);
		fwrite(sliceptr, sizeof(INTTYPE), n / c + 2, SELL_Smtx);
	}
	fwrite(val, sizeof(FPTYPE), sum, SELL_Smtx);
	fwrite(col_ind, sizeof(INTTYPE), sum, SELL_Smtx);
	fwrite(trpos, sizeof(INTTYPE), n, SELL_Smtx);
	fclose(SELL_Smtx);
}


void MatrixSellCSigma::Matrix_VectorMultiplicationInSELL_C_sigma(FPTYPE* Vector, INTTYPE n_v, FPTYPE *result)//умножение матрицы на вектор
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
				if (i == (n_block - 1) * c)
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
				result[trpos[i]] = tmp;
			}
			else
			{
				k = sliceptr[i / c] + i %c;
				while (k < sliceptr[(i / c) + 1])
				{
					if (col_ind[k] != -1)
					{
						tmp = tmp + val[k] * Vector[col_ind[k]];
					}
					k += n % c;
				}
				result[trpos[i]] = tmp;
			}
		}
	}
}

std::ostream & operator<<(ostream &out, const MatrixSellCSigma &Matrix)
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
INTTYPE MatrixSellCSigma::CalculateSum(INTTYPE *_nnz_max, INTTYPE _n, INTTYPE _c)
{
	INTTYPE tmp;
	INTTYPE _sum = 0;
	if ((_c != 0) && (_nnz_max != NULL))
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

void MatrixSellCSigma::DeleteAndAllocateMemory(INTTYPE _n, INTTYPE _c, INTTYPE _sum)
{
	if ((n != _n) || (c != _c))
	{
		if (c != 0)
		{
			delete[] sliceptr;
			delete[] nnz_max;
		}
		if (n != 0)
			delete[] trpos;
		if (_c != 0)
		{
			nnz_max = new INTTYPE[_n / _c + 1];
			sliceptr = new INTTYPE[_n / _c + 2];
		}
		trpos = new INTTYPE[_n];
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
void MatrixSellCSigma::TransposeRow(INTTYPE* nnz_row, INTTYPE n, INTTYPE* trpos) //перестановка строк в зависимости от числа ненулевых элементов
{
	INTTYPE* NNZ_row_copy = new INTTYPE[n];
	INTTYPE tmp1, tmp2;
	for (INTTYPE i = 0; i < n; i++)
	{
		NNZ_row_copy[i] = nnz_row[i];
	}
	for (INTTYPE i = 0; i < n - 1; i++)
		for (INTTYPE j = i + 1; j < n; j++)
		{
			if (NNZ_row_copy[i] < NNZ_row_copy[j])
			{
				tmp1 = NNZ_row_copy[i];
				NNZ_row_copy[i] = NNZ_row_copy[j];
				NNZ_row_copy[j] = tmp1;
				tmp2 = trpos[i];
				trpos[i] = trpos[j];
				trpos[j] = tmp2;
			}
		}
	delete[]NNZ_row_copy;
}