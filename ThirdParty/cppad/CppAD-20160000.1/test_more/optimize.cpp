// $Id$
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
// 2DO: Test that optimize.hpp use of base_atomic<Base>::rev_sparse_jac works.

# include <limits>
# include <cppad/cppad.hpp>


namespace {
	// include conditional skip optimization
	bool conditional_skip_;

	// accuracy for almost equal checks
	double eps_ = 10. * std::numeric_limits<double>::epsilon();
	using CppAD::NearEqual;

	// note this enum type is not part of the API (but its values are)
	CppAD::atomic_base<double>::option_enum atomic_sparsity_option_;
	//
	// ----------------------------------------------------------------
	// Test nested conditional expressions.
	bool nested_cond_exp(void)
	{	bool ok = true;
		using CppAD::AD;
		using CppAD::vector;

		// independent variable vector
		vector< AD<double> > ax(2), ay(1);
		ax[0] = 1.0;
		ax[1] = 2.0;
		Independent(ax);

		// first conditional expression
		AD<double> ac1 = CondExpLe(ax[0], ax[1], 2.0 * ax[0], 3.0 * ax[1] );

		// second conditional expression
		AD<double> ac2 = CondExpGe(ax[0], ax[1], 4.0 * ax[0], 5.0 * ax[1] );

		// third conditional expression
		AD<double> ac3 = CondExpLt(ax[0], ax[1], 6.0 * ac1, 7.0 * ac2 );

		// create function object f : ax -> ay
		ay[0] = ac3;
		CppAD::ADFun<double> f(ax, ay);

		// now optimize the operation sequence
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		// now zero order forward
		vector<double> x(2), y(1);
		for(size_t i = 0; i < 3; i++)
		{	x[0] = 1.0 - double(i);
			x[1] = - x[0];
			y    = f.Forward(0, x);
			//
			// first conditional expression
			double c1;
			if( x[0] <= x[1] )
				c1 = 2.0 * x[0];
			else	c1 = 3.0 * x[1];
			//
			// second conditional expression
			double c2;
			if( x[0] >= x[1] )
				c2 = 4.0 * x[0];
			else	c2 = 5.0 * x[1];

			// third conditional expression
			double c3;
			if( x[0] < x[1] )
				c3 = 6.0 * c1;
			else	c3 = 7.0 * c2;

			ok &= y[0] == c3;
		}
		return ok;
	}
	// ----------------------------------------------------------------
	// Test for bug where checkpoint function did not depend on
	// the operands in the logical comparison because of the CondExp
	// sparsity pattern.
	void j_algo(
		const CppAD::vector< CppAD::AD<double> >& ax ,
		      CppAD::vector< CppAD::AD<double> >& ay )
	{	ay[0] = CondExpGt(ax[0], ax[1], ax[2], ax[3]); }

	bool atomic_cond_exp_sparsity(void)
	{	bool ok = true;
		using CppAD::AD;
		using CppAD::vector;

		// Create a checkpoint version of the function g
		vector< AD<double> > au(4), av(1);
		for(size_t i = 0; i < 4; i++)
			au[i] = AD<double>(i);
		CppAD::checkpoint<double> j_check("j_check", j_algo, au, av);

		// independent variable vector
		vector< AD<double> > ax(2), ay(1);
		ax[0] = 1.;
		ax[1] = 1.;
		Independent(ax);

		// call atomic function that does not get used
		for(size_t i = 0; i < 4; i++)
			au[i] = ax[0] + AD<double>(i + 1) * ax[1];
		j_check(au, ay);

		// create function object f : ax -> ay
		CppAD::ADFun<double> f(ax, ay);


		// now optimize the operation sequence
		j_check.option( atomic_sparsity_option_ );
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		// check result where true case is used; i.e., au[0] > au[1]
		vector<double> x(2), y(1);
		x[0] = 1.;
		x[1] = -1;
		y    = f.Forward(0, x);
		ok  &= y[0] == x[0] + double(3) * x[1];


		// check result where false case is used; i.e., au[0] <= au[1]
		x[0] = 1.;
		x[1] = 1;
		y    = f.Forward(0, x);
		ok  &= y[0] == x[0] + double(4) * x[1];

		return ok;
	}
	// -------------------------------------------------------------------
	// Test conditional optimizing out call to an atomic function call
	void k_algo(
		const CppAD::vector< CppAD::AD<double> >& x ,
		      CppAD::vector< CppAD::AD<double> >& y )
	{	y[0] = x[0] + x[1]; }

	void h_algo(
		const CppAD::vector< CppAD::AD<double> >& x ,
		      CppAD::vector< CppAD::AD<double> >& y )
	{	y[0] = x[0] - x[1]; }

	bool atomic_cond_exp(void)
	{	bool ok = true;
		typedef CppAD::vector< CppAD::AD<double> > ADVector;

		// Create a checkpoint version of the function g
		ADVector ax(2), ag(1), ah(1), ay(1);
		ax[0] = 0.;
		ax[1] = 1.;
		CppAD::checkpoint<double> k_check("k_check", k_algo, ax, ag);
		CppAD::checkpoint<double> h_check("h_check", h_algo, ax, ah);

		// independent variable vector
		Independent(ax);

		// atomic function calls that get conditionally used
		k_check(ax, ag);
		h_check(ax, ah);

		// conditional expression
		ay[0] = CondExpLt(ax[0], ax[1], ag[0], ah[0]);

		// create function object f : ax -> ay
		CppAD::ADFun<double> f;
		f.Dependent(ax, ay);

		// use zero order to evaluate when condition is true
		CppAD::vector<double>  x(2), dx(2);
		CppAD::vector<double>  y(1), dy(1), w(1);
		x[0] = 3.;
		x[1] = 4.;
		y    = f.Forward(0, x);
		ok  &= y[0] == x[0] + x[1];

		// before optimize
		k_check.option( atomic_sparsity_option_ );
		h_check.option( atomic_sparsity_option_ );
		ok  &= f.number_skip() == 0;

		// now optimize the operation sequence
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		// optimized zero order forward when condition is false
		x[0] = 4.;
		x[1] = 3.;
		y    = f.Forward(0, x);
		ok  &= y[0] == x[0] - x[1];

		// after optimize can skip either call to g or call to h
		ok  &= f.number_skip() == 1;

		// optimized first order forward
		dx[0] = 2.;
		dx[1] = 1.;
		dy    = f.Forward(1, dx);
		ok   &= dy[0] == dx[0] - dx[1];

		// optimized first order reverse
		w[0]  = 1.;
		dx    = f.Reverse(1, w);
		ok   &= dx[0] == 1.;
		ok   &= dx[1] == -1.;

		return ok;
	}
	// -------------------------------------------------------------------
	// Test of optimizing out arguments to an atomic function
	void g_algo(
		const CppAD::vector< CppAD::AD<double> >& ax ,
		      CppAD::vector< CppAD::AD<double> >& ay )
	{	ay = ax; }

