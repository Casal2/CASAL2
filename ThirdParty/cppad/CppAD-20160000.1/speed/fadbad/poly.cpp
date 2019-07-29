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
$begin fadbad_poly.cpp$$
$spell
	onetape
	std
	cppad
	cpp
	tadiff
	std
	ddp
	Taylor
	dz
	eval
	cppad
	vector Vector
	typedef
	fadbad
	Lu
	CppAD
	det
	hpp
	const
	bool
$$

$section Fadbad Speed: Second Derivative of a Polynomial$$
$mindex link_poly speed$$


$head Specifications$$
See $cref link_poly$$.

$head Implementation$$

$codep */
# include <cppad/utility/vector.hpp>
# include <cppad/utility/poly.hpp>
# include <cppad/speed/uniform_01.hpp>
# include <FADBAD++/tadiff.h>

// list of possible options
extern bool global_memory, global_onetape, global_atomic, global_optimize;

bool link_poly(
	size_t                     size     ,
	size_t                     repeat   ,
	CppAD::vector<double>     &a        ,  // coefficients of polynomial
	CppAD::vector<double>     &z        ,  // polynomial argument value
	CppAD::vector<double>     &ddp      )  // second derivative w.r.t z
{
	if( global_atomic )
		return false;
	if( global_memory || global_onetape || global_optimize )
		return false;
	// -----------------------------------------------------
	// setup
	size_t i;             // temporary index
	fadbad::T<double>  Z; // domain space AD value
	fadbad::T<double>  P; // range space AD value

	// choose the polynomial coefficients
	CppAD::uniform_01(size, a);

	// AD copy of the polynomial coefficients
	CppAD::vector< fadbad::T<double> > A(size);
	for(i = 0; i < size; i++)
		A[i] = a[i];

	// ------------------------------------------------------
	while(repeat--)
	{	// get the next argument value
		CppAD::uniform_01(1, z);

		// independent variable value
		Z    = z[0]; // argument value
		Z[1] = 1;    // argument first order Taylor coefficient

		// AD computation of the dependent variable
		P = CppAD::Poly(0, A, Z);

		// Taylor-expand P to degree one
		P.eval(2);

		// second derivative is twice second order Taylor coefficient
		ddp[0] = 2. * P[2];

		// Free DAG corresponding to P does not seem to improve speed.
		// Probably because it gets freed the next time P is assigned.
		// P.reset();
	}
	// ------------------------------------------------------
	return true;
}
/* $$
$end
*/
