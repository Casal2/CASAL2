/**
 * @file MCMC.Mock.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief Mock Class definition for MCMC so we can use it
 * during unit tests
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SOURCE_MCMC_MCMC_MOCK_H_
#define SOURCE_MCMC_MCMC_MOCK_H_
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>

#include "MCMC.h"

// namespaces
namespace niwa {

/**
 * Create our MockObjects Class
 */
class MockMCMC : public niwa::MCMC {
public:
  MockMCMC(shared_ptr<Model> model) : niwa::MCMC(model){};
  virtual ~MockMCMC() = default;

  void DoValidate() final{};
  void DoBuild() final{};
  void DoExecute(shared_ptr<ThreadPool> thread_pool) final{};
};

} /* namespace niwa */

#endif  // TESTMODE
#endif  // SOURCE_MCMC_MCMC_MOCK_H_