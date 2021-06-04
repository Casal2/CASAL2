/**
 * @file BasicModel.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 2/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This test fixture will create a basic model. No default processes will be loaded but it will
 * have some methods to add them quickly and easily
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TESTFIXTURES_BASICMODEL_H_
#define TESTFIXTURES_BASICMODEL_H_
#ifdef TESTMODE

// Headers
#include "../../TestResources/TestFixtures/Base.h"

// Namespaces
namespace niwa {
class Model;

namespace testfixtures {
using std::string;

/**
 * Class Definition
 */
class BasicModel : public testfixtures::Base {
public:
  // Setup and TearDown Methods
  BasicModel(){};
  virtual ~BasicModel(){};
  void SetUp() override final;
};

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
#endif /* TESTFIXTURES_BASICMODEL_H_ */
