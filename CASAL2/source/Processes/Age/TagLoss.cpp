/**
 * @file TagLoss.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 20/04/2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
TagLoss::TagLoss(shared_ptr<Model> model) : Process(model), partition_(model) {
  LOG_TRACE();
  process_type_ = ProcessType::kMortality;
  // Why this was changed from type transition to mortality. CASAL includes this in the 'mortality block' so when you are
  // deriving observations partway through a time-step Casal incorporates F and TagLoss. Casal2 wasn't including tag-loss
  // into mid time-step interpolations. This fix will have annoying ramifications because a mortality block is defined
  // as continuous processes. so if you have a time-step with the following processes: F, tag-release, tag-loss. Casal2
  // may complain. CASAL reference see population_section.cpp line: 1924-2006

  partition_structure_ = PartitionType::kAge;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories to apply", "");
  parameters_.Bind<Double>(PARAM_TAG_LOSS_RATE, &tag_loss_input_, "The tag loss rates", "")->set_lower_bound(0.0, true);
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTIONS, &ratios_, "The time step proportions for tag loss", "", false)->set_lower_bound(0.0);
  parameters_.Bind<string>(PARAM_TAG_LOSS_TYPE, &tag_loss_type_, "The type of tag loss", "", PARAM_SINGLE)->set_allowed_values({PARAM_SINGLE, PARAM_DOUBLE_TAG});
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The selectivities", "");
  parameters_.Bind<unsigned>(PARAM_YEAR, &year_, "The year the first tagging release process was executed", "");

  RegisterAsAddressable(PARAM_TAG_LOSS_RATE, &tag_loss_);
}

/**
 * Validate the Mortality Constant Rate process
 *
 * - Validate the required parameters
 * - Assign the label from the parameters
 * - Assign and validate remaining parameters
 * - Duplicate 'm' and 'selectivities' if only one value specified
 * - Check m is between 0.0 and 1.0
 * - Check the categories are real
 */
void TagLoss::DoValidate() {
  LOG_FINEST() << "the number of categories = " << category_labels_.size() << ", the number of proportions = " << tag_loss_input_.size();

  if (tag_loss_input_.size() == 1) {
    auto val_t = tag_loss_input_[0];
    tag_loss_input_.assign(category_labels_.size(), val_t);
  }

  if (selectivity_names_.size() == 1) {
    auto val_s = selectivity_names_[0];
    selectivity_names_.assign(category_labels_.size(), val_s);
  }

  if (tag_loss_input_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_TAG_LOSS_RATE) << ": the number of tag loss values provided is not the same as the number of categories provided. Categories: " << category_labels_.size()
                                     << ", tag loss size " << tag_loss_input_.size();
  }

  if (selectivity_names_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES) << ": the number of selectivities provided is not the same as the number of categories provided. Categories: " << category_labels_.size()
                                     << ", selectivities size " << selectivity_names_.size();
  }

  // Validate our instantaneous rates greater than 0.0
  for (Double tag_loss : tag_loss_input_) {
    if (tag_loss < 0.0)
      LOG_ERROR_P(PARAM_TAG_LOSS_RATE) << ": Tag loss rate " << tag_loss << " must be greater or equal to 0.0";
  }

  for (unsigned i = 0; i < tag_loss_input_.size(); ++i) tag_loss_[category_labels_[i]] = tag_loss_input_[i];

  // Validate the time step proportions sum to one
  Double total = 0.0;
  for (Double value : ratios_) {
    total += value;
  }
#ifndef TESTMODE
  if (!utilities::math::IsOne(total)) {
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << ": The time_step_proportions must be specified to sum to one, but they summed to " << total << ".";
  }
#endif
}

/**
 * Build any runtime relationships
 * - Build the partition accessor
 * - Build the list of selectivities
 * - Build the ratios for the number of time steps
 */
