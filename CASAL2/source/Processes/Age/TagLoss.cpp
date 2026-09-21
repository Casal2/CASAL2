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

#include <set>

#include "Categories/Categories.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default Constructor
 *
 * @param rate_source kSingle for tag_loss (resolved to kSingle/kDoubleTag from tag_loss_type in
 *   DoValidate()), or kEmpirical for tag_loss_empirical. The two types bind different parameters
 *   and register a different addressable shape, so this is fixed at construction.
 */
TagLoss::TagLoss(shared_ptr<Model> model, TagLossRateSource rate_source) : MortalityRateBase(model), rate_source_(rate_source) {
  LOG_TRACE();
  process_type_ = ProcessType::kMortality;
  // Why this was changed from type transition to mortality. CASAL includes this in the 'mortality block' so when you are
  // deriving observations partway through a time-step Casal incorporates F and TagLoss. Casal2 wasn't including tag-loss
  // into mid time-step interpolations. This fix will have annoying ramifications because a mortality block is defined
  // as continuous processes. so if you have a time-step with the following processes: F, tag-release, tag-loss. Casal2
  // may complain. CASAL reference see population_section.cpp line: 1924-2006

  partition_structure_ = PartitionType::kAge;

  // clang-format off
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories to apply")
    ->flag_is_category();
  parameters_.Bind<Double>(PARAM_TAG_LOSS_RATE, &tag_loss_input_, "The tag loss rates");
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &ratios_, "The time step proportions for tag loss");
  if (rate_source_ != TagLossRateSource::kEmpirical)
    parameters_.Bind<string>(PARAM_TAG_LOSS_TYPE, &tag_loss_type_, "The type of tag loss")
      ->set_default_value(PARAM_SINGLE);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The selectivities");
  parameters_.Bind<unsigned>(PARAM_YEAR, &year_, "The year the first tagging release process was executed");
  if (rate_source_ == TagLossRateSource::kEmpirical)
    parameters_.Bind<unsigned>(PARAM_YEARS_AT_LIBERTY, &years_at_liberty_, "Years at liberty to apply the annual tag loss rates of tags", "")->set_lower_bound(0, true);
  // clang-format on

  // tag_loss addresses its rate by category; tag_loss_empirical by year at liberty
  if (rate_source_ == TagLossRateSource::kEmpirical)
    RegisterAsAddressable(PARAM_TAG_LOSS_RATE, &tag_loss_input_);
  else
    RegisterAsAddressable(PARAM_TAG_LOSS_RATE, &tag_loss_);
}

/**
 * Validate the tag loss process
 *
 * - Duplicate 'tag_loss_rate' (per category, or per year at liberty) and 'selectivities' if only one value specified
 * - Check tag_loss_rate is >= 0
 * - Resolve tag_loss_type into the rate source (tag_loss only)
 */
void TagLoss::DoValidate() {
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_TIME_STEP_PROPORTIONS)->SumToOne();
  parameters_.Validate(PARAM_YEAR)->IsModelYear();

  if (rate_source_ == TagLossRateSource::kEmpirical) {
    LOG_FINEST() << "the number of years = " << years_at_liberty_.size() << ", the number of proportions = " << tag_loss_input_.size();
    parameters_.ValidateVector(PARAM_TAG_LOSS_RATE)
        ->GreaterThanOrEqualTo(0.0)
        ->ExpandToSameNumberOfElementsAs(PARAM_YEARS_AT_LIBERTY)
        ->SameNumberOfElementsAs(PARAM_YEARS_AT_LIBERTY);
    parameters_.ValidateVector(PARAM_YEARS_AT_LIBERTY)->GreaterThanOrEqualTo(0u)->LessThanOrEqualTo(model()->final_year() - model()->start_year() + 1u);

    // Normalisation: the pre-merge TagLossEmpirical indexed past the end of its selectivities
    // when a year at liberty was repeated, so reject duplicates outright.
    std::set<unsigned> unique_years(years_at_liberty_.begin(), years_at_liberty_.end());
    if (unique_years.size() != years_at_liberty_.size())
      LOG_ERROR_P(PARAM_YEARS_AT_LIBERTY) << "values must be unique";
    return;
  }

  LOG_FINEST() << "the number of categories = " << category_labels_.size() << ", the number of proportions = " << tag_loss_input_.size();
  parameters_.ValidateVector(PARAM_TAG_LOSS_RATE)->GreaterThanOrEqualTo(0.0)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  tag_loss_ = utilities::OrderedMap<string, Double>::create(category_labels_, tag_loss_input_);

  // Normalisation: the pre-merge TagLoss silently applied no tag loss for an unrecognised type.
  if (tag_loss_type_ == PARAM_SINGLE)
    rate_source_ = TagLossRateSource::kSingle;
  else if (tag_loss_type_ == PARAM_DOUBLE_TAG)
    rate_source_ = TagLossRateSource::kDoubleTag;
  else
    LOG_ERROR_P(PARAM_TAG_LOSS_TYPE) << "value (" << tag_loss_type_ << ") is not valid. It must be either " << PARAM_SINGLE << " or " << PARAM_DOUBLE_TAG;
}

