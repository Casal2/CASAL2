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
old romberg_one.cpp example / test
$spell
	Romberg
$$

$section One Dimensional Romberg Integration: Example and Test$$

$index Romberg, example$$
$index example, Romberg$$
$index test, Romberg$$

$code
old verbatim%example/romberg_one.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++

# include <cppad/cppad.hpp>

namespace {
	class Fun {
	private:
		const size_t degree;
	public:
		// constructor
		Fun(size_t degree_) : degree(degree_)
		{ }

		// function F(x) = x^degree
		template <class Float>
		Float operator () (const Float &x)
		{	size_t i;
			Float   f(1);
			for(i = 0; i < degree; i++)
				f *= x;
			return f;
		}
	};

	template <class Float>
	bool RombergOneCase(void)
	{	bool ok = true;
		size_t i;

		size_t degree = 4;
		Fun F(degree);

		// arguments to RombergOne
		Float a(0);
		Float b(1);
		Float r;
		size_t n = 4;
		Float e;
		size_t p;

		// int_a^b F(x) dx =
		//	[ b^(degree+1) - a^(degree+1) ] / (degree+1)
		Float bpow(1);
		Float apow(1);
		for(i = 0; i <= degree; i++)
		{	bpow *= b;
			apow *= a;
		}
		Float check = (bpow - apow) / Float(degree+1);

		// step size corresponding to r
		Float step = (b - a) / exp(log(Float(2.))*Float(n-1));
		// step size corresponding to error estimate
		step *= Float(2.);
		// step size raised to a power
		Float spow = Float(1);

		for(p = 0; p < n; p++)
		{	spow = spow * step * step;

			r = CppAD::RombergOne(F, a, b, n, p, e);

			ok  &= e < (degree+1) * spow;
			ok  &= CppAD::NearEqual(check, r, Float(0.), e);
		}

		return ok;
	}
}

bool RombergOne(void)
{	bool ok = true;
	using CppAD::AD;

	ok     &= RombergOneCase<double>();
	ok     &= RombergOneCase< AD<double> >();
	ok     &= RombergOneCase< AD< AD<double> > >();
	return ok;
}

// END C++
