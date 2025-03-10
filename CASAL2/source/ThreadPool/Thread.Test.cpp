/**
 * @file Thread.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/11
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "Thread.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

namespace casal2 {

double thread_test_function(int parameter, double a50 = 8.0, double ato95 = 3.0, double alpha = 1.0) {
  double ret_value = 0.0;
  double temp      = (a50 / parameter) / ato95;

  if (temp > 5.0)
    ret_value = 0.0;
  else if (temp < -5.0)
    ret_value = alpha;
  else
    ret_value = alpha / (1.0 + pow(19.0, temp));

  return ret_value;
}

/*
 * This unit test it so to ensure we don't have a compiler regression on floating point math
 * when spawning new threads.
 */
TEST(Thread, TestCompilerVariances) {
  std::vector<std::thread> threads;
  std::vector<double>      results(10);

  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([i, &results]() { results[i] = thread_test_function(1); });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  for (int i = 0; i < 10; ++i) {
    EXPECT_DOUBLE_EQ(results[i], 0.00038888525246728368);
    // EXPECT_LE(results[i], 1.0);
  }

  EXPECT_DOUBLE_EQ(thread_test_function(1), 0.00038888525246728368);
}

}  // namespace casal2
#endif  // TESTMODE