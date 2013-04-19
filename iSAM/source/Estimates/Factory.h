/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains the definitions for 2 factory classes
 * Estimate and EstimateInfo Factories
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ESTIMATE_FACTORY_H_
#define ESTIMATE_FACTORY_H_

// Headers
#include "BaseClasses/Factory.h"
#include "Estimates/Manager.h"

namespace isam {
namespace estimates {

class Factory : public isam::base::Factory<isam::Estimate, isam::estimates::Manager> { };

/**
 * EstimateInfo factory
 */
namespace info {
class Factory : public isam::base::Factory<isam::estimates::Info, isam::estimates::Manager> {};
} /* namespace info */

} /* namespace estimates */
} /* namespace isam */

#endif /* FACTORY_H_ */
