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

#include "Model/Model.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace isam {
namespace processes {

/**
 * default constructor
 */
MortalityEventBiomass::MortalityEventBiomass() {
  is_mortality_process = true;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Category labels", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "Selectivity labels", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to apply mortality", "");
  parameters_.Bind<double>(PARAM_CATCHES, &catches_, "Catches for each year", "");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "U Max", "", 0.99);
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "Penalty label", "", "");

  RegisterAsEstimable(PARAM_U_MAX, &u_max_);
  RegisterAsEstimable(PARAM_CATCHES, &catch_years_);

  model_ = Model::Instance();
}

/**
 *
 */
void MortalityEventBiomass::DoValidate() {
  if (u_max_ <= 0.0 || u_max_ >= 1.0)
    LOG_ERROR(parameters_.location(PARAM_U_MAX) << " (" << u_max_ << ") must be between 0.0 and 1.0 exclusive");

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << " number provided (" << selectivity_labels_.size() << ") must match the number of "
        "categories provided (" << category_labels_.size() << ")");
  if (years_.size() != catches_.size())
    LOG_ERROR(parameters_.location(PARAM_CATCHES) << " number provided (" << catches_.size() << ") must match the number of "
        "years provided (" << years_.size() << ")");


  // Validate: catches_ and years_
  for(unsigned i = 0; i < years_.size(); ++i) {
    if (catch_years_.find(years_[i]) != catch_years_.end()) {
      LOG_ERROR(parameters_.location(PARAM_YEARS) << " year " << years_[i] << " has already been specified, please remove the duplicate");
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
    SelectivityPtr selectivity = selectivities::Manager::Instance().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?");

    selectivities_.push_back(selectivity);
  }

  if (penalty_label_ != "") {
    penalty_ = penalties::Manager::Instance().GetPenalty(penalty_label_);
    if (!penalty_) {
      LOG_ERROR(parameters_.location(PARAM_PENALTY) << ": penalty " << penalty_label_ << " does not exist. Have you defined it?");
    }
  }
}

/**
 *
 */
void MortalityEventBiomass::Execute() {
  if (catch_years_[model_->current_year()] == 0)
    return;

  LOG_TRACE();

  /**
   * Work our how much of the stock is vulnerable
   */
  Double vulnerable = 0.0;
  unsigned i = 0;
  for (auto categories : partition_) {
    unsigned offset = 0;
    for (Double& data : categories->data_) {
      Double temp = data * selectivities_[i]->GetResult(categories->min_age_ + offset);
      vulnerable += temp * categories->mean_weights_[offset];
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
      penalty_->Trigger(label_, catch_years_[model_->current_year()], vulnerable*u_max_);

  } else if (exploitation < 0.0) {
    exploitation = 0.0;
  }

  LOG_INFO("year: " << model_->current_year() << "; exploitation: " << AS_DOUBLE(exploitation));

  /**
   * Remove the stock now. The amount to remove is
   * vulnerable * exploitation
   */
  i = 0;
  for (auto categories : partition_) {
    unsigned offset = 0;
    for (Double& data : categories->data_) {
      data -= data * selectivities_[i]->GetResult(categories->min_age_ + offset) * exploitation;
      ++offset;
    }
    ++i;
  }
}

} /* namespace processes */
} /* namespace isam */

