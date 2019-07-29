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
$begin atomic_reciprocal.cpp$$
$spell
	enum
$$

$section Reciprocal as an Atomic Operation: Example and Test$$
$mindex set_sparsity_enum$$

$head Theory$$
This example demonstrates using $cref atomic_base$$
to define the operation
$latex f : \B{R}^n \rightarrow \B{R}^m$$ where
$latex n = 1$$, $latex m = 1$$, and $latex f(x) = 1 / x$$.

$head sparsity$$
This example only uses set sparsity patterns.

$nospell

$head Start Class Definition$$
$codep */
# include <cppad/cppad.hpp>
namespace {           // isolate items below to this file
using CppAD::vector;  // abbreviate as vector
//
// a utility to compute the union of two sets.
void my_union(
	std::set<size_t>&         result  ,
	const std::set<size_t>&   left    ,
	const std::set<size_t>&   right   )
{	std::set<size_t> temp;
	std::set_union(
		left.begin()              ,
		left.end()                ,
		right.begin()             ,
		right.end()               ,
		std::inserter(temp, temp.begin())
	);
	result.swap(temp);
}
//
class atomic_reciprocal : public CppAD::atomic_base<double> {
/* $$
$head Constructor $$
$codep */
	public:
	// constructor (could use const char* for name)
	atomic_reciprocal(const std::string& name) :
	// this exmaple only uses set sparsity patterns
	CppAD::atomic_base<double>(name, atomic_base<double>::set_sparsity_enum)
	{ }
	private:
/* $$
$head forward$$
$codep */
	// forward mode routine called by CppAD
	virtual bool forward(
		size_t                    p ,
		size_t                    q ,
		const vector<bool>&      vx ,
		      vector<bool>&      vy ,
		const vector<double>&    tx ,
		      vector<double>&    ty
	)
	{	size_t n = tx.size() / (q + 1);
		size_t m = ty.size() / (q + 1);
		assert( n == 1 );
		assert( m == 1 );
		assert( p <= q );

		// return flag
		bool ok = q <= 2;

		// check for defining variable information
		// This case must always be implemented
		if( vx.size() > 0 )
			vy[0] = vx[0];

		// Order zero forward mode.
		// This case must always be implemented
		// y^0 = f( x^0 ) = 1 / x^0
		double f = 1. / tx[0];
		if( p <= 0 )
			ty[0] = f;
		if( q <= 0 )
			return ok;
		assert( vx.size() == 0 );

		// Order one forward mode.
		// This case needed if first order forward mode is used.
		// y^1 = f'( x^0 ) x^1
		double fp = - f / tx[0];
		if( p <= 1 )
			ty[1] = fp * tx[1];
		if( q <= 1 )
			return ok;

		// Order two forward mode.
		// This case needed if second order forward mode is used.
		// Y''(t) = X'(t)^\R{T} f''[X(t)] X'(t) + f'[X(t)] X''(t)
		// 2 y^2  = x^1 * f''( x^0 ) x^1 + 2 f'( x^0 ) x^2
		double fpp  = - 2.0 * fp / tx[0];
		ty[2] = tx[1] * fpp * tx[1] / 2.0 + fp * tx[2];
		if( q <= 2 )
			return ok;

		// Assume we are not using forward mode with order > 2
		assert( ! ok );
		return ok;
	}
/* $$
$head reverse$$
$codep */
	// reverse mode routine called by CppAD
	virtual bool reverse(
		size_t                    q ,
		const vector<double>&    tx ,
		const vector<double>&    ty ,
		      vector<double>&    px ,
		const vector<double>&    py
	)
	{	size_t n = tx.size() / (q + 1);
		size_t m = ty.size() / (q + 1);
		assert( px.size() == n * (q + 1) );
		assert( py.size() == m * (q + 1) );
		assert( n == 1 );
		assert( m == 1 );
		bool ok = q <= 2;

		double f, fp, fpp, fppp;
		switch(q)
		{	case 0:
			// This case needed if first order reverse mode is used
			// reverse: F^0 ( tx ) = y^0 = f( x^0 )
			f     = ty[0];
			fp    = - f / tx[0];
			px[0] = py[0] * fp;;
			assert(ok);
			break;

			case 1:
			// This case needed if second order reverse mode is used
			// reverse: F^1 ( tx ) = y^1 = f'( x^0 ) x^1
			f      = ty[0];
			fp     = - f / tx[0];
			fpp    = - 2.0 * fp / tx[0];
			px[1]  = py[1] * fp;
			px[0]  = py[1] * fpp * tx[1];
			// reverse: F^0 ( tx ) = y^0 = f( x^0 );
			px[0] += py[0] * fp;
			assert(ok);
			break;

			case 2:
			// This needed if third order reverse mode is used
			// reverse: F^2 ( tx ) = y^2 =
			//          = x^1 * f''( x^0 ) x^1 / 2 + f'( x^0 ) x^2
			f      = ty[0];
			fp     = - f / tx[0];
			fpp    = - 2.0 * fp / tx[0];
			fppp   = - 3.0 * fpp / tx[0];
			px[2]  = py[2] * fp;
			px[1]  = py[2] * fpp * tx[1];
			px[0]  = py[2] * tx[1] * fppp * tx[1] / 2.0 + fpp * tx[2];
			// reverse: F^1 ( tx ) = y^1 = f'( x^0 ) x^1
			px[1] += py[1] * fp;
			px[0] += py[1] * fpp * tx[1];
			// reverse: F^0 ( tx ) = y^0 = f( x^0 );
			px[0] += py[0] * fp;
			assert(ok);
			break;

			default:
			assert(!ok);
		}
		return ok;
	}
/* $$
$head for_sparse_jac$$
$codep */
	// forward Jacobian set sparsity routine called by CppAD
	virtual bool for_sparse_jac(
		size_t                                p ,
		const vector< std::set<size_t> >&     r ,
		      vector< std::set<size_t> >&     s )
	{	// This function needed if using f.ForSparseJac
		size_t n = r.size();
		size_t m = s.size();
		assert( n == 1 );
		assert( m == 1 );

		// sparsity for S(x) = f'(x) * R is same as sparsity for R
		s[0] = r[0];

		return true;
	}
/* $$
$head rev_sparse_jac$$
$codep */
	// reverse Jacobian set sparsity routine called by CppAD
	virtual bool rev_sparse_jac(
		size_t                                p  ,
		const vector< std::set<size_t> >&     rt ,
		      vector< std::set<size_t> >&     st )
	{	// This function needed if using RevSparseJac or optimize
		size_t n = st.size();
		size_t m = rt.size();
		assert( n == 1 );
		assert( m == 1 );

		// sparsity for S(x)^T = f'(x)^T * R^T is same as sparsity for R^T
		st[0] = rt[0];

		return true;
	}
/* $$
$head rev_sparse_hes$$
$codep */
	// reverse Hessian set sparsity routine called by CppAD
	virtual bool rev_sparse_hes(
		const vector<bool>&                   vx,
		const vector<bool>&                   s ,
		      vector<bool>&                   t ,
		size_t                                p ,
		const vector< std::set<size_t> >&     r ,
		const vector< std::set<size_t> >&     u ,
		      vector< std::set<size_t> >&     v )
	{	// This function needed if using RevSparseHes
		size_t n = vx.size();
		size_t m = s.size();
		assert( t.size() == n );
		assert( r.size() == n );
		assert( u.size() == m );
		assert( v.size() == n );
		assert( n == 1 );
		assert( m == 1 );

		// There are no cross term second derivatives for this case,
		// so it is not necessary to vx.

		// sparsity for T(x) = S(x) * f'(x) is same as sparsity for S
		t[0] = s[0];

		// V(x) = f'(x)^T * g''(y) * f'(x) * R  +  g'(y) * f''(x) * R
		// U(x) = g''(y) * f'(x) * R
		// S(x) = g'(y)

		// back propagate the sparsity for U, note f'(x) may be non-zero;
		v[0] = u[0];

		// include forward Jacobian sparsity in Hessian sparsity
		// (note sparsty for f''(x) * R same as for R)
		if( s[0] )
			my_union(v[0], v[0], r[0] );

		return true;
	}
/* $$
$head End Class Definition$$
$codep */
}; // End of atomic_reciprocal class
}  // End empty namespace

