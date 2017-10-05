#define _CRT_SECURE_NO_WARNINGS

#include "include\Converter.h"
void Converters::qs(INTTYPE* s_arr, INTTYPE* _s_arr, int first, int last)
{
	int i = first, j = last, x = s_arr[(first + last) / 2], tmp = 0;

	do {
		while (s_arr[i] > x) i++;
		while (s_arr[j] < x) j--;

		if (i <= j) {
			if (s_arr[i] < s_arr[j])
			{
				tmp = s_arr[i];
				s_arr[i] = s_arr[j];
				s_arr[j] = tmp;

				tmp = _s_arr[i];
				_s_arr[i] = _s_arr[j];
				_s_arr[j] = tmp;

			}
			i++;
			j--;
		}
	} while (i <= j);

	if (i < last)
		qs(s_arr, _s_arr, i, last);
	if (first < j)
		qs(s_arr, _s_arr, first, j);
}

	void Converters::COOToCRS(const COOMatrix &Mtx, CRSMatrix &Matrix)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0, n = 0, m = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		vector< vector<int> > col_ind_;
		col_ind_.resize(N);
		vector< vector<double> > val_;
		val_.resize(N);

		for (i = 0; i < NNZ; i++)
		{
			val_[Mtx.row_ind[i]].push_back(Mtx.val[i]);
			col_ind_[Mtx.row_ind[i]].push_back(Mtx.col_ind[i]);
			Matrix.row_ptr[Mtx.row_ind[i] + 1]++;
		}

		for (k = 2; k < N + 1; k++)
		{
			Matrix.row_ptr[k] += Matrix.row_ptr[k - 1];
		}

		for (m = 0; m < N; m++) {
			k = 0;
			for (n = Matrix.row_ptr[m]; n < Matrix.row_ptr[m + 1]; n++)
			{
				Matrix.val[n] = val_[m][k];
				Matrix.col_ind[n] = col_ind_[m][k];
				k++;
			}
		}
	}

	void Converters::COOToCCS(const COOMatrix &Mtx, CCSMatrix &Matrix)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;

		for (i = 0; i < NNZ; i++)
		{
			Matrix.val[i] = Mtx.val[i];
			Matrix.row_ind[i] = Mtx.row_ind[i];
		}

		for (j = 0; j < NNZ; j++)
		{
			tmp_ind = Mtx.col_ind[j];
			Matrix.col_ptr[++tmp_ind]++;
		}
		while (j < NNZ);

		for (k = 2; k < N + 1; k++)
		{
			Matrix.col_ptr[k] += Matrix.col_ptr[k - 1];
		}
	}

	void Converters::COOToCD(const COOMatrix &Mtx, CDMatrix &Matrix)
	{
		int i = 0, j = 0, n, k = 0, l = 0, NNZ = 0, N = 0, diag_ind = 0, B = 0, tmp_ind = 0, m = 0, diag_numb = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		diag_numb = Matrix.B;

		vector< vector<FPTYPE> > val_;
		val_.resize(diag_numb);

		vector< vector<INTTYPE> > col_ind_;
		col_ind_.resize(diag_numb);
		vector< vector<INTTYPE> > row_ind_;
		row_ind_.resize(diag_numb);

		for (int i = 0; i < diag_numb; i++)
		{
			val_[i].push_back(Matrix.diag[i]);
		}

		for (i = 0; i < NNZ; i++)
		{
			tmp_ind = Mtx.col_ind[i] - Mtx.row_ind[i];
			for (j = 0; j < diag_numb; j++)
			{
				if (tmp_ind == val_[j][0])
				{
					val_[j].push_back(Mtx.val[i]);
					if (tmp_ind >= 0) {
						//val_[j].push_back(Mtx.val[i]);
						//new
						col_ind_[j].push_back(Mtx.col_ind[i]);
					}
					else
					{
						row_ind_[j].push_back(Mtx.row_ind[i]);
					}
				}
			}
		}
		INTTYPE row_ind;
		for (m = 0; m < diag_numb; m++) {
			//for (n = diag_ptr[m]; n < diag_ptr[m + 1]; n++)
			for (n = 1; n < val_[m].size(); n++)
			{
				if (Matrix.diag[m] < 0)
				{
					row_ind = row_ind_[m][n - 1];
					Matrix.val[row_ind][m] = val_[m][n];

				}
				else
				{
					row_ind = col_ind_[m][n - 1];
					Matrix.val[row_ind][m] = val_[m][n];
				}
				//p++;
				//k++;
			}
		}

		//	delete[] diag_ptr;
	}

	void Converters::COOToJD(const COOMatrix &Mtx, JDMatrix &Matrix)
	{
		int i = 0, j = 0, k = 0, NNZ = 0, N = 0, tmp_ind = 0, maxval = 0;
		NNZ = Mtx.NNZ;
		N = Mtx.N;
		maxval = Matrix.MaxNNZ;
		vector< vector<INTTYPE> > col_ind_;
		col_ind_.resize(N);
		vector< vector<FPTYPE> > val_;
		val_.resize(N);
		INTTYPE *nnz_row = new INTTYPE[N];
		INTTYPE *nnz_col = new INTTYPE[N];
		INTTYPE *nnz_row_ind = new INTTYPE[N];

		for (i = 0; i < N; i++)
		{
			nnz_row[i] = Mtx.NNZ_row[i];
			nnz_row_ind[i] = i;
			nnz_col[i] = 0;
		}

		Converters::qs(nnz_row, nnz_row_ind, 0, N - 1);

		for (i = 0; i < N; i++)
		{
			Matrix.perm[i] = nnz_row_ind[i];
		}
		for (i = 0; i < NNZ; i++)
		{
			val_[Mtx.row_ind[i]].push_back(Mtx.val[i]);
			col_ind_[Mtx.row_ind[i]].push_back(Mtx.col_ind[i]);
		}
		for (int i = 0; i < N; i++)
		{
			int temp_nnz_col = val_[i].size();
			for (int k = 0; k < temp_nnz_col; k++)
			{
				nnz_col[k]++;
			}
		}

		int l = 0;
		int m = 0;
		for (i = 0; i < N; i++)
		{
			//m++;
			for (j = 0; j < nnz_col[i]; j++)
			{
				Matrix.jdiag[l] = val_[Matrix.perm[j]][i];
				Matrix.col_ind[l] = col_ind_[Matrix.perm[j]][i];
				l++;
			}
		}
		Matrix.jd_ptr[0] = 0;
		for (i = 1; i < N + 1; i++)
		{
			Matrix.jd_ptr[i] = nnz_col[i - 1];
		}

		for (i = 2; i < N + 1; i++)
		{
			Matrix.jd_ptr[i] += Matrix.jd_ptr[i - 1];
		}

		delete[] nnz_row;
		delete[] nnz_col;
		delete[] nnz_row_ind;

	}

	void Converters::COOToSL(const COOMatrix &Mtx, SLMatrix &Matrix)
	{
		int j = 0, k = 0, l = 0, tmp_ind = 0, m = 0, p = 0;
		int NNZ = Mtx.NNZ;
		int N = Mtx.N;
		vector< vector<FPTYPE> > vec;
		vec.resize(N - 1);

		INTTYPE* elem_before_diag = new INTTYPE[N - 1];
		for (int i = 0; i < N - 1; i++)
		{
			elem_before_diag[i] = 0;
		}
		int g = 0, h = 0, o = 0;

		j = 0; l = 0;
		int pos = 0;
		for (int i = 0; i < NNZ; i++)
		{
			if (Mtx.col_ind[i] < Mtx.row_ind[i])
			{
				vec[Mtx.row_ind[i] - 1].push_back(Mtx.val[i]);
				elem_before_diag[Mtx.row_ind[i] - 1]++;
				o++;
			}
			else
			{
				if (Mtx.col_ind[i] == Mtx.row_ind[i])
				{
					Matrix.adiag[Mtx.col_ind[i]] = Mtx.val[i];
					g++;
				}
				else
				{
					Matrix.autr[pos] = Mtx.val[i];
					Matrix.jptr[pos] = Mtx.row_ind[i];
					h++; pos++;
				}
			}
		}
		//printf("diag %d, upper %d, low %d", g, h, o);
		//system("pause");
		for (int i = 0; i < N - 1; i++)
		{
			if (vec[i].size() != 0)
			{
				for (int j = 0; j < vec.size(); j++)
				{
					Matrix.altr[m] = vec[i][j];
					m++;
				}
			}
		}

		for (p = 2; p < N + 1; p++)
		{
			Matrix.iptr[p] += Matrix.iptr[p - 1];
		}
		delete[] elem_before_diag;
	}