	bool atomic_no_used(void)
	{	bool ok = true;
		using CppAD::AD;
		using CppAD::vector;

		// Create a checkpoint version of the function g
		vector< AD<double> > ax(2), ay(2), az(1);
		ax[0] = 0.;
		ax[1] = 1.;
		CppAD::checkpoint<double> g_check("g_check", g_algo, ax, ay);

		// independent variable vector
		Independent(ax);

		// call atomic function that does not get used
		g_check(ax, ay);

		// conditional expression
		az[0] = CondExpLt(ax[0], ax[1], ax[0] + ax[1], ax[0] - ax[1]);

		// create function object f : ax -> az
		CppAD::ADFun<double> f(ax, az);

		// number of variables before optimization
		// (include ay[0] and ay[1])
		size_t n_before = f.size_var();

		// now optimize the operation sequence
		g_check.option( atomic_sparsity_option_ );
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		// number of variables after optimization
		// (does not include ay[0] and ay[1])
		size_t n_after = f.size_var();
		ok            &= n_after + 2 == n_before;

		// check optimization works ok
		vector<double> x(2), z(1);
		x[0] = 4.;
		x[1] = 3.;
		z    = f.Forward(0, x);
		ok  &= z[0] == x[0] - x[1];

		return ok;
	}
	bool atomic_arguments(void)
	{	bool ok = true;
		using CppAD::AD;
		using CppAD::vector;
		vector< AD<double> > au(2), aw(2), ax(2), ay(1);

		// create atomic function corresponding to g_algo
		au[0] = 1.0;
		au[1] = 2.0;
		CppAD::checkpoint<double> g_check("g_algo", g_algo, au, ax);

		// start recording a new function
		CppAD::Independent(ax);

		// now use g_check during the recording
		au[0] = ax[0] + ax[1]; // this argument requires a new variable
		au[1] = ax[0] - ax[1]; // this argument also requires a new variable
		g_check(au, aw);

		// now create f(x) = x_0 - x_1
		ay[0] = aw[0];
		CppAD::ADFun<double> f(ax, ay);

		// number of variables before optimization
		size_t n_before = f.size_var();

		// now optimize f so that the calculation of au[1] is removed
		g_check.option( atomic_sparsity_option_ );
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		// check difference in number of variables
		size_t n_after = f.size_var();
		ok &= n_before == n_after + 1;

		// now compute and check a forward mode calculation
		vector<double> x(2), y(1);
		x[0] = 5.0;
		x[1] = 6.0;
		y    = f.Forward(0, x);
		ok  &= (y[0] == x[0] + x[1]);

		return ok;
	}

	// -------------------------------------------------------------------
	// Test the reverse dependency analysis optimization
	template <class Vector>
	void depend_fun
	(const Vector& x, Vector& y, size_t& original, size_t& opt)
	{	typedef typename Vector::value_type Scalar;
		Scalar not_used;
		Scalar one(1), two(2), three(3);

		// independent variable and phantom at beginning
		original = 1 + x.size();
		opt      = 1 + x.size();

		// unary operator where operand is arg[0]
		// (note that sin corresponds to two tape variables)
		not_used = CppAD::abs(x[0]);
		y[0]     = sin(x[0]);
		original += 3;
		opt      += 2;

		// binary operator where left operand is a variable
		// and right operand is a parameter
		not_used = not_used + 2.;
		y[1]     = x[1] * 3.;
		original += 2;
		opt      += 1;

		// binary operator where left operand is a parameter
		// and right operation is a variable
		not_used = 2. - not_used;
		y[2]     = 3. / x[2];
		original += 2;
		opt      += 1;

		// binary operator where both operands are variables
		not_used  = x[3] - not_used;
		y[3]      = x[3] / x[2];
		original += 2;
		opt      += 1;

		// conditional expression that will be optimized out
		not_used = CppAD::CondExpLt(x[0], x[1], x[2], x[3]) + not_used;
		y[4]     = CppAD::CondExpLt(x[4], one, two, three);
		original += 3;
		opt      += 1;

		// y[5] does not depend on the value of not_used.
		// Make sure a parameter, corresponding to a dependent variable,
		// is not optimized out of the operation sequence.
		y[5]      = 0.0 * not_used;
		original += 1;
		opt      += 1;

		// Wwe do not use the argument x[5], to
		// make sure it is not optimized out.

		return;
	}

	bool depend_one(void)
	{	// Test all except for VecAD operations
		bool ok = true;
		using CppAD::AD;
		size_t original;
		size_t opt;
		size_t i, j;

		// domain space vector
		size_t n  = 6;
		CppAD::vector< AD<double> > X(n);
		for(j = 0; j < n; j++)
			X[j] = 1. / double(j + 1);

		// declare independent variables and start tape recording
		CppAD::Independent(X);

		// range space vector
		size_t m = n;
		CppAD::vector< AD<double> > Y(m);
		depend_fun(X, Y, original, opt);

		// create f: X -> Y and stop tape recording
		CppAD::ADFun<double> F;
		F.Dependent(X, Y);

		CppAD::vector<double> x(n), y(m), check(m);
		for(j = 0; j < n; j++)
			x[j] = Value(X[j]);
		y = F.Forward(0, x);
		depend_fun(x, check, original, opt);
		for(i = 0; i < m; i++)
			ok &= NearEqual(y[i], check[i], eps_, eps_);

		// Check size before optimization
		ok &= F.size_var() == original;

		// Optimize the operation sequence
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// Check size after optimization
		ok &= F.size_var() == opt;

		// check result now
		// (should have already been checked if NDEBUG not defined)
		y = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= NearEqual(y[i], check[i], eps_, eps_);

		return ok;
	}

