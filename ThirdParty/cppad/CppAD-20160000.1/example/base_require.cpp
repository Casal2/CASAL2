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
$begin base_require.cpp$$
$spell
	alloc
$$

$section Using a User Defined AD Base Type: Example and Test$$
$mindex type$$


$code
$verbatim%example/base_require.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$head Purpose$$
The type $code base_alloc$$, defined in $cref base_alloc.hpp$$,
meets the requirements specified by $cref base_require$$
for $icode Base$$ in $codei%AD<%Base%>%$$.
The program below is an example use of $codei%AD<base_alloc>%$$.

$end
*/
// BEGIN C++
# include "base_alloc.hpp"
# include <cppad/cppad.hpp>

bool base_require(void)
{	bool ok = true;
	using CppAD::thread_alloc;
	typedef CppAD::AD<base_alloc> ad_base_alloc;

	// check the amount of memory inuse by this thread (thread zero)
	size_t thread = thread_alloc::thread_num();
	ok &= thread == 0;

	// y = x^2
	size_t n = 1, m = 1;
	CPPAD_TESTVECTOR(ad_base_alloc) a_x(n), a_y(m);
	a_x[0] = ad_base_alloc(1.);
	CppAD::Independent(a_x);
	a_y[0] = a_x[0] * a_x[0];
	CppAD::ADFun<base_alloc> f(a_x, a_y);

	// check function value f(x) = x^2
	CPPAD_TESTVECTOR(base_alloc) x(n), y(m);
	base_alloc eps =
		base_alloc(100.) * CppAD::numeric_limits<base_alloc>::epsilon();
	x[0] = base_alloc(3.);
	y    = f.Forward(0, x);
	ok  &= CppAD::NearEqual(y[0], x[0] * x[0], eps, eps);

	// check derivative value f'(x) = 2 * x
	CPPAD_TESTVECTOR(base_alloc) dy(m * n);
	dy   = f.Jacobian(x);
	ok  &= CppAD::NearEqual(dy[0], base_alloc(2.) * x[0], eps, eps);

	return ok;
}
// END C++