/**
 * Build any runtime relationships
 * - Build the partition accessor
 * - Build the list of selectivities
 * - Build the ratios for the number of time steps
 */
void TagLoss::DoBuild() {
  partition_.Init(category_labels_);

  selectivities_ = ResolveSelectivities(model(), parameters(), selectivity_names_, PARAM_SELECTIVITIES, ": Selectivity label ");

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of tag loss so here we want to verify
   * we have enough and re-scale them to 1.0
   */
  time_step_ratios_
      = BuildTimeStepRatios(model(), parameters(), label_, ratios_, " length (", false, " Time step proportions (", ") must be between 0.0 and 1.0 inclusive");
}

/**
 * Execute the process
 */
void TagLoss::DoExecute() {
  // To reduce computation only execute in relevant years.
  if (model()->current_year() < year_)
    return;

  LOG_FINEST() << "year: " << model()->current_year();

  // get the ratio to apply first
  unsigned time_step = model()->managers()->time_step()->current_time_step();
  LOG_FINEST() << "Ratios.size() " << time_step_ratios_.size() << " : time_step: " << time_step << "; ratio: " << time_step_ratios_[time_step];
  Double ratio = time_step_ratios_[time_step];

  switch (rate_source_) {
    case TagLossRateSource::kSingle:
      ApplyRemovalLoop(partition_, process_profile_, common::RemovalFormulation::kExponentialDecay, tag_loss_, selectivities_, ratio);
      break;

    case TagLossRateSource::kDoubleTag: {
      // identify years (time) since tagging, and derive this year's rate for each category
      Double                     time = Double(model()->current_year() - year_);
      OrderedMap<string, Double> double_tag_loss_by_category;
      for (auto& entry : tag_loss_) {
        Double tag_loss        = entry.second;
        Double double_tag_loss = 0.0;
        if (time > 0.0) {
          Double A        = (1 - exp(-tag_loss * (time - 1)));
          Double B        = (1 - exp(-tag_loss * time));
          double_tag_loss = -log(1.0 - ((B * B) - (A * A)) / (1 - (A * A)));
        }
        LOG_FINEST() << "category " << entry.first << " tag_loss: " << tag_loss << " double_tag_loss: " << double_tag_loss << " time: " << time;
        double_tag_loss_by_category[entry.first] = double_tag_loss;
      }
      ApplyRemovalLoop(partition_, process_profile_, common::RemovalFormulation::kExponentialDecay, double_tag_loss_by_category, selectivities_, ratio);
      break;
    }

    case TagLossRateSource::kEmpirical: {
      // apply the rate for the specific time at liberty (if any) to every category
      unsigned time = model()->current_year() - year_;
      for (unsigned year = 0; year < years_at_liberty_.size(); ++year) {
        if (time != years_at_liberty_[year])
          continue;

        OrderedMap<string, Double> rate_by_category;
        for (const string& label : category_labels_) rate_by_category[label] = tag_loss_input_[year];
        ApplyRemovalLoop(partition_, process_profile_, common::RemovalFormulation::kExponentialDecay, rate_by_category, selectivities_, ratio);
        break;  // years_at_liberty_ values are unique (see DoValidate)
      }
      break;
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
