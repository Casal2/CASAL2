/**
 * @file MortalityEventBiomass.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/11/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "MortalityEventBiomass.h"

#include "Common/Categories/Categories.h"
#include "Common/Model/Model.h"
#include "Common/Penalties/Manager.h"
#include "Common/Selectivities/Manager.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace age {
namespace processes {
namespace math = niwa::utilities::math;

/**
 * default constructor
 */
MortalityEventBiomass::MortalityEventBiomass(Model* model)
  : Process(model),
    partition_(model) {

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Category labels", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities for each of the categories", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years in which to apply the mortality process", "");
  parameters_.Bind<Double>(PARAM_CATCHES, &catches_, "The biomass of removals (catches) to apply for each year", "");
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "Maximum exploitation rate ($Umax$)", "", 0.99);
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "The label of the penalty to apply if the total biomass of removals cannot be taken", "", "");


  RegisterAsAddressable(PARAM_U_MAX, &u_max_);
  RegisterAsAddressable(PARAM_CATCHES, &catch_years_);

  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;
}

/**
 *
 */
void MortalityEventBiomass::DoValidate() {
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  if (u_max_ <= 0.0 || u_max_ >= 1.0)
    LOG_ERROR_P(PARAM_U_MAX) << " (" << u_max_ << ") must be between 0.0 and 1.0 exclusive";

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << " number provided (" << selectivity_labels_.size() << ") must match the number of "
        "categories provided (" << category_labels_.size() << ")";
  if (years_.size() != catches_.size())
    LOG_ERROR_P(PARAM_CATCHES) << " number provided (" << catches_.size() << ") must match the number of "
        "years provided (" << years_.size() << ")";


  // Validate: catches_ and years_
  for(unsigned i = 0; i < years_.size(); ++i) {
    if (catch_years_.find(years_[i]) != catch_years_.end()) {
      LOG_ERROR_P(PARAM_YEARS) << " year " << years_[i] << " has already been specified, please remove the duplicate";
    }
    catch_years_[years_[i]] = catches_[i];
  }

  // add extra years in to model for things like forward projection
  vector<unsigned> model_years = model_->years();
  for (unsigned year : model_years) {
    if (catch_years_.find(year) == catch_years_.end())
      catch_years_[year] = 0.0;
  }
}

/**
 *
 */
void MortalityEventBiomass::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?";

    selectivities_.push_back(selectivity);
  }

  if (penalty_label_ != "") {
    penalty_ = model_->managers().penalty()->GetProcessPenalty(penalty_label_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": penalty " << penalty_label_ << " does not exist. Have you defined it?";
    }
  }
}

/**
 *
 */
void MortalityEventBiomass::DoExecute() {
  if (catch_years_[model_->current_year()] == 0)
    return;
  unsigned time_step_index = model_->managers().time_step()->current_time_step();
  LOG_TRACE();
  /**
   * Work our how much of the stock is vulnerable
   */
  Double vulnerable = 0.0;
  unsigned i = 0;
  for (auto categories : partition_) {
    //categories->UpdateMeanWeightData();
    unsigned offset = 0;
    for (Double& data : categories->data_) {
      Double temp = data * selectivities_[i]->GetResult(categories->min_age_ + offset, categories->age_length_);
      vulnerable += temp * categories->mean_weight_by_time_step_age_[time_step_index][categories->min_age_ + offset];
      ++offset;
    }

    ++i;
  }

  /**
   * Work out the exploitation rate to remove (catch/vulnerable)
   */
  Double exploitation = catch_years_[model_->current_year()] / utilities::doublecompare::ZeroFun(vulnerable);
  if (exploitation > u_max_) {
    exploitation = u_max_;
    if (penalty_)
      penalty_->Trigger(label_, catch_years_[model_->current_year()], vulnerable * u_max_);

  } else if (exploitation < 0.0) {
    exploitation = 0.0;
  }

  LOG_FINEST() << "year: " << model_->current_year() << "; exploitation: " << AS_DOUBLE(exploitation);

  /**
   * Remove the stock now. The amount to remove is
   * vulnerable * exploitation and store for report
   */
/*
  StoreForReport("year: ", model_->current_year());
  StoreForReport("Exploitation: ", AS_DOUBLE(exploitation));
  StoreForReport("Catch: ", AS_DOUBLE(catch_years_[model_->current_year()]));
  string current_year = utilities::ToInline<unsigned,string>(model_->current_year());
  string catch_label, U_label;

	catch_label = "catch[" + label_ + "]." + current_year;
	U_label = "fishing_pressure[" + label_ + "]." + current_year;
  StoreForTabularReport(catch_label, AS_DOUBLE(catch_years_[model_->current_year()]));
  StoreForTabularReport(U_label, AS_DOUBLE(catch_years_[model_->current_year()]));
*/


  i = 0;
  Double removals = 0;
  for (auto categories : partition_) {
    unsigned offset = 0;
    for (Double& data : categories->data_) {
      removals = data * selectivities_[i]->GetResult(categories->min_age_ + offset, categories->age_length_) * exploitation;
      //StoreForReport(categories->name_ + "_Removals: ",AS_DOUBLE(removals));
      data -= removals;
      ++offset;
    }
    ++i;
  }
}

/*
 * @fun FillReportCache
 * @description A method for reporting process information
 * @param cache a cache object to print to
*/
void MortalityEventBiomass::FillReportCache(ostringstream& cache) {

}

/*
 * @fun FillTabularReportCache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
*/
void MortalityEventBiomass::FillTabularReportCache(ostringstream& cache, bool first_run) {

}



} /* namespace processes */
} /* namespace age */
} /* namespace niwa */

