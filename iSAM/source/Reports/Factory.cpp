/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Reports/Manager.h"
#include "Reports/Children/CategoryInfo.h"
#include "Reports/Children/DerivedQuantity.h"
#include "Reports/Children/EstimateSummary.h"
#include "Reports/Children/EstimateValue.h"
#include "Reports/Children/MCMCChain.h"
#include "Reports/Children/ObjectiveFunction.h"
#include "Reports/Children/Partition.h"

// Namespaces
namespace isam {
namespace reports {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
ReportPtr Factory::Create(const string& object_type, const string& sub_type) {
  ReportPtr result;

  if (object_type == PARAM_REPORT) {
    if (sub_type == PARAM_CATEGORY_INFO)
      result = ReportPtr(new CategoryInfo());
    else if (sub_type == PARAM_DERIVED_QUANTITY)
      result = ReportPtr(new DerivedQuantity());
    else if (sub_type == PARAM_ESTIMATE_SUMMARY)
      result = ReportPtr(new EstimateSummary());
    else if (sub_type == PARAM_ESTIMATE_VALUE)
      result = ReportPtr(new EstimateValue());
    else if (sub_type == PARAM_MCMC_CHAIN)
      result = ReportPtr(new MCMCChain());
    else if (sub_type == PARAM_PARTITION)
      result = ReportPtr(new Partition());
    else if (sub_type == PARAM_OBJECTIVE_FUNCTION)
      result = ReportPtr(new ObjectiveFunction());

    if (result)
      isam::reports::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace reports */
} /* namespace isam */
