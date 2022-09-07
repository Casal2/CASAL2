/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers

#include "Factory.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Reports/Common/Addressable.h"
#include "../Reports/Common/AddressableTransformation.h"
#include "../Reports/Common/Catchability.h"
#include "../Reports/Common/CategoryInfo.h"
#include "../Reports/Common/CategoryList.h"
#include "../Reports/Common/CorrelationMatrix.h"
#include "../Reports/Common/CovarianceMatrix.h"
#include "../Reports/Common/DefaultReports.h"
#include "../Reports/Common/DerivedQuantity.h"
#include "../Reports/Common/EquationTest.h"
#include "../Reports/Common/EstimateSummary.h"
#include "../Reports/Common/EstimateValue.h"
#include "../Reports/Common/EstimationResult.h"
#include "../Reports/Common/HessianMatrix.h"
#include "../Reports/Common/InitialisationPartition.h"
#include "../Reports/Common/MCMCCovariance.h"
#include "../Reports/Common/MCMCObjective.h"
#include "../Reports/Common/MCMCSample.h"
#include "../Reports/Common/MPD.h"
#include "../Reports/Common/ObjectiveFunction.h"
#include "../Reports/Common/Observation.h"
#include "../Reports/Common/OutputParameters.h"
#include "../Reports/Common/Partition.h"
#include "../Reports/Common/PartitionBiomass.h"
#include "../Reports/Common/Process.h"
#include "../Reports/Common/Profile.h"
#include "../Reports/Common/Project.h"
#include "../Reports/Common/RandomNumberSeed.h"
#include "../Reports/Common/Selectivity.h"
#include "../Reports/Common/SelectivityByYear.h"
#include "../Reports/Common/SimulatedObservation.h"
#include "../Reports/Common/TimeVarying.h"
// Length
#include "../Reports/Length/GrowthIncrement.h"
#include "../Reports/Manager.h"
// Age
#include "../Reports/Age/AgeLength.h"
#include "../Reports/Age/AgeingErrorMatrix.h"

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
Report* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type, bool add_to_manager) {
  Report* result = nullptr;

  if (object_type == PARAM_REPORT) {
    if (sub_type == PARAM_DEFAULT)
      result = new DefaultReports();
    // Deprecating this report as its not useful nor used in anything but a basic run
    // Note the R extract.mpd() function v22.06 also errors out for this report 
    // if (sub_type == PARAM_CATEGORY_INFO)
    //  result = new CategoryInfo();
    else if (sub_type == PARAM_CATEGORY_LIST)
      result = new CategoryList();
    else if (sub_type == PARAM_CATCHABILITY)
      result = new Catchability();
    else if (sub_type == PARAM_COVARIANCE_MATRIX)
      result = new CovarianceMatrix();
    else if (sub_type == PARAM_CORRELATION_MATRIX)
      result = new CorrelationMatrix();
    else if (sub_type == PARAM_DERIVED_QUANTITY)
      result = new DerivedQuantity();
    else if (sub_type == PARAM_EQUATION)
      result = new EquationTest();
    else if (sub_type == PARAM_ADDRESSABLE)
      result = new Addressable();
    else if (sub_type == PARAM_PARAMETER_TRANSFORMATIONS)
      result = new AddressableTransformation();
    else if (sub_type == PARAM_ESTIMATE_SUMMARY)
      result = new EstimateSummary();
    else if (sub_type == PARAM_ESTIMATE_VALUE)
      result = new EstimateValue();
    else if (sub_type == PARAM_ESTIMATION_RESULT)
      result = new EstimationResult();
    else if (sub_type == PARAM_HESSIAN_MATRIX)
      result = new HessianMatrix();
    else if (sub_type == PARAM_MCMC_COVARIANCE)
      result = new MCMCCovariance();
    else if (sub_type == PARAM_MCMC_OBJECTIVE)
      result = new MCMCObjective();
    else if (sub_type == PARAM_MCMC_SAMPLE)
      result = new MCMCSample();
    else if (sub_type == PARAM_MPD)
      result = new MPD();
    else if (sub_type == PARAM_PROJECT)
      result = new Project();
    else if (sub_type == PARAM_PROFILE)
      result = new Profile();
    else if (sub_type == PARAM_PROCESS)
      result = new Process();
    else if (sub_type == PARAM_OBJECTIVE_FUNCTION)
      result = new ObjectiveFunction();
    else if (sub_type == PARAM_OBSERVATION)
      result = new Observation();
    else if (sub_type == PARAM_OUTPUT_PARAMETERS)
      result = new OutputParameters();
    else if (sub_type == PARAM_RANDOM_NUMBER_SEED)
      result = new RandomNumberSeed();
    else if (sub_type == PARAM_SIMULATED_OBSERVATION)
      result = new SimulatedObservation();
    else if (sub_type == PARAM_SELECTIVITY)
      result = new Selectivity();
    else if (sub_type == PARAM_SELECTIVITY_BY_YEAR)
      result = new SelectivityByYear();
    else if (sub_type == PARAM_TIME_VARYING)
      result = new TimeVarying();
    else if (sub_type == PARAM_INITIALISATION_PARTITION)
      result = new InitialisationPartition();
    else if (sub_type == PARAM_PARTITION_BIOMASS)
      result = new PartitionBiomass();
    else if (sub_type == PARAM_PARTITION)
      result = new Partition();
    else if (model->partition_type() == PartitionType::kAge) {
      if (object_type == PARAM_REPORT) {
        if (sub_type == PARAM_AGEING_ERROR)
          result = new age::AgeingErrorMatrix();
        if (sub_type == PARAM_AGE_LENGTH)
          result = new age::AgeLength();
      }
    } else if (model->partition_type() == PartitionType::kLength) {
      if (sub_type == PARAM_GROWTH_INCREMENT)
        result = new length::GrowthIncrement();
    }

    if (result && add_to_manager)
      model->managers()->report()->AddObject(result);
  }

  return result;
}

} /* namespace reports */
} /* namespace niwa */
