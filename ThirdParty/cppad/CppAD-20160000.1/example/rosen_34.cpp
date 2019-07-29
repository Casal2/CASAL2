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
$begin rosen_34.cpp$$
$spell
	Rosen
$$

$section Rosen34: Example and Test$$
$mindex Rosen34$$


Define
$latex X : \B{R} \rightarrow \B{R}^n$$ by
$latex \[
	X_i (t) =  t^{i+1}
\] $$
for $latex i = 1 , \ldots , n-1$$.
It follows that
$latex \[
\begin{array}{rclr}
X_i(0)     & = & 0                             & {\rm for \; all \;} i \\
X_i ' (t)  & = & 1                             & {\rm if \;} i = 0      \\
X_i '(t)   & = & (i+1) t^i = (i+1) X_{i-1} (t) & {\rm if \;} i > 0
\end{array}
\] $$
The example tests Rosen34 using the relations above:

$code
$verbatim%example/rosen_34.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++

# include <cppad/cppad.hpp>        // For automatic differentiation

namespace {
	class Fun {
	public:
		// constructor
		Fun(bool use_x_) : use_x(use_x_)
		{ }

		// compute f(t, x) both for double and AD<double>
		template <typename Scalar>
		void Ode(
			const Scalar                    &t,
			const CPPAD_TESTVECTOR(Scalar) &x,
			CPPAD_TESTVECTOR(Scalar)       &f)
		{	size_t n  = x.size();
			Scalar ti(1);
			f[0]   = Scalar(1);
			size_t i;
			for(i = 1; i < n; i++)
			{	ti *= t;
				// convert int(size_t) to avoid warning
				// on _MSC_VER systems
				if( use_x )
					f[i] = int(i+1) * x[i-1];
				else	f[i] = int(i+1) * ti;
			}
		}

		// compute partial of f(t, x) w.r.t. t using AD
		void Ode_ind(
			const double                    &t,
			const CPPAD_TESTVECTOR(double) &x,
			CPPAD_TESTVECTOR(double)       &f_t)
		{	using namespace CppAD;

			size_t n  = x.size();
			CPPAD_TESTVECTOR(AD<double>) T(1);
			CPPAD_TESTVECTOR(AD<double>) X(n);
			CPPAD_TESTVECTOR(AD<double>) F(n);

			// set argument values
			T[0] = t;
			size_t i;
			for(i = 0; i < n; i++)
				X[i] = x[i];

			// declare independent variables
			Independent(T);

			// compute f(t, x)
			this->Ode(T[0], X, F);

			// define AD function object
			ADFun<double> fun(T, F);

			// compute partial of f w.r.t t
			CPPAD_TESTVECTOR(double) dt(1);
			dt[0] = 1.;
			f_t = fun.Forward(1, dt);
		}

		// compute partial of f(t, x) w.r.t. x using AD
		void Ode_dep(
			const double                    &t,
			const CPPAD_TESTVECTOR(double) &x,
			CPPAD_TESTVECTOR(double)       &f_x)
		{	using namespace CppAD;

			size_t n  = x.size();
			CPPAD_TESTVECTOR(AD<double>) T(1);
			CPPAD_TESTVECTOR(AD<double>) X(n);
			CPPAD_TESTVECTOR(AD<double>) F(n);

			// set argument values
			T[0] = t;
			size_t i, j;
			for(i = 0; i < n; i++)
				X[i] = x[i];

			// declare independent variables
			Independent(X);

			// compute f(t, x)
			this->Ode(T[0], X, F);

			// define AD function object
			ADFun<double> fun(X, F);

			// compute partial of f w.r.t x
			CPPAD_TESTVECTOR(double) dx(n);
			CPPAD_TESTVECTOR(double) df(n);
			for(j = 0; j < n; j++)
				dx[j] = 0.;
			for(j = 0; j < n; j++)
			{	dx[j] = 1.;
				df = fun.Forward(1, dx);
				for(i = 0; i < n; i++)
					f_x [i * n + j] = df[i];
				dx[j] = 0.;
			}
		}

	private:
		const bool use_x;

	};
}

bool Rosen34(void)
{	bool ok = true;     // initial return value
	size_t i;           // temporary indices

	size_t  n = 4;      // number components in X(t) and order of method
	size_t  M = 2;      // number of Rosen34 steps in [ti, tf]
	double ti = 0.;     // initial time
	double tf = 2.;     // final time

	// xi = X(0)
	CPPAD_TESTVECTOR(double) xi(n);
	for(i = 0; i <n; i++)
		xi[i] = 0.;

	size_t use_x;
	for( use_x = 0; use_x < 2; use_x++)
	{	// function object depends on value of use_x
		Fun F(use_x > 0);

		// compute Rosen34 approximation for X(tf)
		CPPAD_TESTVECTOR(double) xf(n), e(n);
		xf = CppAD::Rosen34(F, M, ti, tf, xi, e);

		double check = tf;
		for(i = 0; i < n; i++)
		{	// check that error is always positive
			ok    &= (e[i] >= 0.);
			// 4th order method is exact for i < 4
			if( i < 4 ) ok &=
				CppAD::NearEqual(xf[i], check, 1e-10, 1e-10);
			// 3rd order method is exact for i < 3
			if( i < 3 )
				ok &= (e[i] <= 1e-10);

			// check value for next i
			check *= tf;
		}
	}
	return ok;
}

// END C++
