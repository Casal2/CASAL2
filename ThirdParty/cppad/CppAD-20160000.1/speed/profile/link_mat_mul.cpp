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
$begin link_mat_mul$$
$spell
	mul
	bool
	CppAD
	dz
$$


$section Speed Testing Derivative of Matrix Multiply$$
$mindex link_mat_mul test multiple$$

$head Prototype$$
$codei%extern bool link_mat_mul(
	size_t                         %size%    ,
	size_t                         %repeat%  ,
	CppAD::vector<double>&         %x%       ,
	CppAD::vector<double>&         %z%       ,
	CppAD::vector<double>&         %dz%
);
%$$

$head Purpose$$
Each $cref/package/speed_main/package/$$
must define a version of this routine as specified below.
This is used by the $cref speed_main$$ program
to run the corresponding speed and correctness tests.

$head Return Value$$
If this speed test is not yet
supported by a particular $icode package$$,
the corresponding return value for $code link_mat_mul$$
should be $code false$$.

$head n$$
The argument $icode n$$ is the number of rows and columns
in the square matrix $icode x$$.

$head repeat$$
The argument $icode repeat$$ is the number of different argument values
that the derivative of $icode z$$ (or just the value of $icode z$$)
will be computed.

$head x$$
The argument $icode x$$ is a vector with
$icode%x%.size() = %size% * %size%$$ elements.
The input value of its elements does not matter.
The output value of its elements is the last random matrix
that is multiplied and then summed to form $icode z$$;
$latex \[
	x_{i,j} = x[ i * s + j ]
\] $$
where $icode%s% = %size%$$.

$head z$$
The argument $icode z$$ is a vector with one element.
The input value of the element does not matter.
The output of its element the sum of the elements of
$icode%y% = %x% * %x%$$; i.e.,
$latex \[
\begin{array}{rcl}
	y_{i,j} & = & \sum_{k=0}^{s-1} x_{i,k} x_{k, j}
	\\
	z       & = & \sum_{i=0}^{s-1} \sum_{j=0}^{s-1} y_{i,j}
\end{array}
\] $$

$head dz$$
The argument $icode dz$$ is a vector with
$icode%dz%.size() = %size% * %size%$$.
The input values of its elements do not matter.
The output value of its elements form the
derivative of $icode z$$ with respect to $icode x$$.

$end
-----------------------------------------------------------------------------
*/
# include <cppad/utility/vector.hpp>
# include <cppad/utility/near_equal.hpp>

extern bool link_mat_mul(
	size_t                     size     ,
	size_t                     repeat   ,
	CppAD::vector<double>&      x       ,
	CppAD::vector<double>&      z       ,
	CppAD::vector<double>&      dz
);
bool available_mat_mul(void)
{	size_t size   = 2;
	size_t repeat = 1;
	CppAD::vector<double>  x(size * size), z(1), dz(size * size);

	return link_mat_mul(size, repeat, x, z, dz);
}
bool correct_mat_mul(bool is_package_double)
{	size_t size   = 2;
	size_t repeat = 1;
	CppAD::vector<double>  x(size * size), z(1), dz(size * size);

	link_mat_mul(size, repeat, x, z, dz);

	double x00 = x[0 * size + 0];
	double x01 = x[0 * size + 1];
	double x10 = x[1 * size + 0];
	double x11 = x[1 * size + 1];
	bool ok = true;
	double check;
	if( is_package_double )
	{	check  = 0;
		check += x00 * x00 + x01 * x10; // y00
		check += x00 * x01 + x01 * x11; // y01
		check += x10 * x00 + x11 * x10; // y10
		check += x10 * x01 + x11 * x11; // y11
		ok &= CppAD::NearEqual(check, z[0], 1e-10, 1e-10);
		return ok;
	}
	// partial w.r.t. x00
	check = x00 + x00 + x01 + x10;
	ok   &= CppAD::NearEqual(check, dz[0 * size + 0], 1e-10, 1e-10);
	// partial w.r.t. x01
	check = x10 + x00 + x11 + x10;
	ok   &= CppAD::NearEqual(check, dz[0 * size + 1], 1e-10, 1e-10);
	// partial w.r.t. x10
	check = x01 + x00 + x11 + x01;
	ok   &= CppAD::NearEqual(check, dz[1 * size + 0], 1e-10, 1e-10);
	// partial w.r.t. x11
	check = x01 + x10 + x11 + x11;
	ok   &= CppAD::NearEqual(check, dz[1 * size + 1], 1e-10, 1e-10);

	return ok;
}

void speed_mat_mul(size_t size, size_t repeat)
{	CppAD::vector<double>  x(size * size), z(1), dz(size * size);

	link_mat_mul(size, repeat, x, z, dz);
	return;
}
