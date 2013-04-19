/**
 * @file EmptyModel.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 9/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TESTFIXTURES_EMPTYMODEL_H_
#define TESTFIXTURES_EMPTYMODEL_H_
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

// Namespaces
namespace isam {
namespace testfixtures {

using std::string;

/**
 * Class Definition
 */
class EmptyModel : public ::testing::Test {
public:
  // Methods
  EmptyModel();
  virtual                     ~EmptyModel();
  virtual void                SetUp();
  virtual void                TearDown();
};

} /* namespace testfixtures */
} /* namespace isam */
#endif /* TESTMODE */
#endif /* TESTFIXTURES_EMPTYMODEL_H_ */
