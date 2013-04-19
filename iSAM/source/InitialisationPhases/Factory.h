/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef INITIALISATIONPHASES_FACTORY_H_
#define INITIALISATIONPHASES_FACTORY_H_

// Headers
#include "BaseClasses/Factory.h"
#include "InitialisationPhases/InitialisationPhase.h"
#include "InitialisationPhases/Manager.h"

// Namespaces
namespace isam {
namespace initialisationphases {

/**
 * Class Definition
 */
class Factory : public isam::base::Factory<isam::InitialisationPhase, isam::initialisationphases::Manager> {};

} /* namespace initialisationphases */
} /* namespace isam */




#endif /* INITIALISATIONPHASES_FACTORY_H_ */