	bool depend_two(void)
	{	// Test VecAD operations
		bool ok = true;
		using CppAD::AD;
		size_t i, j;

		// domain space vector
		size_t n  = 2;
		CppAD::vector< AD<double> > X(n);
		for(j = 0; j < n; j++)
			X[j] = double(j);

		// range space vector
		size_t m = 3;
		CppAD::vector< AD<double> > Y(m);

		CppAD::VecAD<double> U(m);
		CppAD::VecAD<double> V(n);
		for(i = 0; i < m; i++)
			U[i] = 0;
		for(j = 0; j < n; j++)
			V[j] = 0;

		// declare independent variables and start tape recording
		CppAD::Independent(X);

		// first vecad vector that is a variable
		U[ X[0] ] = X[1];

		// second vecad vector that is a variable
		V[ X[0] ] = X[1];

		// Make dependency for vecad vectors different that for
		// variables because original code used worng dependency info.
		// Y does not depend on the first variable in the tape; i.e.
		// the one corresponding to the BeginOp. So make it depend
		// on the first vecad vector in the tape.
		for(i = 0; i < m; i++)
		{	AD<double> I(i);
			Y[i] = U[I];
		}

		// create f: X -> Y and stop tape recording
		// Y[ X[0] ] = X[1] and other components of Y are zero.
		CppAD::ADFun<double> F;
		F.Dependent(X, Y);

		// Check number of VecAD vectors plus number of VecAD elements
		ok &= (F.size_VecAD() == 2 + n + m);

		CppAD::vector<double> x(n), y(m);
		for(j = 0; j < n; j++)
			x[j] = double(j);

		y = F.Forward(0, x);
		for(i = 0; i < m; i++)
		{	if( i != static_cast<size_t>(x[0]) )
				ok &= (y[i] == 0.);
			else	ok &= (y[i] == x[1]);
		}

		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// Check number of VecAD vectors plus number of VecAD elements
		ok &= (F.size_VecAD() == 1 + m);
		y = F.Forward(0, x);
		for(i = 0; i < m; i++)
		{	if( i != static_cast<size_t>(x[0]) )
				ok &= (y[i] == 0.);
			else	ok &= (y[i] == x[1]);
		}

		return ok;
	}
	bool depend_three(void)
	{	// Power function is a special case for optimize
		bool ok = true;
		using CppAD::AD;
		using CppAD::vector;

		size_t n = 3;
		size_t j;

		vector< AD<double> >    X(n), Y(n);
		vector<double>          x(n), y(n);

		for(j = 0; j < n; j++)
			X[j] = x[j] = double(j+2);

		CppAD::Independent(X);

		Y[0] = pow(X[0], 2.0);
		Y[1] = pow(2.0, X[1]);
		Y[2] = pow(X[0], X[1]);

		CppAD::ADFun<double> F(X, Y);
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");
		y = F.Forward(0, x);

		// Use identically equal because the result of the operations
		// have been stored as double and gaurd bits have been dropped.
		// (This may not be true for some compiler in the future).
		for(j = 0; j < n; j++)
			ok &= ( y[j] == Value(Y[j]) );

		// check reverse mode derivative
		vector<double>   w(n), dw(n);
		w[0] = 0.;
		w[1] = 0.;
		w[2] = 1.;
		dw = F.Reverse(1, w);

		double check = x[1] * pow( x[0], x[1] - 1. );
		ok &= NearEqual( dw[0], check, eps_, eps_ );

		check = log( x[0] ) * pow( x[0], x[1] );
		ok &= NearEqual( dw[1], check, eps_, eps_ );

		check = 0.;
		ok &= NearEqual( dw[2], check, eps_, eps_ );

		return ok;
	}
	bool depend_four(void)
	{	// erf function is a special case for optimize
		bool ok = true;
# if CPPAD_USE_CPLUSPLUS_2011
		using CppAD::AD;
		using CppAD::vector;

		size_t n = 1;
		size_t m = 1;
		vector< AD<double> > X(n), Y(m);
		vector<double>       x(n);
		X[0] = x[0] = double(0.5);

		CppAD::Independent(X);

		Y[0] = erf(X[0]) + erf(X[0]);

		CppAD::ADFun<double> F(X, Y);

		vector<double> y_original     = F.Forward(0, x);
		size_t         size_original  = F.size_var();
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");
		ok &= F.size_var() + 5 == size_original;
		vector<double> y = F.Forward(0, x);
		ok &=  NearEqual(y[0], y_original[0], eps_, eps_);
# endif
		return ok;
	}
	// ===================================================================
	// Test duplicate operation analysis

