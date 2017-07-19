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
#include <Reports/Children/Addressable.h>
#include "Factory.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Reports/Manager.h"
#include "Reports/Children/AgeingErrorMatrix.h"
#include "Reports/Children/CategoryInfo.h"
#include "Reports/Children/Catchabilities.h"
#include "Reports/Children/CategoryList.h"
#include "Reports/Children/CovarianceMatrix.h"
#include "Reports/Children/CorrelationMatrix.h"
#include "Reports/Children/DerivedQuantity.h"
#include "Reports/Children/EquationTest.h"
#include "Reports/Children/EstimateSummary.h"
#include "Reports/Children/EstimateValue.h"
#include "Reports/Children/HessianMatrix.h"
#include "Reports/Children/InitialisationPartition.h"
#include "Reports/Children/InitialisationPartitionMeanWeight.h"
#include "Reports/Children/MCMCCovariance.h"
#include "Reports/Children/MCMCObjective.h"
#include "Reports/Children/MCMCSample.h"
#include "Reports/Children/MPD.h"
#include "Reports/Children/ObjectiveFunction.h"
#include "Reports/Children/Observation.h"
#include "Reports/Children/Partition.h"
#include "Reports/Children/PartitionBiomass.h"
#include "Reports/Children/PartitionMeanWeight.h"
#include "Reports/Children/Process.h"
#include "Reports/Children/Project.h"
#include "Reports/Children/RandomNumberSeed.h"
#include "Reports/Children/OutputParameters.h"
#include "Reports/Children/SimulatedObservation.h"
#include "Reports/Children/Selectivity.h"
#include "Reports/Children/TimeVarying.h"


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
    else if (sub_type == PARAM_AGEING_ERROR)
      result = new AgeingErrorMatrix(model);
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
    else if (sub_type == PARAM_INITIALISATION_PARTITION)
      result = new InitialisationPartition(model);
    else if (sub_type == PARAM_INITIALISATION_PARTITION_MEAN_WEIGHT)
      result = new InitialisationPartitionMeanWeight(model);
    else if (sub_type == PARAM_MCMC_COVARIANCE)
      result = new MCMCCovariance(model);
    else if (sub_type == PARAM_MCMC_OBJECTIVE)
      result = new MCMCObjective(model);
    else if (sub_type == PARAM_MCMC_SAMPLE)
      result = new MCMCSample(model);
    else if (sub_type == PARAM_MPD)
      result = new MPD(model);
    else if (sub_type == PARAM_PARTITION)
      result = new Partition(model);
    else if (sub_type == PARAM_PARTITION_BIOMASS)
      result = new PartitionBiomass(model);
    else if (sub_type == PARAM_PARTITION_MEAN_WEIGHT)
      result = new PartitionMeanWeight(model);
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

    if (result)
      model->managers().report()->AddObject(result);
  }

  return result;
}

} /* namespace reports */
} /* namespace niwa */
