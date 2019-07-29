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
$begin fun_assign.cpp$$

$section ADFun Assignment: Example and Test$$
$mindex assignment$$


$code
$verbatim%example/fun_assign.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <limits>

bool fun_assign(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	size_t i, j;

	// ten times machine percision
	double eps = 10. * CppAD::numeric_limits<double>::epsilon();

	// two ADFun<double> objects
	CppAD::ADFun<double> g;

	// domain space vector
	size_t n  = 3;
	CPPAD_TESTVECTOR(AD<double>) x(n);
	for(j = 0; j < n; j++)
		x[j] = AD<double>(j + 2);

	// declare independent variables and start tape recording
	CppAD::Independent(x);

	// range space vector
	size_t m = 2;
	CPPAD_TESTVECTOR(AD<double>) y(m);
	y[0] = x[0] + x[0] * x[1];
	y[1] = x[1] * x[2] + x[2];

	// Store operation sequence, and order zero forward results, in f.
	CppAD::ADFun<double> f(x, y);

	// sparsity pattern for the identity matrix
	CPPAD_TESTVECTOR(std::set<size_t>) r(n);
	for(j = 0; j < n; j++)
		r[j].insert(j);

	// Store forward mode sparsity pattern in f
	f.ForSparseJac(n, r);

	// make a copy in g
	g = f;

	// check values that should be equal
	ok &= ( g.size_order()       == f.size_order() );
	ok &= ( g.size_forward_bool() == f.size_forward_bool() );
	ok &= ( g.size_forward_set()  == f.size_forward_set() );

	// Use zero order Taylor coefficient from f for first order
	// calculation using g.
	CPPAD_TESTVECTOR(double) dx(n), dy(m);
	for(i = 0; i < n; i++)
		dx[i] = 0.;
	dx[1] = 1;
	dy    = g.Forward(1, dx);
	ok &= NearEqual(dy[0], x[0], eps, eps); // partial y[0] w.r.t x[1]
	ok &= NearEqual(dy[1], x[2], eps, eps); // partial y[1] w.r.t x[1]

	// Use forward Jacobian sparsity pattern from f to calculate
	// Hessian sparsity pattern using g.
	CPPAD_TESTVECTOR(std::set<size_t>) s(1), h(n);
	s[0].insert(0); // Compute sparsity pattern for Hessian of y[0]
	h =  f.RevSparseHes(n, s);

	// check sparsity pattern for Hessian of y[0] = x[0] + x[0] * x[1]
	ok  &= ( h[0].find(0) == h[0].end() ); // zero     w.r.t x[0], x[0]
	ok  &= ( h[0].find(1) != h[0].end() ); // non-zero w.r.t x[0], x[1]
	ok  &= ( h[0].find(2) == h[0].end() ); // zero     w.r.t x[0], x[2]

	ok  &= ( h[1].find(0) != h[1].end() ); // non-zero w.r.t x[1], x[0]
	ok  &= ( h[1].find(1) == h[1].end() ); // zero     w.r.t x[1], x[1]
	ok  &= ( h[1].find(2) == h[1].end() ); // zero     w.r.t x[1], x[2]

	ok  &= ( h[2].find(0) == h[2].end() ); // zero     w.r.t x[2], x[0]
	ok  &= ( h[2].find(1) == h[2].end() ); // zero     w.r.t x[2], x[1]
	ok  &= ( h[2].find(2) == h[2].end() ); // zero     w.r.t x[2], x[2]

	return ok;
}

// END C++