	template <class Vector>
	void duplicate_fun
	(const Vector& x, Vector& y, size_t& original, size_t& opt)
	{	typedef typename Vector::value_type Scalar;
		original = 0;
		opt      = 0;

		// unary operator where operand is arg[0] and one result
		Scalar a1 = CppAD::exp(x[0]);
		original += 1;
		opt      += 1;

		// unary operator where operand is arg[0] and two results
		Scalar b1 = CppAD::sin(x[1]);
		original += 2;
		opt      += 2;

		// non-commutative binary operator where left is a variable
		// and right is a parameter
		Scalar c1 = x[2] - 3.;
		original += 1;
		opt      += 1;

		// non-commutative binary operator where left is a parameter
		// and right is a variable
		Scalar d1 = 3. / x[3];
		original += 1;
		opt      += 1;

		// non-commutative binary operator where left is a variable
		// and right is a variable
		Scalar e1 = pow(x[3], x[4]);
		original += 3;
		opt      += 3;

		// commutative binary operator where  left is a variable
		// and right is a parameter
		Scalar f1 = x[5] * 5.;
		original += 1;
		opt      += 1;

		// commutative binary operator where  left is a variable
		// and right is a variable
		Scalar g1 = x[5] + x[6];
		original += 1;
		opt      += 1;

		// duplicate variables
		Scalar a2 = CppAD::exp(x[0]);
		Scalar b2 = CppAD::sin(x[1]);  // counts for 2 variables
		Scalar c2 = x[2] - 3.;
		Scalar d2 = 3. / x[3];
		Scalar e2 = pow(x[3], x[4]);   // counts for 3 variables
		Scalar f2 = 5. * x[5];
		Scalar g2 = x[6] + x[5];
		original += 10;

		// result vector
		y[0] = a1 * a2;
		y[1] = b1 * b2;
		y[2] = c1 * c2;
		y[3] = d1 * d2;
		y[4] = e1 * e2;
		y[5] = f1 * f2;
		y[6] = g1 * g2;
		original += 7;
		opt      += 7;

		return;
	}
	bool duplicate_one(void)
	{
		bool ok = true;
		using CppAD::AD;
		size_t original;
		size_t opt;
		size_t i, j;

		// domain space vector
		size_t n  = 7;
		CppAD::vector< AD<double> > X(n);
		for(j = 0; j < n; j++)
			X[j] = 1. / double(j + 1);

		// declare independent variables and start tape recording
		CppAD::Independent(X);

		// range space vector
		size_t m = n;
		CppAD::vector< AD<double> > Y(m);
		duplicate_fun(X, Y, original, opt);

		// create f: X -> Y and stop tape recording
		CppAD::ADFun<double> F;
		F.Dependent(X, Y);

		CppAD::vector<double> x(n), y(m), check(m);
		for(j = 0; j < n; j++)
			x[j] = Value(X[j]);
		y = F.Forward(0, x);
		duplicate_fun(x, check, original, opt);
		for(i = 0; i < m; i++)
			ok &= NearEqual(y[i], check[i], eps_, eps_);

		// Check size before optimization
		ok &= F.size_var() == (n + 1 + original);

		// Optimize the operation sequence
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// Check size after optimization
		ok &= F.size_var() == (n + 1 + opt);

		// check result now
		// (should have already been checked if NDEBUG not defined)
		y = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= NearEqual(y[i], check[i], eps_, eps_);

		return ok;
	}
	// -------------------------------------------------------------------
	bool duplicate_two(void)
	{	// test that duplicate expression removal is relative to
		// new and not just old argument indices.
		bool ok = true;
		using CppAD::AD;
		size_t i, j;

		// domain space vector
		size_t n  = 1;
		CppAD::vector< AD<double> > X(n);
		for(j = 0; j < n; j++)
			X[j] = double(j + 2);

		// range space vector
		size_t m = 1;
		CppAD::vector< AD<double> > Y(m);

		// declare independent variables and start tape recording
		CppAD::Independent(X);

		// create a new variable
		AD<double> A1 = X[0] - 2.;

		// create a duplicate variable
		AD<double> A2 = X[0] - 2.;

		// create a new variable using first version of duplicate
		AD<double> B1 = A1 / 2.;

		// create a duplicate that can only be dectected using new
		// argument indices
		AD<double> B2 = A2 / 2.;

		// Make a new variable for result
		// and make it depend on all the variables
		Y[0] = B1 + B2;

		// create f: X -> Y and stop tape recording
		CppAD::ADFun<double> F;
		F.Dependent(X, Y);

		// check number of variables in original function
		ok &= (F.size_var() ==  1 + n + m + 4 );

		CppAD::vector<double> x(n), y(m);
		for(j = 0; j < n; j++)
			x[j] = double(j + 2);

		y   = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= ( y[i] == Value( Y[i] ) );

		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// check number of variables  in optimized version
		ok &= (F.size_var() == 1 + n + m + 2 );

		y   = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= ( y[i] == Value( Y[i] ) );

		return ok;
	}
	// -------------------------------------------------------------------
	bool duplicate_three(void)
	{	// test that duplicate expression removal is relative to
		// new and not just old argument indices (commutative case).
		bool ok = true;
		using CppAD::AD;
		size_t i, j;

		// domain space vector
		size_t n  = 1;
		CppAD::vector< AD<double> > X(n);
		for(j = 0; j < n; j++)
			X[j] = double(j + 2);

		// range space vector
		size_t m = 1;
		CppAD::vector< AD<double> > Y(m);

		// declare independent variables and start tape recording
		CppAD::Independent(X);

		// create a new variable
		AD<double> A1 = X[0] + 2.;

		// create a duplicate variable
		AD<double> A2 = 2. + X[0];

		// create a new variable using first version of duplicate
		AD<double> B1 = A1 * 2.;

		// create a duplicate that can only be dectected using new
		// argument indices
		AD<double> B2 = 2. * A2;

		// Make a new variable for result
		// and make it depend on all the variables
		Y[0] = B1 + B2;

		// create f: X -> Y and stop tape recording
		CppAD::ADFun<double> F;
		F.Dependent(X, Y);

		// check number of variables in original function
		ok &= (F.size_var() ==  1 + n + m + 4 );

		CppAD::vector<double> x(n), y(m);
		for(j = 0; j < n; j++)
			x[j] = double(j + 2);

		y   = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= ( y[i] == Value( Y[i] ) );

		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// check number of variables  in optimized version
		ok &= (F.size_var() == 1 + n + m + 2 );

		y   = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= ( y[i] == Value( Y[i] ) );

		return ok;
	}
	// -------------------------------------------------------------------
	bool duplicate_four(void)
	{	// Check that unary expression matching not only checks hash code,
		// and operator, but also operand (old bug that has been fixed).
		bool ok = true;
		using CppAD::AD;
		size_t j;

		// domain space vector
		size_t n  = 1;
		CppAD::vector< AD<double> > X(n);
		X[0] = 1.;

		// range space vector
		size_t m = 1;
		CppAD::vector< AD<double> > Y(m);

		// declare independent variables and start tape recording
		CppAD::Independent(X);

		// check a huge number of same operation with different operands
		size_t n_operations = std::min(
			size_t(CPPAD_HASH_TABLE_SIZE) + 5,
			size_t(std::numeric_limits<CPPAD_TAPE_ADDR_TYPE>::max()) - 5
		);
		Y[0] = X[0];
		for(j = 0; j < n_operations; j++)
			Y[0] = abs(Y[0]);

		// create f: X -> Y and stop tape recording
		CppAD::ADFun<double> F;
		F.Dependent(X, Y);

		// check number of variables in original function
		ok &= (F.size_var() ==  1 + n + n_operations );

		CppAD::vector<double> x(n), y(m);
		x[0] = 1.;

		y   = F.Forward(0, x);
		ok &= ( y[0] == Value( Y[0] ) );

		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// check same number of variables in optimized version
		ok &= (F.size_var() == 1 + n + n_operations );

		y   = F.Forward(0, x);
		ok &= ( y[0] == Value( Y[0] ) );

		return ok;
	}
	// ====================================================================
	bool cummulative_sum(void)
	{	// test conversion of a sequence of additions and subtraction
		// to a cummulative summation sequence.
		bool ok = true;
		using CppAD::AD;
		size_t i, j;

		// domain space vector
		size_t n  = 7;
		CppAD::vector< AD<double> > X(n);
		for(j = 0; j < n; j++)
			X[j] = double(j + 2);

		size_t n_original = 1 + n;
		size_t n_optimize = 1 + n;

		// range space vector
		size_t m = 2;
		CppAD::vector< AD<double> > Y(m);

		// declare independent variables and start tape recording
		CppAD::Independent(X);

		// Operations inside of optimize_cadd
		Y[0] = 5. + (X[0] + X[1]) + (X[1] - X[2]) // Addvv, Subvv
		     + (X[2] - 1.) + (2. - X[3])   // Subvp, Subpv
		     + (X[4] + 3.) + (4. + X[5]);  // Addpv, Addpv (no Addvp)
		n_original += 12;
		n_optimize += 1;


		// Operations inside of optimize_csub
		Y[1] = 5. - (X[1] + X[2]) - (X[2] - X[3]) // Addvv, Subvv
		     - (X[3] - 1.) - (2. - X[4])   // Subvp, Subpv
		     - (X[5] + 3.) - (4. + X[6]);  // Addpv, Addpv (no Addvp)
		n_original += 12;
		n_optimize += 1;

		CppAD::ADFun<double> F;
		F.Dependent(X, Y);

		// check number of variables in original function
		ok &= (F.size_var() ==  n_original );

		CppAD::vector<double> x(n), y(m);
		for(j = 0; j < n; j++)
			x[j] = double(j + 2);

		y   = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= ( y[i] == Value( Y[i] ) );

		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// check number of variables  in optimized version
		ok &= (F.size_var() == n_optimize );

		y   = F.Forward(0, x);
		for(i = 0; i < m; i++)
			ok &= ( y[i] == Value( Y[i] ) );

		return ok;
	}
	// -------------------------------------------------------------------
	bool forward_csum(void)
	{	bool ok = true;

		using namespace CppAD;

		// independent variable vector
		CppAD::vector< AD<double> > X(2);
		X[0] = 0.;
		X[1] = 1.;
		Independent(X);

		// compute sum of elements in X
		CppAD::vector< AD<double> > Y(1);
		Y[0] = X[0] + X[0] + X[1];

		// create function object F : X -> Y
		ADFun<double> F(X, Y);

		// now optimize the operation sequence
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// use zero order to evaluate F[ (3, 4) ]
		CppAD::vector<double>  x0( F.Domain() );
		CppAD::vector<double>  y0( F.Range() );
		x0[0]    = 3.;
		x0[1]    = 4.;
		y0       = F.Forward(0, x0);
		ok      &= NearEqual(y0[0] , x0[0]+x0[0]+x0[1], eps_, eps_);

		// evaluate derivative of F in X[0] direction
		CppAD::vector<double> x1( F.Domain() );
		CppAD::vector<double> y1( F.Range() );
		x1[0]    = 1.;
		x1[1]    = 0.;
		y1       = F.Forward(1, x1);
		ok      &= NearEqual(y1[0] , x1[0]+x1[0]+x1[1], eps_, eps_);

		// evaluate second derivative of F in X[0] direction
		CppAD::vector<double> x2( F.Domain() );
		CppAD::vector<double> y2( F.Range() );
		x2[0]       = 0.;
		x2[1]       = 0.;
		y2          = F.Forward(2, x2);
		double F_00 = 2. * y2[0];
		ok         &= NearEqual(F_00, 0., eps_, eps_);

		return ok;
	}
	// -------------------------------------------------------------------
	bool reverse_csum(void)
	{	bool ok = true;

		using namespace CppAD;

		// independent variable vector
		CppAD::vector< AD<double> > X(2);
		X[0] = 0.;
		X[1] = 1.;
		Independent(X);

		// compute sum of elements in X
		CppAD::vector< AD<double> > Y(1);
		Y[0] = X[0] - (X[0] - X[1]);

		// create function object F : X -> Y
		ADFun<double> F(X, Y);

		// now optimize the operation sequence
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// use zero order to evaluate F[ (3, 4) ]
		CppAD::vector<double>  x0( F.Domain() );
		CppAD::vector<double>  y0( F.Range() );
		x0[0]    = 3.;
		x0[1]    = 4.;
		y0       = F.Forward(0, x0);
		ok      &= NearEqual(y0[0] , x0[0]-x0[0]+x0[1], eps_, eps_);

		// evaluate derivative of F
		CppAD::vector<double> dF( F.Domain() );
		CppAD::vector<double> w( F.Range() );
		w[0]    = 1.;
		dF      = F.Reverse(1, w);
		ok     &= NearEqual(dF[0] , 0., eps_, eps_);
		ok     &= NearEqual(dF[1] , 1., eps_, eps_);

		return ok;
	}
	bool forward_sparse_jacobian()
	{	bool ok = true;
		using namespace CppAD;

		// dimension of the domain space
		size_t n = 3;

		// dimension of the range space
		size_t m = 3;

		// independent variable vector
		CppAD::vector< AD<double> > X(n);
		X[0] = 2.;
		X[1] = 3.;
		X[2] = 4.;
		Independent(X);

		// dependent variable vector
		CppAD::vector< AD<double> > Y(m);

		// check results vector
		CppAD::vector< bool >       Check(m * n);

		// initialize index into Y
		size_t index = 0;

		// Y[0]
		Y[index]             = X[0] + X[1] + 5.;
		Check[index * n + 0] = true;
		Check[index * n + 1] = true;
		Check[index * n + 2] = false;
		index++;

		// Y[1]
		Y[index]             = Y[0] - (X[1] + X[2]);
		Check[index * n + 0] = true;
		Check[index * n + 1] = true;
		Check[index * n + 2] = true;
		index++;

		// Y[2]
		// 2DO: There is a subtitle issue that has to do with using reverse
		// jacobian sparsity patterns during the optimization process.
		// We need an option to include X[0] in the sparsity pattern
		// so the optimizer can know it affects the results.
		Y[index]             = CondExpLe(X[0], X[1], X[1]+X[1], X[2]-X[2]);
		Check[index * n + 0] = false;
		Check[index * n + 1] = true;
		Check[index * n + 2] = true;
		index++;

		// check final index
		assert( index == m );

		// create function object F : X -> Y
		ADFun<double> F(X, Y);
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// ---------------------------------------------------------
		// dependency matrix for the identity function
		CppAD::vector< std::set<size_t> > Sx(n);
		size_t i, j;
		for(i = 0; i < n; i++)
		{	assert( Sx[i].empty() );
			Sx[i].insert(i);
		}

		// evaluate the dependency matrix for F(x)
		CppAD::vector< std::set<size_t> > Sy(m);
		Sy = F.ForSparseJac(n, Sx);

		// check values
		bool found;
		for(i = 0; i < m; i++)
		{	for(j = 0; j < n; j++)
			{	found = Sy[i].find(j) != Sy[i].end();
				ok &= (found == Check[i * n + j]);
			}
		}

		return ok;
	}
	bool reverse_sparse_jacobian()
	{	bool ok = true;
		using namespace CppAD;

		// dimension of the domain space
		size_t n = 3;

		// dimension of the range space
		size_t m = 3;

		// independent variable vector
		CppAD::vector< AD<double> > X(n);
		X[0] = 2.;
		X[1] = 3.;
		X[2] = 4.;
		Independent(X);

		// dependent variable vector
		CppAD::vector< AD<double> > Y(m);

		// check results vector
		CppAD::vector< bool >       Check(m * n);

		// initialize index into Y
		size_t index = 0;

		// Y[0]
		Y[index]             = X[0] + X[1] + 5.;
		Check[index * n + 0] = true;
		Check[index * n + 1] = true;
		Check[index * n + 2] = false;
		index++;

		// Y[1]
		Y[index]             = Y[0] - (X[1] + X[2]);
		Check[index * n + 0] = true;
		Check[index * n + 1] = true;
		Check[index * n + 2] = true;
		index++;

		// Y[2]
		Y[index]             = CondExpLe(X[0], X[1], X[1]+X[1], X[2]-X[2]);
		Check[index * n + 0] = false;
		Check[index * n + 1] = true;
		Check[index * n + 2] = true;
		index++;

		// check final index
		assert( index == m );

		// create function object F : X -> Y
		ADFun<double> F(X, Y);
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// ----------------------------------------------------------
		// dependency matrix for the identity function
		CppAD::vector< bool > Py(m * m);
		size_t i, j;
		for(i = 0; i < m; i++)
		{	for(j = 0; j < m; j++)
				Py[ i * m + j ] = (i == j);
		}

		// evaluate the dependency matrix for F(x)
		CppAD::vector< bool > Px(m * n);
		Px = F.RevSparseJac(m, Py);

		// check values
		for(i = 0; i < m; i++)
		{	for(j = 0; j < n; j++)
				ok &= (Px[i * n + j] == Check[i * n + j]);
		}

		return ok;
	}
	bool reverse_sparse_hessian(void)
	{	bool ok = true;
		using CppAD::AD;
		size_t i, j;

		size_t n = 3;
		CppAD::vector< AD<double> > X(n);
		X[0] = 1.;
		X[1] = 2.;
		X[2] = 3.;
		CppAD::Independent(X);

		size_t m = 1;
		CppAD::vector< AD<double> > Y(m);
		Y[0] = CondExpGe( X[0], X[1],
			X[0] + (2. + X[1] + 3.) * X[1],
			X[0] + (2. + X[2] + 3.) * X[1]
		);

		CppAD::vector<bool> check(n * n);
		check[0 * n + 0] = false; // partial w.r.t. x[0], x[0]
		check[0 * n + 1] = false; //                x[0], x[1]
		check[0 * n + 2] = false; //                x[0], x[2]

		check[1 * n + 0] = false; // partial w.r.t. x[1], x[0]
		check[1 * n + 1] = true;  //                x[1], x[1]
		check[1 * n + 2] = true;  //                x[1], x[2]

		check[2 * n + 0] = false; // partial w.r.t. x[2], x[0]
		check[2 * n + 1] = true;  //                x[2], x[1]
		check[2 * n + 2] = false; //                x[2], x[2]

		// create function object F : X -> Y
		CppAD::ADFun<double> F(X, Y);
		if( conditional_skip_ )
			F.optimize();
		else
			F.optimize("no_conditional_skip");

		// sparsity pattern for the identity function U(x) = x
		CppAD::vector<bool> Px(n * n);
		for(i = 0; i < n; i++)
			for(j = 0; j < n; j++)
				Px[ i * n + j ] = (i == j);

		// compute sparsity pattern for Jacobian of F(U(x))
		CppAD::vector<bool> P_jac(m * n);
		P_jac = F.ForSparseJac(n, Px);

		// compute sparsity pattern for Hessian of F_k ( U(x) )
		CppAD::vector<bool> Py(m);
		CppAD::vector<bool> Pxx(n * n);
		Py[0] = true;
		Pxx = F.RevSparseHes(n, Py);
		// check values
		for(i = 0; i < n * n; i++)
			ok &= (Pxx[i] == check[i]);

		return ok;
	}
	// check that CondExp properly detects dependencies
	bool cond_exp_depend(void)
	{	bool ok = true;
		using CppAD::AD;

		AD<double> zero(0.), one(1.), two(2.), three(3.);

		size_t n = 4;
		CppAD::vector< AD<double> > X(n);
		X[0] = zero;
		X[1] = one;
		X[2] = two;
		X[3] = three;
		CppAD::Independent(X);

		size_t m = 4;
		CppAD::vector< AD<double> > Y(m);
		Y[0] = CondExpLt(X[0] + .5,  one,  two, three);
		Y[1] = CondExpLt(zero, X[1] + .5,  two, three);
		Y[2] = CondExpLt(zero,  one, X[2] + .5, three);
		Y[3] = CondExpLt(zero,  one,  two,  X[3] + .5);

		CppAD::ADFun<double> f(X, Y);
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		CppAD::vector<double> x(n), y(m);
		size_t i;
		for(i = 0; i < n; i++)
			x[i] = double(n - i);
		y    = f.Forward(0, x);

		if( x[0] + .5 < 1. )
			ok  &= y[0] == 2.;
		else	ok  &= y[0] == 3.;
		if( 0. < x[1] + .5 )
			ok  &= y[1] == 2.;
		else	ok  &= y[1] == 3.;
		ok  &= y[2] == x[2] + .5;;
		ok  &= y[3] == 2.;

		return ok;
	}
	// check that CondExp properly handels expressions that get
	// removed during opitmization
	bool cond_exp_removed(void)
	{	bool ok = true;
		using CppAD::AD;
		AD<double> zero(0.);

		size_t n = 1;
		CppAD::vector< AD<double> > X(n);
		X[0] = 1.0;
		CppAD::Independent(X);

		size_t m = 1;
		CppAD::vector< AD<double> > Y(m);

		AD<double> true_case  = sin(X[0]) + sin(X[0]);
		AD<double> false_case = cos(X[0]) + cos(X[0]);
		Y[0] = CondExpLt(X[0],  zero,  true_case, false_case);

		CppAD::ADFun<double> f(X, Y);
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		CppAD::vector<double> x(n), y(m), w(m), dw(n);
		x[0] = 1.0;
		y    = f.Forward(0, x);
		ok &= NearEqual(y[0], false_case, eps_, eps_);

		w[0] = 1.0;
		dw   = f.Reverse(1, w);
		// derivative of cos is minus sin
		ok &= NearEqual(dw[0], - true_case, eps_, eps_);

		return ok;
	}
	// -------------------------------------------------------------------
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

