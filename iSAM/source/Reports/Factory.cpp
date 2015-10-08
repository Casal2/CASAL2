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

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Reports/Manager.h"
#include "Reports/Children/AgeingErrorMatrix.h"
#include "Reports/Children/CategoryInfo.h"
#include "Reports/Children/CategoryList.h"
#include "Reports/Children/CovarianceMatrix.h"
#include "Reports/Children/DerivedQuantity.h"
#include "Reports/Children/Estimable.h"
#include "Reports/Children/EstimateSummary.h"
#include "Reports/Children/EstimateValue.h"
#include "Reports/Children/InitialisationPartition.h"
#include "Reports/Children/MCMCChain.h"
#include "Reports/Children/MCMCCovariance.h"
#include "Reports/Children/ObjectiveFunction.h"
#include "Reports/Children/Observation.h"
#include "Reports/Children/Partition.h"
#include "Reports/Children/PartitionBiomass.h"
#include "Reports/Children/PartitionMeanWeight.h"
#include "Reports/Children/RandomNumberSeed.h"
#include "Reports/Children/SimulatedObservation.h"
#include "Reports/Children/Selectivity.h"

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
      result = new CategoryInfo();
    else if (sub_type == PARAM_AGEING_ERROR)
      result = new AgeingErrorMatrix();
    else if (sub_type == PARAM_CATEGORY_LIST)
      result = new CategoryList();
    else if (sub_type == PARAM_COVARIANCE_MATRIX)
      result = new CovarianceMatrix();
    else if (sub_type == PARAM_DERIVED_QUANTITY)
      result = new DerivedQuantity();
    else if (sub_type == PARAM_ESTIMABLE)
      result = new Estimable();
    else if (sub_type == PARAM_ESTIMATE_SUMMARY)
      result = new EstimateSummary();
    else if (sub_type == PARAM_ESTIMATE_VALUE)
      result = new EstimateValue();
    else if (sub_type == PARAM_INITIALISATION_PARTITION)
      result = new InitialisationPartition();
    else if (sub_type == PARAM_MCMC_CHAIN)
      result = new MCMCChain();
    else if (sub_type == PARAM_MCMC_COVARIANCE)
      result = new MCMCCovariance();
    else if (sub_type == PARAM_PARTITION)
      result = new Partition();
    else if (sub_type == PARAM_PARTITION_BIOMASS)
      result = new PartitionBiomass();
    else if (sub_type == PARAM_PARTITION_MEAN_WEIGHT)
      result = new PartitionMeanWeight();
    else if (sub_type == PARAM_OBJECTIVE_FUNCTION)
      result = new ObjectiveFunction();
    else if (sub_type == PARAM_OBSERVATION)
      result = new Observation();
    else if (sub_type == PARAM_RANDOM_NUMBER_SEED)
      result = new RandomNumberSeed();
    else if (sub_type == PARAM_SIMULATED_OBSERVATION)
      result = new SimulatedObservation();
    else if (sub_type == PARAM_SELECTIVITY)
      result = new Selectivity();

    if (result)
      model->managers().report()->AddObject(result);
  }

  return result;
}

} /* namespace reports */
} /* namespace niwa */
