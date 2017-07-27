/**
 * @file Model.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
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
#ifndef MOCK_MODEL_H_
#define MOCK_MODEL_H_

// Headers
#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"

// Namespaces
namespace niwa {

/**
 * Class Definition
 */
class MockModel : public niwa::Model {
public:
  MOCK_CONST_METHOD0(min_age, unsigned());
  MOCK_CONST_METHOD0(max_age, unsigned());
  MOCK_CONST_METHOD0(age_spread, unsigned());
  MOCK_CONST_METHOD0(start_year, unsigned());
  MOCK_CONST_METHOD0(final_year, unsigned());
  MOCK_CONST_METHOD0(age_plus, bool());
  MOCK_CONST_METHOD0(current_year, unsigned());
  MOCK_CONST_METHOD0(time_steps, vector<string>&());
  MOCK_CONST_METHOD0(years, vector<unsigned>());
  MOCK_METHOD0(managers, niwa::Managers&());
};

} /* namespace niwa */

#endif /* MOCK_MODEL_H_ */