	bool old_atomic_forward(
		size_t                         id ,
		size_t                          k ,
		size_t                          n ,
		size_t                          m ,
		const CppAD::vector<bool>&     vx ,
		CppAD::vector<bool>&           vy ,
		const CppAD::vector<double>&   tx ,
		CppAD::vector<double>&         ty )
	{	assert(n == 3 && m == 2);
		if( k > 0 )
			return false;

		// y[0] = x[0] + x[1]
		ty[0] = tx[0] + tx[1];

		// y[1] = x[1] + x[2]
		ty[1] = tx[1] + tx[2];

		if( vy.size() > 0 )
		{	vy[0] = (vx[0] | vx[1]);
			vy[1] = (vx[1] | vx[2]);
		}
		return true;
	}

	bool old_atomic_reverse(
		size_t                         id ,
		size_t                          k ,
		size_t                          n ,
		size_t                          m ,
		const CppAD::vector<double>&   tx ,
		const CppAD::vector<double>&   ty ,
		CppAD::vector<double>&         px ,
		const CppAD::vector<double>&   py )
	{	return false; }

	bool old_atomic_for_jac_sparse(
		size_t                                  id ,
		size_t                                   n ,
		size_t                                   m ,
		size_t                                   q ,
		const CppAD::vector< std::set<size_t> >& r ,
		CppAD::vector< std::set<size_t>  >&      s )
	{	return false; }