void TagLoss::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";

    selectivities_.push_back(selectivity);
  }

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of M so here we want to verify
   * we have enough and re-scale them to 1.0
   */
  vector<TimeStep*> time_steps = model_->managers()->time_step()->ordered_time_steps();
  LOG_FINEST() << "time_steps.size(): " << time_steps.size();
  vector<unsigned> active_time_steps;
  for (unsigned i = 0; i < time_steps.size(); ++i) {
    if (time_steps[i]->HasProcess(label_))
      active_time_steps.push_back(i);
  }

  if (ratios_.size() == 0) {
    LOG_FATAL_P(PARAM_TIME_STEP_PROPORTIONS) << "are required parameters for this process and must be specified";
  } else {
    if (ratios_.size() != active_time_steps.size())
      LOG_FATAL_P(PARAM_TIME_STEP_PROPORTIONS) << " length (" << ratios_.size() << ") does not match the number of time steps this process has been assigned to ("
                                               << active_time_steps.size() << ")";

    for (Double value : ratios_) {
      if (value < 0.0 || value > 1.0)
        LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << " Time step proportions (" << value << ") must be between 0.0 and 1.0 inclusive";
    }

    for (unsigned i = 0; i < ratios_.size(); ++i) time_step_ratios_[active_time_steps[i]] = ratios_[i];
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
    unsigned time_step = model_->managers()->time_step()->current_time_step();
    LOG_FINEST() << "Ratios.size() " << time_step_ratios_.size() << " : time_step: " << time_step << "; ratio: " << time_step_ratios_[time_step];
    Double ratio = time_step_ratios_[time_step];

    // StoreForReport("year", model_->current_year());

    if (tag_loss_type_ == PARAM_SINGLE) {
      unsigned i = 0;
      for (auto category : partition_) {
        Double tag_loss = tag_loss_[category->name_];

        unsigned j = 0;
        LOG_FINEST() << "category " << category->name_ << "; min_age: " << category->min_age_ << "; ratio: " << ratio;
        // StoreForReport(category->name_ + " ratio", ratio);
        for (Double& data : category->data_) {
          // Deleting this partition. In future we may have a target category to migrate to.
          Double amount = data * (1.0 - exp(-selectivities_[i]->GetAgeResult(category->min_age_ + j, category->age_length_) * tag_loss * ratio));
          LOG_FINEST() << "Category " << category->name_ << " numbers at age: " << category->min_age_ + j << " = " << data << " removing " << amount;
          data -= amount;
          ++j;
        }
        ++i;
      }
    } else if (tag_loss_type_ == PARAM_DOUBLE_TAG) {
      unsigned i = 0;
      // identify years (time) since tagging
      Double time = Double(model_->current_year() - year_);
      for (auto category : partition_) {
        Double tag_loss        = tag_loss_[category->name_];
        Double double_tag_loss = 0.0;
        if (time > 0.0) {
          Double A        = (1 - exp(-tag_loss * (time - 1)));
          Double B        = (1 - exp(-tag_loss * time));
          double_tag_loss = -log(1.0 - ((B * B) - (A * A)) / (1 - (A * A)));
        }
        unsigned j = 0;
        // StoreForReport(category->name_ + " ratio", ratio);
        for (Double& data : category->data_) {
          Double amount = data * (1.0 - exp(-selectivities_[i]->GetAgeResult(category->min_age_ + j, category->age_length_) * double_tag_loss * ratio));
          LOG_FINEST() << "Category " << category->name_ << " numbers at age: " << category->min_age_ + j << " = " << data << " removing " << amount
                       << " Selectivity: " << selectivities_[i]->GetAgeResult(category->min_age_ + j, category->age_length_) << " tag_loss: " << tag_loss
                       << " double_tag_loss: " << double_tag_loss << " ratio: " << ratio << " time: " << time;
          data -= amount;
          ++j;
        }
        ++i;
      }
    }
  }
}

/**
 * Reset the tag loss process
 */
void TagLoss::DoReset() {}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
