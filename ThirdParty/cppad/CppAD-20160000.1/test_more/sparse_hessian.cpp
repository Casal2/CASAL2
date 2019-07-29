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
Old SparseHessian example
*/

# include <cppad/cppad.hpp>
namespace { // ---------------------------------------------------------

bool rc_tridiagonal(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j, k, ell;
	double eps = 10. * CppAD::epsilon<double>();

	size_t n = 12; // must be greater than or equal 3; see n_sweep.
	size_t m = n - 1;
	CPPAD_TESTVECTOR(AD<double>) a_x(n), a_y(m);
	CPPAD_TESTVECTOR(double) x(n), check(n * n), w(m);
	for(j = 0; j < n; j++)
		a_x[j] = x[j] = double(j+1);

	// declare independent variables and start taping
	CppAD::Independent(a_x);

	for(ell = 0; ell < n * n; ell++)
		check[ell] = 0.0;

	for(i = 0; i < m; i++)
	{	AD<double> diff = a_x[i+1] - a_x[i];
		a_y[i] = 0.5 * diff * diff / double(i+2);
		w[i] = double(i+1);
		ell         = i * n + i;
		check[ell] += w[i] / double(i+2);
		ell         = (i+1) * n + i+1;
		check[ell] += w[i] / double(i+2);
		ell         = (i+1) * n + i;
		check[ell] -= w[i] / double(i+2);
		ell         = i * n + i+1;
		check[ell] -= w[i] / double(i+2);
	}

	// create f: x -> y
	CppAD::ADFun<double> f(a_x, a_y);

	// determine the sparsity pattern p for Hessian of w^T f
	typedef CppAD::vector< std::set<size_t> > VectorSet;
	VectorSet p_r(n);
	for(j = 0; j < n; j++)
		p_r[j].insert(j);
	f.ForSparseJac(n, p_r);
	//
	VectorSet p_s(1);
	for(i = 0; i < m; i++)
		if( w[i] != 0 )
			p_s[0].insert(i);
	VectorSet p_h = f.RevSparseHes(n, p_s);

	// requres the upper triangle of the Hessian
	size_t K = 2 * n - 1;
	CPPAD_TESTVECTOR(size_t) r(K), c(K);
	CPPAD_TESTVECTOR(double) hes(K);
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
	ok &= k == K;

	// test computing sparse Hessian
	CppAD::sparse_hessian_work work;
	size_t n_sweep = f.SparseHessian(x, w, p_h, r, c, hes, work);
	ok &= n_sweep == 3;
	for(k = 0; k < K; k++)
	{	ell = r[k] * n + c[k];
		ok &=  NearEqual(check[ell], hes[k], eps, eps);
		ell = c[k] * n + r[k];
		ok &=  NearEqual(check[ell], hes[k], eps, eps);
	}

	return ok;
}


template <class VectorBase, class VectorBool>
bool bool_case()
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j, k;

	// domain space vector
	size_t n = 3;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(i = 0; i < n; i++)
		X[i] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 1;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = X[0] * X[0] + X[0] * X[1] + X[1] * X[1] + X[2] * X[2];

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(i = 0; i < n; i++)
		x[i] = double(i);

	// second derivative of y[1]
	VectorBase w(m);
	w[0] = 1.;
	VectorBase h( n * n );
	h = f.SparseHessian(x, w);
	/*
	    [ 2 1 0 ]
	h = [ 1 2 0 ]
            [ 0 0 2 ]
	*/
	VectorBase check(n * n);
	check[0] = 2.; check[1] = 1.; check[2] = 0.;
	check[3] = 1.; check[4] = 2.; check[5] = 0.;
	check[6] = 0.; check[7] = 0.; check[8] = 2.;
	for(k = 0; k < n * n; k++)
		ok &=  NearEqual(check[k], h[k], 1e-10, 1e-10 );

	// determine the sparsity pattern p for Hessian of w^T F
	VectorBool r(n * n);
	for(j = 0; j < n; j++)
	{	for(k = 0; k < n; k++)
		r[j * n + k] = false;
		r[j * n + j] = true;
	}
	f.ForSparseJac(n, r);
	//
	VectorBool s(m);
	for(i = 0; i < m; i++)
		s[i] = w[i] != 0;
	VectorBool p = f.RevSparseHes(n, s);

	// test passing sparsity pattern
	h = f.SparseHessian(x, w, p);
	for(k = 0; k < n * n; k++)
		ok &=  NearEqual(check[k], h[k], 1e-10, 1e-10 );

	return ok;
}
template <class VectorBase, class VectorSet>
bool set_case()
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j, k;

	// domain space vector
	size_t n = 3;
	CPPAD_TESTVECTOR(AD<double>)  X(n);
	for(i = 0; i < n; i++)
		X[i] = AD<double> (0);

	// declare independent variables and starting recording
	CppAD::Independent(X);

	size_t m = 1;
	CPPAD_TESTVECTOR(AD<double>)  Y(m);
	Y[0] = X[0] * X[0] + X[0] * X[1] + X[1] * X[1] + X[2] * X[2];

	// create f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// new value for the independent variable vector
	VectorBase x(n);
	for(i = 0; i < n; i++)
		x[i] = double(i);

	// second derivative of y[1]
	VectorBase w(m);
	w[0] = 1.;
	VectorBase h( n * n );
	h = f.SparseHessian(x, w);
	/*
	    [ 2 1 0 ]
	h = [ 1 2 0 ]
            [ 0 0 2 ]
	*/
	VectorBase check(n * n);
	check[0] = 2.; check[1] = 1.; check[2] = 0.;
	check[3] = 1.; check[4] = 2.; check[5] = 0.;
	check[6] = 0.; check[7] = 0.; check[8] = 2.;
	for(k = 0; k < n * n; k++)
		ok &=  NearEqual(check[k], h[k], 1e-10, 1e-10 );

	// determine the sparsity pattern p for Hessian of w^T F
	VectorSet r(n);
	for(j = 0; j < n; j++)
		r[j].insert(j);
	f.ForSparseJac(n, r);
	//
	VectorSet s(1);
	for(i = 0; i < m; i++)
		if( w[i] != 0 )
			s[0].insert(i);
	VectorSet p = f.RevSparseHes(n, s);

	// test passing sparsity pattern
	h = f.SparseHessian(x, w, p);
	for(k = 0; k < n * n; k++)
		ok &=  NearEqual(check[k], h[k], 1e-10, 1e-10 );

	return ok;
}
} // End empty namespace
# include <vector>
# include <valarray>
bool sparse_hessian(void)
{	bool ok = true;

	ok &= rc_tridiagonal();
	// ---------------------------------------------------------------
	// vector of bool cases
	ok &= bool_case< CppAD::vector  <double>, CppAD::vectorBool   >();
	ok &= bool_case< std::vector    <double>, CppAD::vector<bool> >();
	ok &= bool_case< std::valarray  <double>, std::vector<bool>   >();
	// ---------------------------------------------------------------
	// vector of set cases
	typedef std::vector< std::set<size_t> >   std_vector_set;
	typedef CppAD::vector< std::set<size_t> > cppad_vector_set;
	//
	ok &= set_case< CppAD::vector<double>, std_vector_set   >();
	ok &= set_case< std::valarray<double>, std_vector_set   >();
	ok &= set_case< std::vector<double>,   cppad_vector_set >();
	ok &= set_case< CppAD::vector<double>, cppad_vector_set >();
	//
	// According to section 26.3.2.3 of the 1998 C++ standard
	// a const valarray does not return references to its elements.
	// so do not include it in the testing for sets.
	// ---------------------------------------------------------------
	return ok;
}