	bool old_atomic_rev_jac_sparse(
		size_t                                  id ,
		size_t                                   n ,
		size_t                                   m ,
		size_t                                   q ,
		CppAD::vector< std::set<size_t> >&       r ,
		const CppAD::vector< std::set<size_t> >& s )
	{	assert(n == 3 && m == 2);
		r[0].clear();
		r[1].clear();
		r[2].clear();
		// y[0] = x[0] + x[1]
		my_union(r[0], r[0], s[0]);
		my_union(r[1], r[1], s[0]);
		// y[1] = x[1] + x[2]
		my_union(r[1], r[1], s[1]);
		my_union(r[2], r[2], s[1]);

		return true;
	}

	bool old_atomic_rev_hes_sparse(
		size_t                                  id ,
		size_t                                   n ,
		size_t                                   m ,
		size_t                                   q ,
		const CppAD::vector< std::set<size_t> >& r ,
		const CppAD::vector<bool>&               s ,
		CppAD::vector<bool>&                     t ,
		const CppAD::vector< std::set<size_t> >& u ,
		CppAD::vector< std::set<size_t> >&       v )
	{	return false; }

	CPPAD_USER_ATOMIC(
		my_old_atomic             ,
		CppAD::vector              ,
		double                     ,
		old_atomic_forward        ,
		old_atomic_reverse        ,
		old_atomic_for_jac_sparse ,
		old_atomic_rev_jac_sparse ,
		old_atomic_rev_hes_sparse
	)

