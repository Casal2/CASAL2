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
$begin fadbad_mat_mul.cpp$$
$spell
	badiff
	sq
	onetape
	typedef
	diff
	Fadbad
	Adolc
	cppad.hpp
	bool
	mul
	dz
	CppAD
$$

$section Fadbad Speed: Matrix Multiplication$$
$mindex link_mat_mul speed multiply$$


$head Specifications$$
See $cref link_mat_mul$$.

$head Implementation$$

$codep */
# include <FADBAD++/badiff.h>
# include <cppad/speed/mat_sum_sq.hpp>
# include <cppad/speed/uniform_01.hpp>
# include <cppad/utility/vector.hpp>

// list of possible options
extern bool global_memory, global_onetape, global_atomic, global_optimize;

bool link_mat_mul(
	size_t                           size     ,
	size_t                           repeat   ,
	CppAD::vector<double>&           x        ,
	CppAD::vector<double>&           z        ,
	CppAD::vector<double>&           dz       )
{
	// speed test global option values
	if( global_memory || global_onetape || global_atomic || global_optimize )
		return false;
	// The correctness check for this test is failing, so abort (for now).
	return false;

	// -----------------------------------------------------
	// setup

	// object for computing determinant
	typedef fadbad::B<double>       ADScalar;
	typedef CppAD::vector<ADScalar> ADVector;

	size_t j;                // temporary index
	size_t m = 1;            // number of dependent variables
	size_t n = size * size;  // number of independent variables
	ADVector   X(n);         // AD domain space vector
	ADVector   Y(n);         // Store product matrix
	ADVector   Z(m);         // AD range space vector

	// ------------------------------------------------------
	while(repeat--)
	{	// get the next matrix
		CppAD::uniform_01(n, x);

		// set independent variable values
		for(j = 0; j < n; j++)
			X[j] = x[j];

		// do the computation
		mat_sum_sq(size, X, Y, Z);

		// create function object f : X -> Z
		Z[0].diff(0, m);  // index 0 of m dependent variables

		// evaluate and return gradient using reverse mode
		for(j = 0; j < n; j++)
			dz[j] = X[j].d(0); // partial Z[0] w.r.t X[j]
	}
	// return function value
	z[0] = Z[0].x();

	// ---------------------------------------------------------
	return true;
}
/* $$
$end
*/
