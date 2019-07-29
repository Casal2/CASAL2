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
$begin div.cpp$$

$section AD Binary Division: Example and Test$$
$mindex / divide quotient$$


$code
$verbatim%example/div.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>

bool Div(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;


	// domain space vector
	size_t n  = 1;
	double x0 = 0.5;
	CPPAD_TESTVECTOR(AD<double>) x(n);
	x[0]      = x0;

	// declare independent variables and start tape recording
	CppAD::Independent(x);

	// some binary division operations
	AD<double> a = x[0] / 1.; // AD<double> / double
	AD<double> b = a  / 2;    // AD<double> / int
	AD<double> c = 3. / b;    // double     / AD<double>
	AD<double> d = 4  / c;    // int        / AD<double>

	// range space vector
	size_t m = 1;
	CPPAD_TESTVECTOR(AD<double>) y(m);
	y[0] = (x[0] * x[0]) / d;   // AD<double> / AD<double>

	// create f: x -> y and stop tape recording
	CppAD::ADFun<double> f(x, y);

	// check value
	ok &= NearEqual(y[0], x0*x0*3.*2.*1./(4.*x0),  1e-10 , 1e-10);

	// forward computation of partials w.r.t. x[0]
	CPPAD_TESTVECTOR(double) dx(n);
	CPPAD_TESTVECTOR(double) dy(m);
	dx[0] = 1.;
	dy    = f.Forward(1, dx);
	ok   &= NearEqual(dy[0], 3.*2.*1./4., 1e-10, 1e-10);

	// reverse computation of derivative of y[0]
	CPPAD_TESTVECTOR(double)  w(m);
	CPPAD_TESTVECTOR(double) dw(n);
	w[0]  = 1.;
	dw    = f.Reverse(1, w);
	ok   &= NearEqual(dw[0], 3.*2.*1./4., 1e-10, 1e-10);

	// use a VecAD<Base>::reference object with division
	CppAD::VecAD<double> v(1);
	AD<double> zero(0);
	v[zero] = d;
	AD<double> result = (x[0] * x[0]) / v[zero];
	ok     &= (result == y[0]);

	return ok;
}

// END C++