	bool old_atomic_test(void)
	{	bool ok = true;

		using CppAD::AD;
		size_t j;
		size_t n = 3;
		size_t m = 2;
		CppAD::vector< AD<double> > ax(n), ay(m), az(m);
		for(j = 0; j < n; j++)
			ax[j] = AD<double>(j + 1);
		CppAD::Independent(ax);

		size_t id = 0;
		// first call should stay in the tape
		my_old_atomic(id++, ax, ay);
		// second call will not get used
		my_old_atomic(id++, ax, az);
		// create function
		CppAD::ADFun<double> g(ax, ay);
		// should have 1 + n + m + m varaibles
		ok &= g.size_var() == (1 + n + m + m);
		g.optimize();
		// should have 1 + n + m varaibles
		ok &= g.size_var() == (1 + n + m);

		// now test that the optimized function gives same results
		CppAD::vector<double> x(n), y(m);
		for(j = 0; j < n; j++)
			x[j] = (j + 1) * (j + 1);
		y = g.Forward(0, x);
		// y[0] = x[0] + x[1]
		ok &= (y[0] == x[0] + x[1]);
		// y[1] = x[1] + x[2]
		ok &= (y[0] == x[0] + x[1]);

		return ok;
	}
	bool not_identically_equal(void)
	{	bool ok = true;
		using CppAD::AD;

		// independent variable vector
		size_t n = 5;
		CppAD::vector< AD<double> > ax(n);
		size_t j;
		for(j = 0; j < n; j++)
			ax[j] = 1. / 3.;
		CppAD::Independent(ax);

		// dependent variable vector
		size_t m = 1;
		CppAD::vector< AD<double> > ay(m);
		ay[0]       = 0.;
		for(j = 0; j < n; j++)
		{	if( j % 2 == 0 )
				ay[0] += ax[j];
			else	ay[0] -= ax[j];
		}
		CppAD::ADFun<double> f(ax, ay);

		// Used to fail assert in optimize that forward mode results
		// are identically equal
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		return ok;
	}
	// -----------------------------------------------------------------------
	double floor(const double& x)
	{	return std::floor(x); }
	CPPAD_DISCRETE_FUNCTION(double, floor)
	bool discrete_function(void)
	{	bool ok = true;
		using CppAD::vector;

		vector< CppAD::AD<double> > ax(1), ay(1);
		ax[0] = 0.0;
		CppAD::Independent(ax);
		ay[0] =  floor(ax[0]) + floor(ax[0]);
		CppAD::ADFun<double> f(ax, ay);

		size_t size_before = f.size_var();
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");
		size_t size_after = f.size_var();
		ok &= size_after + 1 == size_before;

		vector<double> x(1), y(1);
		x[0] = -2.2;
		y    = f.Forward(0, x);
		ok &= y[0] == -6.0;

		return ok;
	}
	// ----------------------------------------------------------------
	void i_algo(
		const CppAD::vector< CppAD::AD<double> >& ax ,
		      CppAD::vector< CppAD::AD<double> >& ay )
	{	ay[0] = 1.0 / ax[0]; }
	//
	// Test bug where atomic functions were not properly conditionally skipped.
	bool cond_exp_skip_atomic(void)
	{	bool ok = true;
		using CppAD::AD;
		using CppAD::vector;

		// Create a checkpoint version of the function i_algo
		vector< AD<double> > au(1), av(1), aw(1);
		au[0] = 1.0;
		CppAD::checkpoint<double> i_check("i_check", i_algo, au, av);

		// independent variable vector
		vector< AD<double> > ax(2), ay(1);
		ax[0] = 1.0;
		ax[1] = 2.0;
		Independent(ax);

		// call atomic function that does not get used
		au[0] = ax[0];
		i_check(au, av);
		au[0] = ax[1];
		i_check(au, aw);
		AD<double> zero = 0.0;
		ay[0] = CondExpGt(av[0], zero, av[0], aw[0]);

		// create function object f : ax -> ay
		CppAD::ADFun<double> f(ax, ay);

		// run case that skips the second call to afun
		// (can use trace in forward0sweep.hpp to see this).
		vector<double> x(2), y_before(1), y_after(1);
		x[0]      = 1.0;
		x[1]      = 2.0;
		y_before  = f.Forward(0, x);
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");
		y_after   = f.Forward(0, x);

		ok &= y_before[0] == y_after[0];

		return ok;
	}
	//
	// Test bug where conditional dependence did not pass through
	// atomic functions
	bool cond_exp_atomic_dependence(void)
	{	bool ok = true;
		using CppAD::AD;
		using CppAD::vector;

		// Create a checkpoint version of the function i_algo
		vector< AD<double> > au(1), av(1), aw(1);
		au[0] = 1.0;
		CppAD::checkpoint<double> i_check("i_check", i_algo, au, av);

		vector< AD<double> > ax(2), ay(1);
		AD<double> zero = 0.0;
		ax[0] = 1.0;
		ax[1] = 1.0;
		Independent(ax);
		av[0] = ax[0] + ax[1];
		i_check(av, aw);
		ay[0] = CondExpGt(aw[0], zero, zero, aw[0]);
		CppAD::ADFun<double> f;
		f.Dependent(ax, ay);

		// run case that skips the second call to afun
		// (but not for order zero)
		vector<double> x(2), y_before(1), y_after(1);
		vector<double> dx(2), dy_before(1), dy_after(1);
		x[0]      = 1.0;
		x[1]      = 1.0;
		y_before  = f.Forward(0, x);
		dx[0]     = 2.0;
		dx[1]     = 2.0;
		dy_before = f.Forward(1, dx);
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");
		y_after   = f.Forward(0, x);
		dy_after  = f.Forward(1, dx);

		ok &= y_before[0]  == y_after[0];
		ok &= dy_before[0] == dy_after[0];

		return ok;
	}
	// -----------------------------------------------------------------------
	// Test reverse mode conditionalay skipping commands.
	template <class Type>
	Type my_max(const CppAD::vector<Type>& arg)
	{	Type res = arg[0];
		for(size_t j = 0;j < arg.size(); j++)
		res = CondExpGt(res, arg[j], res, arg[j]);
		return res;
	}
	bool cond_exp_reverse(void)
	{	bool ok = true;
		size_t n = 3;
		using CppAD::vector;
		using CppAD::AD;

		vector< AD<double> > ax(n), ay(1);
		for(size_t j = 0; j < n; j++)
			ax[j] = 1.0;
		Independent(ax);
		ay[0] = my_max(ax) + my_max(ax);
		CppAD::ADFun<double> f(ax, ay);

		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		vector<double> x(n), w(1), dx(n);
		for(size_t j = 0;j < n; j++)
			x[j] = double(j);
		f.Forward(0, x);
		w[0] = 1.0;
		dx = f.Reverse(1, w);
		for(size_t j = 0; j < n; j++)
		{	if( j == n-1 )
				ok &= dx[j] == 2.0;
			else
				ok &= dx[j] == 0.0;
		}
		return ok;
	}
	// Test case where an expression depends on both the true
	// and false cases (bug fixed 2014-12-22)
	bool cond_exp_both_true_and_false(void)
	{	bool ok = true;
		using CppAD::vector;
		using CppAD::AD;

		// f(x) = x[0] + x[0] if x[0] >= 3
		//      = x[0] + x[1] otherwise
		vector< AD<double> > ax(2), ay(3);
		ax[0] = 1.0;
		ax[1] = 2.0;
		Independent(ax);
		AD<double> three(3);
		AD<double> value = ax[0] + ax[1];
		// a simple value
		ay[0]  = CppAD::CondExpGe(ax[0], three, value, value);
		// a  binary exprpression
		ay[1]  = CppAD::CondExpGe(ax[0], three, ax[0]-ax[1], ax[0]-ax[1]);
		// a unary expression
		ay[2]  = CppAD::CondExpGe(ax[0], three, exp(ax[0]), exp(ax[0]) );
		CppAD::ADFun<double> f(ax, ay);
		if( conditional_skip_ )
			f.optimize();
		else
			f.optimize("no_conditional_skip");

		// check case where x[0] >= 3
		vector<double> x(2), y(3);
		x[0] = 4.0;
		x[1] = 2.0;
		y    = f.Forward(0, x);
		ok  &= y[0] == x[0] + x[1];
		ok  &= y[1] == x[0] - x[1];
		ok  &= y[2] == exp(x[0]);

		// check case where x[0] < 3
		x[0] = 1.0;
		x[1] = 2.0;
		y    = f.Forward(0, x);
		ok  &= y[0] == x[0] + x[1];
		ok  &= y[1] == x[0] - x[1];
		ok  &= y[2] == exp(x[0]);

		return ok;
	}
}

