// $Id$
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
# include <cstring>
# include <cppad/utility/vector.hpp>

/*
$begin sacado_sparse_jacobian.cpp$$
$spell
	const
	bool
	CppAD
	sacado
	sparse_jacobian
$$

$section sacado Speed: sparse_jacobian$$

$codep */
// A sacado version of this test is not yet available
bool link_sparse_jacobian(
	size_t                           size     ,
	size_t                           repeat   ,
	size_t                           m        ,
	const CppAD::vector<size_t>&     row      ,
	const CppAD::vector<size_t>&     col      ,
	      CppAD::vector<double>&     x        ,
	      CppAD::vector<double>&     jacobian ,
	      size_t&                    n_sweep  )
{
	return false;
}
/* $$
$end
*/
