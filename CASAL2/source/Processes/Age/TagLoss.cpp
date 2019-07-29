/**
 * @file TagLoss.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 20/04/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "TagLoss.h"

#include <numeric>

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default Constructor
 */
TagLoss::TagLoss(Model* model)
  : Process(model),
    partition_(model) {
  LOG_TRACE();
  process_type_ = ProcessType::kTransition;
  partition_structure_ = PartitionType::kAge;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "List of categories", "");
  parameters_.Bind<Double>(PARAM_TAG_LOSS_RATE, &tag_loss_input_, "Tag Loss rates", "");
  parameters_.Bind<Double>(PARAM_TIME_STEP_RATIO, &ratios_, "Time step ratios for Tag Loss", "", true);
  parameters_.Bind<string>(PARAM_TAG_LOSS_TYPE, &tag_loss_type_, "Type of tag loss", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "Selectivities", "");
  parameters_.Bind<unsigned>(PARAM_YEAR, &year_, "The year the first tagging release process was executed", "");

  RegisterAsAddressable(PARAM_TAG_LOSS, &tag_loss_);
}

/**
 * Validate our Mortality Constant Rate process
 *
 * - Validate the required parameters
 * - Assign the label from the parameters
 * - Assign and validate remaining parameters
 * - Duplicate 'm' and 'selectivities' if only 1 vale specified
 * - Check m is between 0.0 and 1.0
 * - Check the categories are real
 */
void TagLoss::DoValidate() {
  LOG_FINEST() << "Number of categories = " << category_labels_.size() << " number of proportions given = " << tag_loss_input_.size();

  if (tag_loss_input_.size() == 1)
    tag_loss_input_.assign(category_labels_.size(), tag_loss_input_[0]);
  if (selectivity_names_.size() == 1)
    selectivity_names_.assign(category_labels_.size(), selectivity_names_[0]);

  if (tag_loss_input_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_TAG_LOSS_RATE)
        << ": Number of tag loss values provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << tag_loss_input_.size();
  }

  if (selectivity_names_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << selectivity_names_.size();
  }

  // Validate type of tag loss
  if (tag_loss_type_ != "single")
    LOG_ERROR_P(PARAM_TAG_LOSS_TYPE) << tag_loss_type_ << " Is not an expected type. Values allowed are " << PARAM_SINGLE << " and " << PARAM_DOUBLE << " is coming soon";

  if (tag_loss_type_ == PARAM_DOUBLE)
    LOG_ERROR() << PARAM_TAG_LOSS_TYPE << " " << PARAM_DOUBLE << " is not implemented yet";
  // Validate our Ms are between 1.0 and 0.0
  for (Double tag_loss : tag_loss_input_) {
    if (tag_loss < 0.0 || tag_loss > 1.0)
      LOG_ERROR_P(PARAM_TAG_LOSS_RATE) << ": m value " << AS_DOUBLE(tag_loss) << " must be between 0.0 and 1.0 (inclusive)";
  }

  for (unsigned i = 0; i < tag_loss_input_.size(); ++i)
    tag_loss_[category_labels_[i]] = tag_loss_input_[i];
}

/**
 * Build any runtime relationships
 * - Build the partition accessor
 * - Build our list of selectivities
 * - Build our ratios for the number of time steps
 */
void TagLoss::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?";

    selectivities_.push_back(selectivity);
  }

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of M so here we want to verify
   * we have enough and re-scale them to 1.0
   */
  vector<TimeStep*> time_steps = model_->managers().time_step()->ordered_time_steps();
  LOG_FINEST() << "time_steps.size(): " << time_steps.size();
  vector<unsigned> active_time_steps;
  for (unsigned i = 0; i < time_steps.size(); ++i) {
    if (time_steps[i]->HasProcess(label_))
      active_time_steps.push_back(i);
  }

  if (ratios_.size() == 0) {
    for (unsigned i : active_time_steps)
      time_step_ratios_[i] = 1.0;
  } else {
    if (ratios_.size() != active_time_steps.size())
      LOG_FATAL_P(PARAM_TIME_STEP_RATIO) << " length (" << ratios_.size()
          << ") does not match the number of time steps this process has been assigned to (" << active_time_steps.size() << ")";

    for (Double value : ratios_) {
      if (value < 0.0 || value > 1.0)
        LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << " value (" << value << ") must be between 0.0 (inclusive) and 1.0 (inclusive)";
    }

    for (unsigned i = 0; i < ratios_.size(); ++i)
      time_step_ratios_[active_time_steps[i]] = ratios_[i];
  }
}

/**
 * Execute the process
 */
void TagLoss::DoExecute() {
  // To reduce computation only execute in relevant years.
  if (model_->current_year() >= year_) {
    LOG_FINEST() << "year: " << model_->current_year();

    // get the ratio to apply first
    unsigned time_step = model_->managers().time_step()->current_time_step();

    LOG_FINEST() << "Ratios.size() " << time_step_ratios_.size() << " : time_step: " << time_step << "; ratio: " << time_step_ratios_[time_step];
    Double ratio = time_step_ratios_[time_step];

    //StoreForReport("year", model_->current_year());

    unsigned i = 0;
    for (auto category : partition_) {
      Double tag_loss = tag_loss_[category->name_];

      unsigned j = 0;
      LOG_FINEST() << "category " << category->name_ << "; min_age: " << category->min_age_ << "; ratio: " << ratio;
      //StoreForReport(category->name_ + " ratio", ratio);
      for (Double& data : category->data_) {
        // Deleting this partition. In future we may have a target category to migrate to.
        Double amount = data * (1 - exp(-selectivities_[i]->GetAgeResult(category->min_age_ + j, category->age_length_) * tag_loss * ratio));
        LOG_FINEST() << "Category " << category->name_  << " numbers at age: " << category->min_age_ + j << " = " << data << " removing " << amount;
        data -= amount;
        ++j;
      }

      ++i;
    }
  }
}

/**
 * Reset the Tag loss Process
 */
void TagLoss::DoReset() {
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