bool optimize(void)
{	bool ok = true;
	conditional_skip_       = true;
	atomic_sparsity_option_ = CppAD::atomic_base<double>::bool_sparsity_enum;

	// atomic sparsity loop
	for(size_t i = 0; i < 3; i++)
	{	if( i == 0 ) atomic_sparsity_option_ =
			CppAD::atomic_base<double>::pack_sparsity_enum;
		else if( i == 1 ) atomic_sparsity_option_ =
			CppAD::atomic_base<double>::bool_sparsity_enum;
		else if( i == 2 ) atomic_sparsity_option_ =
			CppAD::atomic_base<double>::set_sparsity_enum;
		else
			ok &= false;
		//
		// check conditional expression sparsity pattern
		// (used to optimize calls to atomic functions).
		ok     &= atomic_cond_exp_sparsity();
		// check optimizing out entire atomic function
		ok     &= atomic_cond_exp();
		// check optimizing out atomic arguments
		ok     &= atomic_no_used();
		ok     &= atomic_arguments();
	}

	// conditional skip loop
	for(size_t i = 0; i < 2; i++)
	{	conditional_skip_ = i == 0;
		//
		// check nested conditional expressions
		ok     &= nested_cond_exp();
		// check reverse dependency analysis optimization
		ok     &= depend_one();
		ok     &= depend_two();
		ok     &= depend_three();
		ok     &= depend_four();
		// check removal of duplicate expressions
		ok     &= duplicate_one();
		ok     &= duplicate_two();
		ok     &= duplicate_three();
		ok     &= duplicate_four();
		// convert sequence of additions to cummulative summation
		ok     &= cummulative_sum();
		ok     &= forward_csum();
		ok     &= reverse_csum();
		// sparsity patterns
		ok     &= forward_sparse_jacobian();
		ok     &= reverse_sparse_jacobian();
		ok     &= reverse_sparse_hessian();
		// check that CondExp properly detects dependencies
		ok     &= cond_exp_depend();
		// check that it properly handles expressions that have been removed
		ok     &= cond_exp_removed();
		// check old_atomic functions
		ok     &= old_atomic_test();
		// case where results are not identically equal
		ok     &= not_identically_equal();
		// case where a discrete function is used
		ok     &= discrete_function();
		// check conditional skip of an atomic function
		ok     &= cond_exp_skip_atomic();
		// check conditional dependence through atomic function
		ok     &= cond_exp_atomic_dependence();
		// check reverse mode conditional skipping
		ok     &= cond_exp_reverse();
		// check case where an expresion needed by both true and false case
		ok     &=  cond_exp_both_true_and_false();
	}
	//
	CppAD::user_atomic<double>::clear();
	return ok;
}
