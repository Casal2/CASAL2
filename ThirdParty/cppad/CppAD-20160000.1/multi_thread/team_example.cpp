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
$begin team_example.cpp$$
$spell
	CppAD
$$

$section Using a Team of AD Threads: Example and Test$$
$mindex thread$$


$head Purpose$$
This example demonstrates how use a team threads with CppAD.

$head thread_team$$
The following three implementations of the
$cref team_thread.hpp$$ specifications are included:
$table
$rref team_openmp.cpp$$
$rref team_bthread.cpp$$
$rref team_pthread.cpp$$
$tend

$head Source Code$$
$code
$verbatim%multi_thread/team_example.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
------------------------------------------------------------------------------
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include "team_thread.hpp"
# define NUMBER_THREADS  4

namespace {
	using CppAD::thread_alloc;

	// structure with information for one thread
	typedef struct {
		// function argument (worker input)
		double          x;
		// false if an error occurs, true otherwise (worker output)
		bool            ok;
	} work_one_t;
	// vector with information for all threads
	// (use pointers instead of values to avoid false sharing)
	work_one_t* work_all_[NUMBER_THREADS];
	// --------------------------------------------------------------------
	// function that does the work for one thread
	void worker(void)
	{	using CppAD::NearEqual;
		using CppAD::AD;
		bool ok = true;
		size_t thread_num = thread_alloc::thread_num();

		// CppAD::vector uses the CppAD fast multi-threading allocator
		CppAD::vector< AD<double> > ax(1), ay(1);
		ax[0] = work_all_[thread_num]->x;
		Independent(ax);
		ay[0] = sqrt( ax[0] * ax[0] );
		CppAD::ADFun<double> f(ax, ay);

		// Check function value corresponds to the identity
		double eps = 10. * CppAD::numeric_limits<double>::epsilon();
		ok        &= NearEqual(ay[0], ax[0], eps, eps);

		// Check derivative value corresponds to the identity.
		CppAD::vector<double> d_x(1), d_y(1);
		d_x[0] = 1.;
		d_y    = f.Forward(1, d_x);
		ok    &= NearEqual(d_x[0], 1., eps, eps);

		// pass back ok information for this thread
		work_all_[thread_num]->ok = ok;
	}
}

// This test routine is only called by the master thread (thread_num = 0).
bool team_example(void)
{	bool ok = true;

	size_t num_threads = NUMBER_THREADS;

	// Check that no memory is in use or avialable at start
	// (using thread_alloc in sequential mode)
	size_t thread_num;
	for(thread_num = 0; thread_num < num_threads; thread_num++)
	{	ok &= thread_alloc::inuse(thread_num) == 0;
		ok &= thread_alloc::available(thread_num) == 0;
	}

	// initialize work_all_
	for(thread_num = 0; thread_num < num_threads; thread_num++)
	{	// allocate separate memory for this thread to avoid false sharing
		size_t min_bytes(sizeof(work_one_t)), cap_bytes;
		void*  v_ptr = thread_alloc::get_memory(min_bytes, cap_bytes);
		work_all_[thread_num]     = static_cast<work_one_t*>(v_ptr);
		// incase this thread's worker does not get called
		work_all_[thread_num]->ok = false;
		// parameter that defines the work for this thread
		work_all_[thread_num]->x  = double(thread_num) + 1.;
	}

	ok &= team_create(num_threads);
	ok &= team_work(worker);
	ok &= team_destroy();

	// go down so that free memrory for other threads before memory for master
	thread_num = num_threads;
	while(thread_num--)
	{	// check that this thread was ok with the work it did
		ok &= work_all_[thread_num]->ok;
		// delete problem specific information
		void* v_ptr = static_cast<void*>( work_all_[thread_num] );
		thread_alloc::return_memory( v_ptr );
		// check that there is no longer any memory inuse by this thread
		// (for general applications, the master might still be using memory)
		ok &= thread_alloc::inuse(thread_num) == 0;
		// return all memory being held for future use by this thread
		thread_alloc::free_available(thread_num);
	}
	return ok;
}
// END C++