/* $$
$head Use Atomic Function$$
$codep */
bool reciprocal(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	double eps = 10. * CppAD::numeric_limits<double>::epsilon();
/* $$
$subhead Constructor$$
$codep */
	// --------------------------------------------------------------------
	// Create the atomic reciprocal object
	atomic_reciprocal afun("atomic_reciprocal");
/* $$
$subhead Recording$$
$codep */
	// Create the function f(x)
	//
	// domain space vector
	size_t n  = 1;
	double  x0 = 0.5;
	vector< AD<double> > ax(n);
	ax[0]     = x0;

	// declare independent variables and start tape recording
	CppAD::Independent(ax);

	// range space vector
	size_t m = 1;
	vector< AD<double> > ay(m);

	// call user function and store reciprocal(x) in au[0]
	vector< AD<double> > au(m);
	afun(ax, au);        // u = 1 / x

	// now use AD division to invert to invert the operation
	ay[0] = 1.0 / au[0]; // y = 1 / u = x

	// create f: x -> y and stop tape recording
	CppAD::ADFun<double> f;
	f.Dependent (ax, ay);  // f(x) = x
/* $$
$subhead forward$$
$codep */
	// check function value
	double check = x0;
	ok &= NearEqual( Value(ay[0]) , check,  eps, eps);

	// check zero order forward mode
	size_t q;
	vector<double> x_q(n), y_q(m);
	q      = 0;
	x_q[0] = x0;
	y_q    = f.Forward(q, x_q);
	ok &= NearEqual(y_q[0] , check,  eps, eps);

	// check first order forward mode
	q      = 1;
	x_q[0] = 1;
	y_q    = f.Forward(q, x_q);
	check  = 1.;
	ok &= NearEqual(y_q[0] , check,  eps, eps);

	// check second order forward mode
	q      = 2;
	x_q[0] = 0;
	y_q    = f.Forward(q, x_q);
	check  = 0.;
	ok &= NearEqual(y_q[0] , check,  eps, eps);
/* $$
$subhead reverse$$
$codep */
	// third order reverse mode
	q     = 3;
	vector<double> w(m), dw(n * q);
	w[0]  = 1.;
	dw    = f.Reverse(q, w);
	check = 1.;
	ok &= NearEqual(dw[0] , check,  eps, eps);
	check = 0.;
	ok &= NearEqual(dw[1] , check,  eps, eps);
	ok &= NearEqual(dw[2] , check,  eps, eps);
/* $$
$subhead for_sparse_jac$$
$codep */
	// forward mode sparstiy pattern
	size_t p = n;
	CppAD::vectorBool r1(n * p), s1(m * p);
	r1[0] = true;          // compute sparsity pattern for x[0]
	//
	s1    = f.ForSparseJac(p, r1);
	ok  &= s1[0] == true;  // f[0] depends on x[0]
/* $$
$subhead rev_sparse_jac$$
$codep */
	// reverse mode sparstiy pattern
	q = m;
	CppAD::vectorBool s2(q * m), r2(q * n);
	s2[0] = true;          // compute sparsity pattern for f[0]
	//
	r2    = f.RevSparseJac(q, s2);
	ok  &= r2[0] == true;  // f[0] depends on x[0]
/* $$
$subhead rev_sparse_hes$$
$codep */
	// Hessian sparsity (using previous ForSparseJac call)
	CppAD::vectorBool s3(m), h(p * n);
	s3[0] = true;        // compute sparsity pattern for f[0]
	//
	h     = f.RevSparseHes(p, s3);
	ok  &= h[0] == true; // second partial of f[0] w.r.t. x[0] may be non-zero
	//
	return ok;
}
/* $$
$$ $comment end nospell$$
$end
*/
