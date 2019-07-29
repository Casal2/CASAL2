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
$begin num_limits.cpp$$
$spell
$$

$section Numeric Limits: Example and Test$$

$code
$verbatim%example/num_limits.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++

# ifdef _MSC_VER
// Supress Microsoft compiler warning about possible loss of precision,
// in the constructors (when converting to std::complex<float>)
//	Float one = 1
//	Float two = 2
// 1 and 2 are small enough so no loss of precision when converting to float.
# pragma warning(disable:4244)
# endif

# include <cppad/cppad.hpp>
# include <complex>

namespace {
	typedef CppAD::AD<double> Float;
	//
	// -----------------------------------------------------------------
	bool check_epsilon(void)
	{	bool ok    = true;
		Float eps   = CppAD::numeric_limits<Float>::epsilon();
		Float eps2  = eps / 2.0;
		Float check = 1.0 + eps;
		ok         &= 1.0 !=  check;
		check       = 1.0 + eps2;
		ok         &= 1.0 == check;
		return ok;
	}
	// -----------------------------------------------------------------
	bool check_min(void)
	{	bool ok     = true;
		Float min   = CppAD::numeric_limits<Float>::min();
		Float eps   = CppAD::numeric_limits<Float>::epsilon();
		//
		Float match = (min / 100.) * 100.;
		ok         &= abs(match / min - 1.0)  > 3.0 * eps;
		//
		match       = (min * 100.) / 100.;
		ok         &= abs(match / min - 1.0)  < 3.0 * eps;
		return ok;
	}
	// -----------------------------------------------------------------
	bool check_max(void)
	{	bool ok     = true;
		Float max   = CppAD::numeric_limits<Float>::max();
		Float eps   = CppAD::numeric_limits<Float>::epsilon();
		//
		Float match = (max * 100.) / 100.;
		ok         &= abs(match / max - 1.0) > 3.0 * eps;
		//
		match       = (max / 100.) * 100.;
		ok         &= abs(match / max - 1.0) < 3.0 * eps;
		return ok;
	}
	// -----------------------------------------------------------------
	bool check_nan(void)
	{	bool ok     = true;
		Float nan   = CppAD::numeric_limits<Float>::quiet_NaN();
		ok         &= nan != nan;
		return ok;
	}
}

bool num_limits(void)
{	bool ok = true;

	ok &= check_epsilon();
	ok &= check_min();
	ok &= check_max();
	ok &= check_nan();

	return ok;
}
// END C++
