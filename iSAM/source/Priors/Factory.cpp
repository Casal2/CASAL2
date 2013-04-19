/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
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

// Headers
#include "Factory.h"
#include "Priors/Manager.h"
#include "Priors/Children/Beta.h"
#include "Priors/Children/LogNormal.h"
#include "Priors/Children/Normal.h"
#include "Priors/Children/NormalByStdev.h"
#include "Priors/Children/NormalLog.h"
#include "Priors/Children/Uniform.h"
#include "Priors/Children/UniformLog.h"

// Namespaces
namespace isam {
namespace priors {

/**
 *
 */
PriorPtr Factory::Create(const string& block_type, const string& prior_type) {

  PriorPtr prior;

  if (block_type == PARAM_PRIOR) {
    if (prior_type == PARAM_BETA)
      prior = PriorPtr(new Beta());
    else if (prior_type == PARAM_LOG_NORMAL)
      prior = PriorPtr(new LogNormal());
    else if (prior_type == PARAM_NORMAL)
      prior = PriorPtr(new Normal());
    else if (prior_type == PARAM_NORMAL_BY_STDEV)
      prior = PriorPtr(new NormalByStdev());
    else if (prior_type == PARAM_NORMAL_LOG)
      prior = PriorPtr(new NormalLog());
    else if (prior_type == PARAM_UNIFORM)
      prior = PriorPtr(new Uniform());
    else if (prior_type == PARAM_UNIFORM_LOG)
      prior = PriorPtr(new UniformLog());
  }

  if (prior)
    isam::priors::Manager::Instance().AddObject(prior);

  return prior;
}

} /* namespace priors */
} /* namespace isam */
