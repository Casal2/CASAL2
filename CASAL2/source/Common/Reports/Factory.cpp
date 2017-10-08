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

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Reports/Manager.h"
#include "Common/Reports/Children/Addressable.h"
#include "Common/Reports/Children/CategoryInfo.h"
#include "Common/Reports/Children/Catchabilities.h"
#include "Common/Reports/Children/CategoryList.h"
#include "Common/Reports/Children/CovarianceMatrix.h"
#include "Common/Reports/Children/CorrelationMatrix.h"
#include "Common/Reports/Children/DerivedQuantity.h"
#include "Common/Reports/Children/EquationTest.h"
#include "Common/Reports/Children/EstimateSummary.h"
#include "Common/Reports/Children/EstimateValue.h"
#include "Common/Reports/Children/HessianMatrix.h"
#include "Common/Reports/Children/MCMCCovariance.h"
#include "Common/Reports/Children/MCMCObjective.h"
#include "Common/Reports/Children/MCMCSample.h"
#include "Common/Reports/Children/MPD.h"
#include "Common/Reports/Children/ObjectiveFunction.h"
#include "Common/Reports/Children/Observation.h"
#include "Common/Reports/Children/InitialisationPartition.h"
#include "Common/Reports/Children/Process.h"
#include "Common/Reports/Children/Project.h"
#include "Common/Reports/Children/RandomNumberSeed.h"
#include "Common/Reports/Children/OutputParameters.h"
#include "Common/Reports/Children/SimulatedObservation.h"
#include "Common/Reports/Children/Selectivity.h"
#include "Common/Reports/Children/TimeVarying.h"

// Age Factory
#include "Age/Reports/Factory.h"
// Length Factory
#include "Length/Reports/Factory.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Report* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Report* result = nullptr;

  if (object_type == PARAM_REPORT) {
    if (sub_type == PARAM_CATEGORY_INFO)
      result = new CategoryInfo(model);
    else if (sub_type == PARAM_CATEGORY_LIST)
      result = new CategoryList(model);
    else if (sub_type == PARAM_CATCHABILITY)
      result = new Catchabilities(model);
    else if (sub_type == PARAM_COVARIANCE_MATRIX)
      result = new CovarianceMatrix(model);
    else if (sub_type == PARAM_CORRELATION_MATRIX)
      result = new CorrelationMatrix(model);
    else if (sub_type == PARAM_DERIVED_QUANTITY)
      result = new DerivedQuantity(model);
    else if (sub_type == PARAM_EQUATION)
      result = new EquationTest(model);
    else if (sub_type == PARAM_ADDRESSABLE)
      result = new Addressable(model);
    else if (sub_type == PARAM_ESTIMATE_SUMMARY)
      result = new EstimateSummary(model);
    else if (sub_type == PARAM_ESTIMATE_VALUE)
      result = new EstimateValue(model);
    else if (sub_type == PARAM_HESSIAN_MATRIX)
      result = new HessianMatrix(model);
    else if (sub_type == PARAM_MCMC_COVARIANCE)
      result = new MCMCCovariance(model);
    else if (sub_type == PARAM_MCMC_OBJECTIVE)
      result = new MCMCObjective(model);
    else if (sub_type == PARAM_MCMC_SAMPLE)
      result = new MCMCSample(model);
    else if (sub_type == PARAM_MPD)
      result = new MPD(model);
    else if (sub_type == PARAM_PROJECT)
      result = new Project(model);
    else if (sub_type == PARAM_PROCESS)
      result = new Process(model);
    else if (sub_type == PARAM_OBJECTIVE_FUNCTION)
      result = new ObjectiveFunction(model);
    else if (sub_type == PARAM_OBSERVATION)
      result = new Observation(model);
    else if (sub_type == PARAM_OUTPUT_PARAMETERS)
      result = new OutputParameters(model);
    else if (sub_type == PARAM_RANDOM_NUMBER_SEED)
      result = new RandomNumberSeed(model);
    else if (sub_type == PARAM_SIMULATED_OBSERVATION)
      result = new SimulatedObservation(model);
    else if (sub_type == PARAM_SELECTIVITY)
      result = new Selectivity(model);
    else if (sub_type == PARAM_TIME_VARYING)
      result = new TimeVarying(model);
    else if (sub_type == PARAM_INITIALISATION_PARTITION)
      result = new InitialisationPartition(model);
    else if (model->partition_type() == PartitionType::kAge)
      result = age::reports::Factory::Create(model, object_type, sub_type);
    else if (model->partition_type() == PartitionType::kLength)
      result = length::reports::Factory::Create(model, object_type, sub_type);

    if (result)
      model->managers().report()->AddObject(result);
  }

  return result;
}

} /* namespace reports */
} /* namespace niwa */
