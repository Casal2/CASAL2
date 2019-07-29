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
$begin speed_test.cpp$$
$spell
	cppad.hpp
	Microsoft
	namespace
	std
	const
	cout
	ctime
	ifdef
	const
	endif
	cpp
$$


$section speed_test: Example and test$$
$mindex speed_test$$

$code
$verbatim%speed/example/speed_test.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/utility/speed_test.hpp>
# include <cppad/utility/vector.hpp>

namespace { // empty namespace
	using CppAD::vector;
	vector<double> a, b, c;
	void test(size_t size, size_t repeat)
	{	// setup
		a.resize(size);
		b.resize(size);
		c.resize(size);
		size_t i  = size;;
		while(i)
		{	--i;
			a[i] = i;
			b[i] = 2 * i;
			c[i] = 0.0;
		}
		// operations we are timing
		while(repeat--)
		{	i = size;;
			while(i)
			{	--i;
				c[i] += std::sqrt(a[i] * a[i] + b[i] * b[i]);
			}
		}
	}
}
bool speed_test(void)
{	bool ok = true;

	// size of the test cases
	vector<size_t> size_vec(2);
	size_vec[0] = 40;
	size_vec[1] = 80;

	// minimum amount of time to run test
	double time_min = 0.5;

	// run the test cases
	vector<size_t> rate_vec(2);
	rate_vec = CppAD::speed_test(test, size_vec, time_min);

	// time per repeat loop (note counting setup or teardown)
	double time_0 = 1. / double(rate_vec[0]);
	double time_1 = 1. / double(rate_vec[1]);

	// for this case, time should be linear w.r.t size
	double check    = double(size_vec[1]) * time_0 / double(size_vec[0]);
	double rel_diff = (check - time_1) / time_1;
	ok             &= (std::fabs(rel_diff) <= .1);
	if( ! ok )
		std::cout << std::endl << "rel_diff = " << rel_diff << std::endl;

	a.clear();
	b.clear();
	c.clear();
	return ok;
}
// END C++
