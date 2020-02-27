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
#include "Reports/Age/AgeingErrorMatrix.h"
#include "Reports/Age/AgeLength.h"
#include "Reports/Age/InitialisationPartitionMeanWeight.h"
#include "Reports/Age/PartitionMeanWeight.h"
#include "Reports/Age/PartitionBiomass.h"
#include "Reports/Age/Partition.h"
#include "Reports/Age/PartitionYearCrossAgeMatrix.h"
#include "Reports/Common/Addressable.h"
#include "Reports/Common/CategoryInfo.h"
#include "Reports/Common/Catchability.h"
#include "Reports/Common/CategoryList.h"
#include "Reports/Common/CovarianceMatrix.h"
#include "Reports/Common/CorrelationMatrix.h"
#include "Reports/Common/DerivedQuantity.h"
#include "Reports/Common/EquationTest.h"
#include "Reports/Common/EstimateSummary.h"
#include "Reports/Common/EstimateValue.h"
#include "Reports/Common/EstimationResult.h"
#include "Reports/Common/HessianMatrix.h"
#include "Reports/Common/MCMCCovariance.h"
#include "Reports/Common/MCMCObjective.h"
#include "Reports/Common/MCMCSample.h"
#include "Reports/Common/MPD.h"
#include "Reports/Common/ObjectiveFunction.h"
#include "Reports/Common/Observation.h"
#include "Reports/Common/InitialisationPartition.h"
#include "Reports/Common/Process.h"
#include "Reports/Common/Project.h"
#include "Reports/Common/RandomNumberSeed.h"
#include "Reports/Common/OutputParameters.h"
#include "Reports/Common/SimulatedObservation.h"
#include "Reports/Common/Selectivity.h"
#include "Reports/Common/TimeVarying.h"
#include "Reports/Length/InitialisationPartitionMeanWeight.h"
#include "Reports/Length/PartitionMeanWeight.h"
#include "Reports/Length/PartitionBiomass.h"
#include "Reports/Length/Partition.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object created
 */
Report* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Report* result = nullptr;

  if (object_type == PARAM_REPORT) {
    if (sub_type == PARAM_CATEGORY_INFO)
      result = new CategoryInfo(model);
    else if (sub_type == PARAM_CATEGORY_LIST)
      result = new CategoryList(model);
    else if (sub_type == PARAM_CATCHABILITY)
      result = new Catchability(model);
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
    else if (sub_type == PARAM_ESTIMATION_RESULT)
      result = new EstimationResult(model);
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
    else if (model->partition_type() == PartitionType::kAge) {
      if (object_type == PARAM_REPORT) {
        if (sub_type == PARAM_AGEING_ERROR)
          result = new age::AgeingErrorMatrix(model);
        if (sub_type == PARAM_AGE_LENGTH)
          result = new age::AgeLength(model);
        else if (sub_type == PARAM_PARTITION_BIOMASS)
          result = new age::PartitionBiomass(model);
        else if (sub_type == PARAM_PARTITION_MEAN_WEIGHT)
          result = new age::PartitionMeanWeight(model);
        else if (sub_type == PARAM_INITIALISATION_PARTITION_MEAN_WEIGHT)
          result = new age::InitialisationPartitionMeanWeight(model);
        else if (sub_type == PARAM_PARTITION)
          result = new age::Partition(model);
         else if (sub_type == PARAM_REPORT_PARTITION_YEARCROSSAGEMATRIX)
          result = new age::Partition_YearCrossAgeMatrix(model);
       }
    } else if (model->partition_type() == PartitionType::kLength) {
      if (sub_type == PARAM_INITIALISATION_PARTITION_MEAN_WEIGHT)
        result = new age::InitialisationPartitionMeanWeight(model);
      else if (sub_type == PARAM_PARTITION_MEAN_WEIGHT)
        result = new age::PartitionMeanWeight(model);
      else if (sub_type == PARAM_PARTITION_BIOMASS)
        result = new age::PartitionBiomass(model);
      else if (sub_type == PARAM_PARTITION)
        result = new age::Partition(model);
    }


    if (result)
      model->managers().report()->AddObject(result);
  }

  return result;
}

} /* namespace reports */
} /* namespace niwa */
