// $Id$
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
Old sparse Jacobian example
*/

# include <cppad/cppad.hpp>
namespace { // ---------------------------------------------------------

bool rc_tridiagonal(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j, k, ell;
	double eps = 10. * CppAD::epsilon<double>();

	// domain space vector
	size_t n = 13; // must be greater than or equal 3 (see n_sweep below)
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	CPPAD_TESTVECTOR(double)        x(n);
	for(j = 0; j < n; j++)
		X[j] = x[j] = double(j+1);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = n;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	CPPAD_TESTVECTOR(double) check(m * n );
	for(ell = 0; ell < m * n; ell++)
		check[ell] = 0.0;

	size_t K = 0;
	for(i = 0; i < n; i++)
	{	ell        = i * n + i;
		Y[i]       = (ell+1) * 0.5 * X[i] * X[i];
		check[ell] = (ell+1) * x[i];
		K++;
		if( i < n-1 )
		{	j          = i + 1;
			ell        = i * n + j;
			Y[i]      += (ell+1) * 0.5 * X[i+1] * X[i+1];
			check[ell] = (ell+1) * x[i+1];
			K++;
		}
		if(i > 0 )
		{	j          = i - 1;
			ell        = i * n + j;
			Y[i]      += (ell+1) * 0.5 * X[i-1] * X[i-1];
			check[ell] = (ell+1) * x[i-1];
		}
	}

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// sparsity pattern
	CppAD::vector< std::set<size_t> > s(m), p(m);
	for(i = 0; i < m; i++)
		s[i].insert(i);
	p   = f.RevSparseJac(m, s);

	// Request the upper triangle of the array
	CPPAD_TESTVECTOR(size_t) r(K), c(K);
	CPPAD_TESTVECTOR(double) jac(K);
	k = 0;
	for(i = 0; i < n; i++)
	{	r[k] = i;
		c[k] = i;
		k++;
		if( i < n-1 )
		{	r[k] = i;
			c[k] = i+1;
			k++;
		}
	}
	ok &= K == k;

	CppAD::sparse_jacobian_work work;
	size_t n_sweep = f.SparseJacobianForward(x, p, r, c, jac, work);
	ok &= n_sweep == 3;
	for(k = 0; k < K; k++)
	{	ell = r[k] * n + c[k];
		ok &=  NearEqual(check[ell], jac[k], eps, eps);
	}
	work.clear();
	n_sweep = f.SparseJacobianReverse(x, p, r, c, jac, work);
	ok &= n_sweep == 3;
	for(k = 0; k < K; k++)
	{	ell = r[k] * n + c[k];
		ok &=  NearEqual(check[ell], jac[k], eps, eps);
	}

	return ok;
}

template <class VectorBase, class VectorSet>
bool rc_set(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j, k, ell;
	double eps = 10. * CppAD::epsilon<double>();

	// domain space vector
	size_t n = 4;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(j = 0; j < n; j++)
		X[j] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 3;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = 1.0*X[0] + 2.0*X[1];
	Y[1] = 3.0*X[2] + 4.0*X[3];
	Y[2] = 5.0*X[0] + 6.0*X[1] + 7.0*X[3]*X[3]/2.;

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(j = 0; j < n; j++)
		x[j] = double(j);

	// Jacobian of y
	/*
	      [ 1 2 0 0    ]
	jac = [ 0 0 3 4    ]
	      [ 5 6 0 7*x_3]
	*/
	VectorBase check(m * n);
	check[0] = 1.; check[1] = 2.; check[2]  = 0.; check[3]  = 0.;
	check[4] = 0.; check[5] = 0.; check[6]  = 3.; check[7]  = 4.;
	check[8] = 5.; check[9] = 6.; check[10] = 0.; check[11] = 7.*x[3];

	// sparsity pattern
	VectorSet s(m), p(m);
	for(i = 0; i < m; i++)
		s[i].insert(i);
	p   = f.RevSparseJac(m, s);

	// Use forward mode to compute columns 0 and 2
	// (make sure order of rows and columns does not matter)
	CPPAD_TESTVECTOR(size_t) r(3), c(3);
	VectorBase jac(3);
	r[0] = 2; c[0] = 0;
	r[1] = 1; c[1] = 2;
	r[2] = 0; c[2] = 0;
	CppAD::sparse_jacobian_work work;
	size_t n_sweep = f.SparseJacobianForward(x, p, r, c, jac, work);
	for(k = 0; k < 3; k++)
	{	ell = r[k] * n + c[k];
		ok &=  NearEqual(check[ell], jac[k], eps, eps);
	}
	ok &= (n_sweep == 1);

	// Use reverse mode to compute rows 0 and 1
	// (make sure order of rows and columns does not matter)
	r.resize(4), c.resize(4); jac.resize(4);
	r[0] = 0; c[0] = 0;
	r[1] = 1; c[1] = 2;
	r[2] = 0; c[2] = 1;
	r[3] = 1; c[3] = 3;
	work.clear();
	n_sweep = f.SparseJacobianReverse(x, p, r, c, jac, work);
	for(k = 0; k < 4; k++)
	{	ell = r[k] * n + c[k];
		ok &=  NearEqual(check[ell], jac[k], eps, eps);
	}
	ok &= (n_sweep == 1);

	return ok;
}
template <class VectorBase, class VectorBool>
bool rc_bool(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t j, k, ell;
	double eps = 10. * CppAD::epsilon<double>();

	// domain space vector
	size_t n = 4;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(j = 0; j < n; j++)
		X[j] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 3;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = 1.0*X[0] + 2.0*X[1];
	Y[1] = 3.0*X[2] + 4.0*X[3];
	Y[2] = 5.0*X[0] + 6.0*X[1] + 7.0*X[3]*X[3]/2.;

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(j = 0; j < n; j++)
		x[j] = double(j);

	// Jacobian of y
	/*
	      [ 1 2 0 0    ]
	jac = [ 0 0 3 4    ]
	      [ 5 6 0 7*x_3]
	*/
	VectorBase check(m * n);
	check[0] = 1.; check[1] = 2.; check[2]  = 0.; check[3]  = 0.;
	check[4] = 0.; check[5] = 0.; check[6]  = 3.; check[7]  = 4.;
	check[8] = 5.; check[9] = 6.; check[10] = 0.; check[11] = 7.*x[3];
	VectorBool s(m * n);
	s[0] = true;   s[1] = true;   s[2] = false;   s[3] = false;
	s[4] = false;  s[5] = false;  s[6] = true;    s[7] = true;
	s[8] = true;   s[9] = true;  s[10] = false;  s[11] = true;

	// Use forward mode to compute columns 0 and 2
	// (make sure order of rows and columns does not matter)
	CPPAD_TESTVECTOR(size_t) r(3), c(3);
	VectorBase jac(3);
	r[0] = 2; c[0] = 0;
	r[1] = 1; c[1] = 2;
	r[2] = 0; c[2] = 0;
	CppAD::sparse_jacobian_work work;
	size_t n_sweep = f.SparseJacobianForward(x, s, r, c, jac, work);
	for(k = 0; k < 3; k++)
	{	ell = r[k] * n + c[k];
		ok &=  NearEqual(check[ell], jac[k], eps, eps);
	}
	ok &= (n_sweep == 1);

	// Use reverse mode to compute rows 0 and 1
	// (make sure order of rows and columns does not matter)
	r.resize(4), c.resize(4); jac.resize(4);
	r[0] = 0; c[0] = 0;
	r[1] = 1; c[1] = 2;
	r[2] = 0; c[2] = 1;
	r[3] = 1; c[3] = 3;
	work.clear();
	n_sweep = f.SparseJacobianReverse(x, s, r, c, jac, work);
	for(k = 0; k < 4; k++)
	{	ell = r[k] * n + c[k];
		ok &=  NearEqual(check[ell], jac[k], eps, eps);
	}
	ok &= (n_sweep == 1);

	return ok;
}


template <class VectorBase, class VectorBool>
bool reverse_bool(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j, k;

	// domain space vector
	size_t n = 4;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(j = 0; j < n; j++)
		X[j] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 3;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = 1.0*X[0] + 2.0*X[1];
	Y[1] = 3.0*X[2] + 4.0*X[3];
	Y[2] = 5.0*X[0] + 6.0*X[1] + 7.0*X[2] + 8.0*X[3]*X[3]/2.;

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(j = 0; j < n; j++)
		x[j] = double(j);

	// Jacobian of y without sparsity pattern
	VectorBase jac(m * n);
	jac = f.SparseJacobian(x);
	/*
	      [ 1 2 0 0    ]
	jac = [ 0 0 3 4    ]
	      [ 5 6 7 8*x_3]
	*/
	VectorBase check(m * n);
	check[0] = 1.; check[1] = 2.; check[2]  = 0.; check[3]  = 0.;
	check[4] = 0.; check[5] = 0.; check[6]  = 3.; check[7]  = 4.;
	check[8] = 5.; check[9] = 6.; check[10] = 7.; check[11] = 8.*x[3];
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	// test passing sparsity pattern
	VectorBool s(m * m);
	VectorBool p(m * n);
	for(i = 0; i < m; i++)
	{	for(k = 0; k < m; k++)
			s[i * m + k] = false;
		s[i * m + i] = true;
	}
	p   = f.RevSparseJac(m, s);
	jac = f.SparseJacobian(x, p);
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	return ok;
}

template <class VectorBase, class VectorSet>
bool reverse_set(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j, k;

	// domain space vector
	size_t n = 4;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(j = 0; j < n; j++)
		X[j] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 3;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = X[0] + X[1];
	Y[1] = X[2] + X[3];
	Y[2] = X[0] + X[1] + X[2] + X[3] * X[3] / 2.;

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(j = 0; j < n; j++)
		x[j] = double(j);

	// Jacobian of y without sparsity pattern
	VectorBase jac(m * n);
	jac = f.SparseJacobian(x);
	/*
	      [ 1 1 0 0  ]
	jac = [ 0 0 1 1  ]
	      [ 1 1 1 x_3]
	*/
	VectorBase check(m * n);
	check[0] = 1.; check[1] = 1.; check[2]  = 0.; check[3]  = 0.;
	check[4] = 0.; check[5] = 0.; check[6]  = 1.; check[7]  = 1.;
	check[8] = 1.; check[9] = 1.; check[10] = 1.; check[11] = x[3];
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	// test passing sparsity pattern
	VectorSet s(m), p(m);
	for(i = 0; i < m; i++)
		s[i].insert(i);
	p   = f.RevSparseJac(m, s);
	jac = f.SparseJacobian(x, p);
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	return ok;
}

template <class VectorBase, class VectorBool>
bool forward_bool(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t j, k;

	// domain space vector
	size_t n = 3;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(j = 0; j < n; j++)
		X[j] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 4;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = X[0] + X[2];
	Y[1] = X[0] + X[2];
	Y[2] = X[1] + X[2];
	Y[3] = X[1] + X[2] * X[2] / 2.;

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(j = 0; j < n; j++)
		x[j] = double(j);

	// Jacobian of y without sparsity pattern
	VectorBase jac(m * n);
	jac = f.SparseJacobian(x);
	/*
	      [ 1 0 1   ]
	jac = [ 1 0 1   ]
	      [ 0 1 1   ]
	      [ 0 1 x_2 ]
	*/
	VectorBase check(m * n);
	check[0] = 1.; check[1]  = 0.; check[2]  = 1.;
	check[3] = 1.; check[4]  = 0.; check[5]  = 1.;
	check[6] = 0.; check[7]  = 1.; check[8]  = 1.;
	check[9] = 0.; check[10] = 1.; check[11] = x[2];
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	// test passing sparsity pattern
	VectorBool r(n * n);
	VectorBool p(m * n);
	for(j = 0; j < n; j++)
	{	for(k = 0; k < n; k++)
			r[j * n + k] = false;
		r[j * n + j] = true;
	}
	p   = f.ForSparseJac(n, r);
	jac = f.SparseJacobian(x, p);
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	return ok;
}

template <class VectorBase, class VectorSet>
bool forward_set(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t j, k;

	// domain space vector
	size_t n = 3;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(j = 0; j < n; j++)
		X[j] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 4;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = X[0] + X[2];
	Y[1] = X[0] + X[2];
	Y[2] = X[1] + X[2];
	Y[3] = X[1] + X[2] * X[2] / 2.;

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(j = 0; j < n; j++)
		x[j] = double(j);

	// Jacobian of y without sparsity pattern
	VectorBase jac(m * n);
	jac = f.SparseJacobian(x);
	/*
	      [ 1 0 1   ]
	jac = [ 1 0 1   ]
	      [ 0 1 1   ]
	      [ 0 1 x_2 ]
	*/
	VectorBase check(m * n);
	check[0] = 1.; check[1]  = 0.; check[2]  = 1.;
	check[3] = 1.; check[4]  = 0.; check[5]  = 1.;
	check[6] = 0.; check[7]  = 1.; check[8]  = 1.;
	check[9] = 0.; check[10] = 1.; check[11] = x[2];
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	// test passing sparsity pattern
	VectorSet r(n), p(m);
	for(j = 0; j < n; j++)
		r[j].insert(j);
	p   = f.ForSparseJac(n, r);
	jac = f.SparseJacobian(x, p);
	for(k = 0; k < 12; k++)
		ok &=  NearEqual(check[k], jac[k], 1e-10, 1e-10 );

	return ok;
}

bool multiple_of_n_bit(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::vector;
	size_t i, j;

	// should be the same as the corresponding typedef in
	// cppad/local/sparse_pack.hpp
	typedef size_t Pack;

	// number of bits per packed value
	size_t n_bit = std::numeric_limits<Pack>::digits;

	// check case where number of variables is equal to n_bit
	vector< AD<double> > x(n_bit);
	vector< AD<double> > y(n_bit);

	// create an AD function with domain and range dimension equal to n_bit
	CppAD::Independent(x);
	for(i = 0; i < n_bit; i++)
		y[i] = x[n_bit - i - 1];
	CppAD::ADFun<double> f(x, y);

	// Jacobian sparsity patterns
	vector<bool> r(n_bit * n_bit);
	vector<bool> s(n_bit * n_bit);
	for(i = 0; i < n_bit; i++)
	{	for(j = 0; j < n_bit; j++)
			r[ i * n_bit + j ] = (i == j);
	}
	s = f.ForSparseJac(n_bit, r);

	// check the result
	for(i = 0; i < n_bit; i++)
	{	for(j = 0; j < n_bit; j++)
		{	if( i == n_bit - j - 1 )
				ok = ok & s[ i * n_bit + j ];
			else	ok = ok & (! s[i * n_bit + j] );
		}
	}

	return ok;
}
} // End empty namespace
# include <vector>
# include <valarray>
bool sparse_jacobian(void)
{	bool ok = true;
	ok &= rc_tridiagonal();
	ok &= multiple_of_n_bit();
	// ---------------------------------------------------------------
	// vector of bool cases
	ok &=      rc_bool< CppAD::vector<double>, CppAD::vectorBool   >();
	ok &= forward_bool< CppAD::vector<double>, CppAD::vector<bool> >();
	//
	ok &= reverse_bool< std::vector<double>,   std::vector<bool>   >();
	ok &=      rc_bool< std::vector<double>,   std::valarray<bool> >();
	//
	ok &= forward_bool< std::valarray<double>, CppAD::vectorBool   >();
	ok &= reverse_bool< std::valarray<double>, CppAD::vector<bool> >();
	// ---------------------------------------------------------------
	// vector of set cases
	typedef std::vector< std::set<size_t> >   std_vector_set;
	typedef CppAD::vector< std::set<size_t> > cppad_vector_set;
	//
	ok &=      rc_set< CppAD::vector<double>, std_vector_set   >();
	ok &= forward_set< std::valarray<double>, std_vector_set   >();
	//
	ok &= reverse_set< std::vector<double>,   cppad_vector_set >();
	ok &=      rc_set< CppAD::vector<double>, cppad_vector_set >();
	//
	// According to section 26.3.2.3 of the 1998 C++ standard
	// a const valarray does not return references to its elements.
	// typedef std::valarray< std::set<size_t> > std_valarray_set;
	// ok &= forward_set< std::valarray<double>, std_valarray_set >();
	// ok &= reverse_set< std::valarray<double>, std_valarray_set >();
	// ---------------------------------------------------------------
	//
	return ok;
}
