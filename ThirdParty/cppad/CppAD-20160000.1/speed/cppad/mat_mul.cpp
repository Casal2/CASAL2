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
$begin cppad_mat_mul.cpp$$
$spell
	resize
	nr
	nc
	cppad
	mul
	hpp
	bool
	onetape
	sq
	var
	std::cout
	endl
	CppAD
	dz
	typedef
	boolsparsity
	enum
$$

$section CppAD Speed, Matrix Multiplication$$
$mindex link_mat_mul multiply$$


$head Specifications$$
See $cref link_mat_mul$$.

$head Implementation$$

$codep */
# include <cppad/cppad.hpp>
# include <cppad/speed/mat_sum_sq.hpp>
# include <cppad/speed/uniform_01.hpp>
# include <cppad/example/matrix_mul.hpp>

// Note that CppAD uses global_memory at the main program level
extern bool
	global_onetape, global_atomic, global_optimize, global_boolsparsity;

bool link_mat_mul(
	size_t                           size     ,
	size_t                           repeat   ,
	CppAD::vector<double>&           x        ,
	CppAD::vector<double>&           z        ,
	CppAD::vector<double>&           dz
)
{
	// -----------------------------------------------------
	// setup
	typedef CppAD::AD<double>           ADScalar;
	typedef CppAD::vector<ADScalar>     ADVector;

	size_t j;               // temporary index
	size_t m = 1;           // number of dependent variables
	size_t n = size * size; // number of independent variables
	ADVector   X(n);        // AD domain space vector
	ADVector   Y(n);        // Store product matrix
	ADVector   Z(m);        // AD range space vector
	CppAD::ADFun<double> f; // AD function object

	// vectors of reverse mode weights
	CppAD::vector<double> w(1);
	w[0] = 1.;

	// user atomic information
	CppAD::vector<ADScalar> ax(2 * n), ay(n);
	size_t nr_result = size;
	size_t n_middle  = size;
	size_t nc_result = size;
	matrix_mul atom_mul(nr_result, n_middle, nc_result);
	//
	if( global_boolsparsity )
		atom_mul.option( CppAD::atomic_base<double>::pack_sparsity_enum );
	else
		atom_mul.option( CppAD::atomic_base<double>::set_sparsity_enum );
	// ------------------------------------------------------
	if( ! global_onetape ) while(repeat--)
	{	// get the next matrix
		CppAD::uniform_01(n, x);
		for( j = 0; j < n; j++)
			X[j] = x[j];

		// declare independent variables
		Independent(X);

		// do computations
		if( ! global_atomic )
			mat_sum_sq(size, X, Y, Z);
		else
		{	for(j = 0; j < n; j++)
			{	ax[j]   = X[j];
				ax[j+n] = X[j];
			}
			// Y = X * X
			atom_mul(ax, ay);
			Z[0] = 0.;
			for(j = 0; j < n; j++)
				Z[0] += ay[j];
		}
		// create function object f : X -> Z
		f.Dependent(X, Z);

		if( global_optimize )
			f.optimize();

		// skip comparison operators
		f.compare_change_count(0);

		// evaluate and return gradient using reverse mode
		z  = f.Forward(0, x);
		dz = f.Reverse(1, w);
	}
	else
	{	// get a next matrix
		CppAD::uniform_01(n, x);
		for(j = 0; j < n; j++)
			X[j] = x[j];

		// declare independent variables
		Independent(X);

		// do computations
		if( ! global_atomic )
			mat_sum_sq(size, X, Y, Z);
		else
		{	for(j = 0; j < n; j++)
			{	ax[j]   = X[j];
				ax[j+n] = X[j];
			}
			// Y = X * X
			atom_mul(ax, ay);
			Z[0] = 0.;
			for(j = 0; j < n; j++)
				Z[0] += ay[j];
		}

		// create function object f : X -> Z
		f.Dependent(X, Z);

		if( global_optimize )
			f.optimize();

		// skip comparison operators
		f.compare_change_count(0);

		while(repeat--)
		{	// get a next matrix
			CppAD::uniform_01(n, x);

			// evaluate and return gradient using reverse mode
			z  = f.Forward(0, x);
			dz = f.Reverse(1, w);
		}
	}
	// --------------------------------------------------------------------
	// Free temporary work space. (If there are future calls to
	// mat_mul they would create new temporary work space.)
	CppAD::user_atomic<double>::clear();

	return true;
}
/* $$
$end
*/
