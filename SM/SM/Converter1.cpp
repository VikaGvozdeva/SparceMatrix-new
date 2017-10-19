#include "CooMatrix.h"
#include "MatrixEllpack.h"
#include "MatrixEllpackWithCoo.h"
#include "MatrixSellC.h"
#include "MatrixSellCSigma.h"
#include "Converter1.h"
//
//INTTYPE SearchMax_int(INTTYPE* arr, INTTYPE n) // ïîèñê ìàêñèìóìà äëÿ INTTYPE
//{
//	INTTYPE max_arr = arr[0];
//	for (INTTYPE i = 1; i < n; i++)
//	{
//		if (max_arr < arr[i])
//			max_arr = arr[i];
//	}
//	return max_arr;
//}
//
//FPTYPE SearchMax_double(FPTYPE* arr, INTTYPE n) // ïîèñê ìàêñèìóìà äëÿ FPTYPE
//{
//	FPTYPE max_arr = arr[0];
//	for (INTTYPE i = 1; i < n; i++)
//	{
//		if (max_arr < arr[i])
//			max_arr = arr[i];
//	}
//	return max_arr;
//}
//
//FPTYPE CheckCorrectness(FPTYPE* my_mult, FPTYPE* mkl_mult, INTTYPE n) 
//{
//	FPTYPE res1;
//	FPTYPE res2;
//	FPTYPE* arr_abs;
//	FPTYPE* my_mult_abs;
//	arr_abs = (FPTYPE*)malloc(n * sizeof(FPTYPE));
//	my_mult_abs = (FPTYPE*)malloc(n * sizeof(FPTYPE));
//	for (INTTYPE i = 0; i < n; i++)
//	{
//		arr_abs[i] = abs(my_mult[i] - mkl_mult[i]);
//		my_mult_abs[i] = abs(my_mult[i]);
//	}
//	res1 = SearchMax_double(arr_abs, n);
//	res2 = SearchMax_double(my_mult_abs, n);
//	free(arr_abs);
//	free(my_mult_abs);
//	return res1 / res2;
//}
//
//void Converter::CooToEll(const CooMatrix& coo, MatrixEllpack& ell)
//{
//	INTTYPE j;
//	INTTYPE k = 0;
//	ell.DeleteAndAllocateMemory(coo.n, SearchMax_int(coo.nnz_row, coo.n));
//	for (INTTYPE i = 0; i < ell.n*ell.nnz_max; i++)
//	{
//		ell.val_data[i] = 0;
//		ell.col_ind_data[i] = -1;
//	}
//	j = 0;
//	for (INTTYPE i = 0; i < ell.n; i++)
//	{
//		while ((j<coo.nnz) && (coo.row_ind[j] == i))
//		{
//			ell.val[i][k] = coo.val[j];
//			ell.col_ind[i][k] = coo.col_ind[j];
//			k++;
//			j++;
//		}
//		k = 0;
//	}
//}
//
//void Converter:: CooToEllWithCoo(const CooMatrix& coo, MatrixEllpackWithCoo& ellc , INTTYPE p1) // p1-параметр,отвечающий за то,сколько нулей допускается хранить в формате ELLPACK ; p1<=n;
//{
//	INTTYPE j, l, tmp2, tmp3, tmp4;
//	INTTYPE k;
//	INTTYPE tmp1 = 0;//сколько нулей будет хранить, используя ELLPACK
//	INTTYPE nnz_max = SearchMax_int(coo.nnz_row, coo.n);
//	INTTYPE* nnz_dif;
//	nnz_dif = new INTTYPE[ coo.n ];
//	for (INTTYPE i = 0; i < coo.n; i++)
//	{
//		nnz_dif[i] = coo.nnz_row[i] - nnz_max;//в массиве лежат неположительные числа,поэтому на следю шаге вычитаем это значение
//		tmp1 = tmp1 - nnz_dif[i];
//	}
//	tmp2 = tmp1 / coo.n;
//	tmp3 = tmp1 % coo.n;
//	if (tmp3 > p1)
//	{
//		nnz_max = nnz_max - tmp2 - 1;
//		for (INTTYPE i = 0; i < coo.n; i++)
//			nnz_dif[i] += tmp2 + 1;
//		tmp4 = tmp2 + 1;
//	}
//	else
//	{
//		nnz_max = nnz_max - tmp2;
//		for (INTTYPE i = 0; i < coo.n; i++)
//			nnz_dif[i] += tmp2;
//		tmp4 = tmp2;
//	}
//	ellc.DeleteAndAllocateMemory(coo.n, nnz_max, tmp4);
//	for (INTTYPE i = 0; i < ellc.n*ellc.nnz_max; i++)
//	{
//		ellc.val_data[i] = 0;
//		ellc.col_ind_data[i] = -1;
//	}
//	j = 0;
//	k = 0;
//	for (INTTYPE i = 0; i < ellc.n; i++)
//	{
//		while ((j<coo.nnz) && (coo.row_ind[j] == i) && (k < nnz_max))
//		{
//			ellc.val[i][k] = coo.val[j];
//			ellc.col_ind[i][k] = coo.col_ind[j];
//			k++;
//			j++;
//		}
//		k = 0;
//		if (nnz_dif[i] > 0)
//			j = j + nnz_dif[i];
//	}
//	k = 0;
//	j = 0;
//	for (INTTYPE i = 0; i < coo.n; i++)
//	{
//		while ((j<coo.nnz) && (coo.row_ind[j] == i))
//			j++;
//		if (nnz_dif[i]>0)
//		{
//			for (l = j - nnz_dif[i]; l < j; l++)
//			{
//				ellc.row_ind_c[k] = coo.row_ind[l];
//				ellc.col_ind_c[k] = coo.col_ind[l];
//				ellc.val_c[k] = coo.val[l];
//				k++;
//			}
//		}
//	}
//	ellc.row_ind_c[k] = -1;
//	ellc.col_ind_c[k] = -1;
//	delete[] nnz_dif;
//}
//
//void Converter::CooToSellC(const CooMatrix& coo, MatrixSellC& sell, INTTYPE c)
//{
//	INTTYPE j, max_r, k, p, tmp1, tmp2, sum;
//	INTTYPE m = 0;
//	INTTYPE t = 0;
//	INTTYPE* nnz_max;
//	FPTYPE* val_data_e;
//	INTTYPE* col_ind_data_e;
//	FPTYPE** val_e;
//	INTTYPE** col_ind_e;
//	nnz_max = new INTTYPE[coo.n / c + 1];
//	for (INTTYPE i = 0; i < coo.n / c; i++)
//		nnz_max[i] = SearchMax_int(coo.nnz_row + i * c, c);
//	nnz_max[coo.n / c] = 0;
//	nnz_max[coo.n / c] = SearchMax_int(coo.nnz_row + (coo.n / c) * c, coo.n % c);
//	max_r = SearchMax_int(nnz_max, coo.n / c + 1);
//	val_data_e = new FPTYPE[c*max_r];
//	val_e = new FPTYPE*[c];
//	col_ind_data_e = new INTTYPE[c*max_r];
//	col_ind_e = new INTTYPE*[c];
//	for (INTTYPE i = 0; i < c; i++)
//	{
//		val_e[i] = val_data_e + i*max_r;
//		col_ind_e[i] = col_ind_data_e + i*max_r;
//	}
//	sum = sell.CalculateSum(nnz_max, coo.n, c);
//	sell.DeleteAndAllocateMemory(coo.n, c, sum);
//	memcpy(sell.nnz_max, nnz_max, sizeof(INTTYPE)*(coo.n / c + 1));
//	k = 0;
//	sell.sliceptr[0] = 0;
//	while (m != coo.n / c + 1)
//	{
//		p = 0;
//		if (m != coo.n / c)
//			tmp1 = (m + 1) * c;
//		else
//			tmp1 = m * c + coo.n % c;
//		for (INTTYPE i = 0; i < c*max_r; i++)
//		{
//			val_data_e[i] = 0;
//			col_ind_data_e[i] = -1;
//		}
//		for (INTTYPE i = m * c; i < tmp1; i++)
//		{
//			while ((t<coo.nnz) && coo.row_ind[t] == i)
//			{
//				val_e[i - m * c][p] = coo.val[t];
//				col_ind_e[i - m * c][p] = coo.col_ind[t];
//				p++;
//				t++;
//			}
//			p = 0;
//		}
//		if (m != coo.n / c)
//			tmp2 = c;
//		else
//			tmp2 = coo.n % c;
//		for (INTTYPE i = 0; i < nnz_max[m]; i++)
//			for (j = 0; j < tmp2; j++)
//			{
//				sell.val[k] = val_e[j][i];
//				sell.col_ind[k] = col_ind_e[j][i];
//				k++;
//			}
//		sell.sliceptr[m + 1] = sell.sliceptr[m] + nnz_max[m] * tmp2;
//		m++;
//	}
//	delete[]nnz_max;
//	delete[] col_ind_data_e;
//	delete[] val_data_e;
//	delete[] col_ind_e;
//	delete[] val_e;
//	
//}
//void Converter::CooToSellCSigma(const CooMatrix& coo, MatrixSellCSigma& sells, INTTYPE c, INTTYPE sigma)
//{
//	INTTYPE max_r, k, p, tmp1, tmp2, N_tr, sum;
//	INTTYPE m = 0;
//	INTTYPE t = 0;
//	INTTYPE* nnz_max;
//	FPTYPE** val_e;
//	FPTYPE** val_e_trpos = NULL;
//	INTTYPE** col_ind_e_trpos = NULL;
//	INTTYPE** col_ind_e;
//	INTTYPE* trpos = new INTTYPE[coo.n];
//	for (INTTYPE i = 0; i < coo.n; i++)
//		trpos[i] = i;
//	N_tr = coo.n / sigma;
//	for (INTTYPE i = 0; i < N_tr; i++)
//		MatrixSellCSigma::TransposeRow(coo.nnz_row + i * sigma, sigma, trpos + i * sigma);
//	if (coo.n % sigma != 0)
//		MatrixSellCSigma::TransposeRow(coo.nnz_row + N_tr * sigma, coo.n % sigma, coo.nnz_row + N_tr * sigma);
//	nnz_max = new INTTYPE[coo.n / c + 1];
//	INTTYPE* nnz_row_trpos = new INTTYPE[coo.n];
//	for (INTTYPE i = 0; i < coo.n; i++)
//		nnz_row_trpos[i] = coo.nnz_row[trpos[i]];
//	for (INTTYPE i = 0; i < coo.n / c; i++)
//		nnz_max[i] = SearchMax_int(nnz_row_trpos + i * c, c);
//	nnz_max[coo.n / c] = SearchMax_int(nnz_row_trpos + (coo.n / c) * c, coo.n % c);
//	max_r = SearchMax_int(nnz_max, coo.n / c + 1);
//	val_e = new FPTYPE*[c];
//	col_ind_e = new INTTYPE*[c];
//	for (INTTYPE i = 0; i < c; i++)
//	{
//		val_e[i] = new FPTYPE[max_r];
//		col_ind_e[i] = new INTTYPE[max_r];
//	}
//	sum = sells.CalculateSum(nnz_max, coo.n, c);
//	sells.DeleteAndAllocateMemory(coo.n, c, sum);
//	memcpy(sells.nnz_max, nnz_max, sizeof(INTTYPE)*(coo.n / c + 1));
//	memcpy(sells.trpos, trpos, sizeof(INTTYPE)*coo.n);
//	k = 0;
//	sells.sliceptr[0] = 0;
//	while (m != coo.n / c + 1)
//	{
//		p = 0;
//		if (m != coo.n / c)
//			tmp1 = (m + 1) * c;
//		else
//			tmp1 = m * c + coo.n % c;
//		for (INTTYPE i = 0; i < c; i++)
//			for (INTTYPE j = 0; j < max_r; j++)
//			{
//				val_e[i][j] = 0;
//				col_ind_e[i][j] = -1;
//			}
//		for (INTTYPE i = m * c; i < tmp1; i++)
//		{
//			while ((t<coo.nnz) && (coo.row_ind[t] == i))
//			{
//				val_e[i - m * c][p] = coo.val[t];
//				col_ind_e[i - m * c][p] = coo.col_ind[t];
//				p++;
//				t++;
//			}
//			p = 0;
//		}
//		val_e_trpos = new FPTYPE*[c];
//		col_ind_e_trpos = new INTTYPE*[c];
//		for (INTTYPE i = 0; i < c; i++)
//		{
//			val_e_trpos[i] = new FPTYPE[max_r];
//			col_ind_e_trpos[i] = new INTTYPE[max_r];
//		}
//		for (INTTYPE i = 0; i < c; i++)
//			for (INTTYPE j = 0; j < max_r; j++)
//			{
//				val_e_trpos[i][j] = 0;
//				col_ind_e_trpos[i][j] = -1;
//			}
//		for (INTTYPE i = 0; i < c; i++)
//			for (INTTYPE j = 0; j < max_r; j++)
//			{
//				if (i + c * m < coo.n)
//				{
//					val_e_trpos[i][j] = val_e[trpos[i + c * m] - c * m][j];
//					col_ind_e_trpos[i][j] = col_ind_e[trpos[i + c * m] - c * m][j];
//				}
//			}
//		if (m != coo.n / c)
//			tmp2 = c;
//		else
//			tmp2 = coo.n % c;
//		for (INTTYPE i = 0; i < nnz_max[m]; i++)
//			for (INTTYPE j = 0; j < tmp2; j++)
//			{
//				sells.val[k] = val_e_trpos[j][i];
//				sells.col_ind[k] = col_ind_e_trpos[j][i];
//				k++;
//			}
//		sells.sliceptr[m + 1] = sells.sliceptr[m] + nnz_max[m] * tmp2;
//		m++;
//		for (INTTYPE i = 0; i < c; i++)
//		{
//			delete[]val_e_trpos[i];
//			delete[]col_ind_e_trpos[i];
//		}
//		delete[]val_e_trpos;
//		delete[] col_ind_e_trpos;
//	}
//
//	delete[]nnz_max;
//	for (INTTYPE i = 0; i < c; i++)
//	{
//		delete[]val_e[i];
//		delete[]col_ind_e[i];
//	}
//	delete[]val_e;
//	delete[] col_ind_e;
//	delete[]trpos;
//	delete[] nnz_row_trpos;
//}