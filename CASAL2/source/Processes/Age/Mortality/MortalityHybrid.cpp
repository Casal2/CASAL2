/**
 * @file MortalityHybrid.cpp
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * This mortality process is taken from Stock Synthesis see the hybrid approach
 * found at
 * https://github.com/nmfs-stock-synthesis/stock-synthesis/blob/main/SS_popdyn.tpl
 *
 */

// headers
#include "MortalityHybrid.h"

#include "AgeLengths/AgeLength.h"
#include "Model/Managers.h"
#include "TimeSteps/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {
using niwa::utilities::findInVector;

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
MortalityHybrid::MortalityHybrid(shared_ptr<Model> model) : MortalityFisheryBase(model) {
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;

  catches_table_ = parameters_.BindTable(PARAM_CATCHES, "The table of removals (catch) data");
  catches_table_->set_required_columns({PARAM_YEAR}, true);
  method_table_ = parameters_.BindTable(PARAM_METHOD, "The table of method of removal data");
  method_table_->set_required_columns({PARAM_METHOD, PARAM_CATEGORY, PARAM_SELECTIVITY, PARAM_TIME_STEP, PARAM_PENALTY, PARAM_ANNUAL_DURATION}, true);
  method_table_->set_optional_columns({PARAM_AGE_WEIGHT_LABEL});

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories for instantaneous mortality")->flag_is_category();
  parameters_.Bind<Double>(PARAM_M, &m_input_, "The natural mortality rates for each category");
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_ratios_temp_, "The time step proportions for natural mortality");
  parameters_.Bind<bool>(PARAM_BIOMASS, &is_catch_biomass_, "Indicator to denote if the catches are as biomass (true) or abundance (false)")->set_default_value(true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The M-by-age ogives to apply to each category for natural mortality")
      ->set_alias_labels({PARAM_RELATIVE_M_BY_AGE});
  parameters_.Bind<Double>(PARAM_MAX_F, &max_F_, "The maximum F applied in a time-step")->set_default_value(4.0);
  parameters_.Bind<unsigned>(PARAM_F_ITERATIONS, &F_iterations_, "The number of iterations to tune the F coefficients")->set_default_value(4u);

  RegisterAsAddressable(PARAM_M, &m_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityHybrid::DoValidate() {
  parameters_.ValidateVector(PARAM_TIME_STEP_PROPORTIONS)->GreaterThanOrEqualTo(0.0);
  parameters_.ValidateVector(PARAM_M)->GreaterThanOrEqualTo(0.0)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.Validate(PARAM_MAX_F)->GreaterThanOrEqualTo(0.0);
  parameters_.Validate(PARAM_F_ITERATIONS)->GreaterThanOrEqualTo(1u);

  auto columns = method_table_->columns();
  if (std::find(columns.begin(), columns.end(), PARAM_AGE_WEIGHT_LABEL) != columns.end() && !is_catch_biomass_)
    LOG_FATAL_P(PARAM_BIOMASS) << "The parameter " << PARAM_BIOMASS << " must be true if an Age weight relationship is also used";

  BuildFisheryTables({{PARAM_ANNUAL_DURATION}});
}

/**
 * Parse the columns of a method table row specific to this process
 *
 * @param row The method table row
 * @param fishery The fishery to populate
 */
void MortalityHybrid::ParseFisheryColumns(const MethodRow& row, FisheryData& fishery) {
  if (!utilities::To<string, Double>(row[PARAM_ANNUAL_DURATION], fishery.annual_duration_))
    LOG_ERROR_P(PARAM_METHOD) << PARAM_ANNUAL_DURATION << " value " << row[PARAM_ANNUAL_DURATION] << " could not be converted to a Double";

  if ((fishery.annual_duration_ < 0) | (fishery.annual_duration_ > 1)) {
    LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << fishery.label_ << " had " << PARAM_ANNUAL_DURATION << " = " << fishery.annual_duration_ << " at row " << row.number_
                              << " This must be a value between 0 and 1.";
  }
  if (fishery.annual_duration_ != 1) {
    LOG_WARNING()
        << "The " << PARAM_ANNUAL_DURATION << " for fishery " << fishery.label_ << " was " << fishery.annual_duration_
        << ", not one. This is an uncommon assumption and this warning is to check that you understand it. Disregard this warning if you are comfortable with making this assumption.";
  }
}

/**
 * Check the columns specific to this process are the same on each row of a fishery
 *
 * @param row The method table row
 * @param stored The fishery from the row it was first seen on
 * @param repeat The fishery from this row
 * @param first_row The row the fishery was first seen on
 */
void MortalityHybrid::CheckRepeatedFishery(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) {
  if (stored.annual_duration_ != repeat.annual_duration_)
    LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << repeat.label_ << " had annual duration = " << repeat.annual_duration_ << " at row " << row.number_
                              << " but the same fishery had  annual duration = " << stored.annual_duration_ << " at row = " << first_row
                              << " these need to be consistent for a single fishery";
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 *
 * Validate any parameters that require information from other objects
 */
void MortalityHybrid::DoBuild() {
  LOG_TRACE();
  BindCategories();

  for (auto& category : categories_) {
    category.survivor_with_z_by_age_.assign(category.category_->age_spread(), 0.0);
    category.z_values_by_age_.assign(category.category_->age_spread(), 0.0);
    category.exp_values_half_duration_m_.assign(category.category_->age_spread(), 0.0);
  }

  BuildTimeStepRatios(false);
  ResolveFisheries();
  ResolveCategories();
  BuildSkippedFTimeSteps(PARAM_MORTALITY_HYBRID);
  AllocateRemovals(model()->age_spread());

  const vector<TimeStep*> ordered_time_steps = model()->managers()->time_step()->ordered_time_steps();
  // check fisheries in the same time-step have the same annual duration
  Double current_annual_value = 0.0;
  string last_fishery         = "";
  bool   first_fishery        = true;
  annual_duration_by_timestep_.resize(ordered_time_steps.size());
  for (unsigned time_step_iter = 0; time_step_iter < ordered_time_steps.size(); ++time_step_iter) {
    current_annual_value = 0.0;
    first_fishery        = true;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      if (fishery.time_step_index_ != time_step_iter)
        continue;
      if (first_fishery) {
        current_annual_value = fishery.annual_duration_;
        last_fishery         = fishery.label_;
        first_fishery        = false;
      } else {
        if (current_annual_value != fishery.annual_duration_)
          LOG_ERROR_P(PARAM_METHOD) << "Fishery " << fishery.label_ << " had an annual duration = " << fishery.annual_duration_ << ". Whereas fishery " << last_fishery
                                    << " had an annual duration value = " << current_annual_value
                                    << ". Fisheries in the same time-step have to have the same annual duration value.";
      }
    }
    annual_duration_by_timestep_[time_step_iter] = current_annual_value;
  }
}

/**
 * A time step without a fishery has an annual duration of zero in DoExecute(), so Z = 0 there and
 * its proportion of M is never applied. Stop the model unless the user chooses to carry on.
 *
 * @param model The model
 */
void MortalityHybrid::DoVerify(shared_ptr<Model> model) {
  vector<TimeStep*> time_steps = model->managers()->time_step()->ordered_time_steps();
  for (auto& ratio_iter : time_step_ratios_) {
    if (ratio_iter.second <= 0.0)
      continue;

    bool has_fishery = false;
    for (auto& fishery_iter : fisheries_) {
      if (fishery_iter.second.time_step_index_ == ratio_iter.first)
        has_fishery = true;
    }

    if (!has_fishery)
      LOG_VERIFY_P(PARAM_TIME_STEP_PROPORTIONS) << "time step " << time_steps[ratio_iter.first]->label() << " has a proportion of " << ratio_iter.second << " of M but no fishery in the "
                                                << PARAM_METHOD << " table. " << PARAM_MORTALITY_HYBRID << " does not apply M in a time step without a fishery, so this M is not applied."
                                                << " Use --verifylevel warning to run anyway";
  }
}

/**
 * Execute this process
 */
void MortalityHybrid::DoExecute() {
  LOG_TRACE();
  unsigned             time_step_index = model()->managers()->time_step()->current_time_step();
  unsigned             year            = model()->current_year();
  double               ratio           = time_step_ratios_[time_step_index];
  std::pair<bool, int> this_year_iter  = findInVector(process_years_, year);
  LOG_FINE() << "state = " << model()->state();
  LOG_FINE() << "Year = " << year << " time-step = " << time_step_index << " found? = " << this_year_iter.first;
  LOG_FINE() << "season duration = " << annual_duration_by_timestep_[time_step_index];
  Double selectivity_value = 0.0;
  // This is to do with M. so pre-allocate and only call the exp call once per category
  // per execute. will be used in initialisation and when Catch is not removed
  for (auto& category : categories_) {
    for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
      selectivity_value                       = category.selectivity_->GetAgeResult(category.category_->min_age_ + i, category.category_->age_length_);
      category.z_values_by_age_[i]            = annual_duration_by_timestep_[time_step_index] * ratio * (*category.m_) * selectivity_value;  // if no F this Z = M
      category.survivor_with_z_by_age_[i]     = 0.0;
      category.selectivity_values_[i]         = selectivity_value;
      category.exp_values_half_duration_m_[i] = exp(-0.5 * annual_duration_by_timestep_[time_step_index] * ratio * (*category.m_) * selectivity_value);  // this exp call should ony
      // LOG_FINEST() << "category " << category.category_label_ << " age index " << i << " selectivity " << selectivity_value;
    }
  }

  /**
   * This is where F gets applied, only enter if
   * - not in initialisation phase or no F in this time-step
   *  or if no F in this year or we are in projection years, as we probably want to project.
   * Loop for each category. Add the vulnerability from each
   * category in to the fisheries it belongs too
   */
  if (model()->state() != State::kInitialise) {
    // Not in initialisation phase
    LOG_FINE() << "Should we calculate F based on years on and time-step";
    if (((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model()->final_year()))) {
      // There is F in this year are we in the right time-step
      LOG_FINE() << "check time-step";
      if (find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index)
          == time_steps_to_skip_applying_F_mortality_.end()) {
        // Good to calculate F
        LOG_FINEST() << "time step = " << time_step_index << " not in initialisation and there is an F method in this timestep. year = " << model()->current_year();
        for (auto& fishery_category : fishery_categories_) {
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;

          for (unsigned i = 0; i < fishery_category.selectivity_values_.size(); ++i) {
            fishery_category.selectivity_values_[i]
                = fishery_category.selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
          }
        }
        // clear some variables
        for (auto& fishery : fisheries_) {
          fishery.second.vulnerability_ = 0.0;
        }

        Double total_vulnerable = 0.0;
        for (auto& fishery_category : fishery_categories_) {
          LOG_FINEST() << "checking fishery = " << fishery_category.fishery_label_;

          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;

          partition::Category* category = fishery_category.category_.category_;

          Double vulnerable = 0.0;
          if (fishery_category.category_.age_weight_) {
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              vulnerable += category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model()->min_age())
                            * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_duration_m_[i];
            }
          } else if (is_catch_biomass_) {  // as biomass
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              vulnerable += category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                            * fishery_category.category_.exp_values_half_duration_m_[i];
              Double sel_bio = category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i];
              Double Nmid    = category->data_[i] * fishery_category.category_.exp_values_half_duration_m_[i];
              LOG_FINEST() << "i = " << i << " Nmid = " << Nmid << " sel bio = " << sel_bio;
            }
          } else {  // as abundance
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_duration_m_[i];
            }
          }
          fishery_category.fishery_.vulnerability_ += vulnerable;
          total_vulnerable += vulnerable;
          LOG_FINEST() << "Category is fished in year " << year << " time_step " << time_step_index << ", numbers at age length = " << category->data_.size();
          LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << vulnerable;
        }
        LOG_FINE() << " total vulnerable = " << total_vulnerable;
        // Total from the catches rather than cached at build, as @project and @time_varying change them
        Double total_catch = 0.0;
        for (auto& fishery_iter : fisheries_) {
          if (fishery_iter.second.time_step_index_ == time_step_index)
            total_catch += fishery_iter.second.catches_[year];
        }
        // Start by doing a Pope's approximation
        for (auto& fishery_iter : fisheries_) {
          auto& fishery = fishery_iter.second;
          if (fishery.time_step_index_ != time_step_index)
            continue;
          fishery.init_popes_rate_ = fishery.catches_[year] / (fishery.vulnerability_ + 0.1 * fishery.catches_[year]);  //  Pope's rate  robust A.1.22 of SS appendix
          fishery.steep_jointer_
              = 1.0 / (1.0 + exp(30.0 * (fishery.init_popes_rate_ - 0.95)));  // steep logistic joiner at harvest rate of 0.95 //steep logistic joiner at harvest rate of 0.95
          fishery.popes_rate_ = fishery.steep_jointer_ * fishery.init_popes_rate_ + (1.0 - fishery.steep_jointer_) * 0.95;
          fishery.init_F_     = -log(1.0 - fishery.popes_rate_) / annual_duration_by_timestep_[time_step_index];
          LOG_FINEST() << "init_popes_rate_ = " << fishery.init_popes_rate_ << " vbio = " << fishery.vulnerability_ << " popes rate " << fishery.popes_rate_ << " init F "
                       << fishery.init_F_;
          // reset vulnerable
          fishery.vulnerability_ = 0.0;
        }
        //  Do a specified number of loops to tune up these F values to more closely match the observed catch
        Double interim_tot_catch;  // interim catch over all fisheries in this time-step
        for (unsigned f = 0; f < F_iterations_; ++f) {
          LOG_FINE() << "F-iter = " << f + 1;
          interim_tot_catch = 0.0;
          // set start z value to M for each category and iteration
          for (auto& category : categories_) {
            for (unsigned i = 0; i < category.category_->age_spread(); ++i) category.z_values_by_age_[i] = (*category.m_) * ratio * category.selectivity_values_[i];
          }
          // Use this F to calculate a Z for categories
          for (auto& fishery_category : fishery_categories_) {
            partition::Category* category = fishery_category.category_.category_;
            // Don't enter if this fishery is not executed in this time-step.
            if (fishery_category.fishery_.time_step_index_ != time_step_index)
              continue;
            // M + sum(F_f_a * S_f_a)
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.category_.z_values_by_age_[i] += fishery_category.fishery_.init_F_ * fishery_category.selectivity_values_[i];
            }
          }
          // The survivorship is calculated as:
          for (auto& category : categories_) {
            for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
              category.survivor_with_z_by_age_[i] = (1.0 - exp(-annual_duration_by_timestep_[time_step_index] * category.z_values_by_age_[i])) / category.z_values_by_age_[i];
              // LOG_FINEST() << " age = " << model_->min_age() << " Z " << category.z_values_by_age_[i] << " survivorship = " << category.survivor_with_z_by_age_[i];
            }
          }
          // Calculate the expected total catch that would occur with the current Hrates and Z
          for (auto& fishery_category : fishery_categories_) {
            partition::Category* category = fishery_category.category_.category_;
            // Don't enter if this fishery is not executed in this time-step.
            if (fishery_category.fishery_.time_step_index_ != time_step_index)
              continue;
            // M + sum(F_f_a * S_f_a)
            if (fishery_category.category_.age_weight_) {
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                interim_tot_catch += fishery_category.fishery_.init_F_ * category->data_[i]
                                     * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model()->min_age()) * fishery_category.selectivity_values_[i]
                                     * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else if (is_catch_biomass_) {  // as biomass
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                interim_tot_catch += fishery_category.fishery_.init_F_ * category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i)
                                     * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else {  // as abundance
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                interim_tot_catch
                    += fishery_category.fishery_.init_F_ * category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            }
          }
          LOG_FINEST() << "interim tot catch = " << interim_tot_catch << " observed total catch = " << total_catch;
          // make Z adjustments
          Double z_adjustment = total_catch / (interim_tot_catch + 0.0001);
          LOG_FINEST() << "Z adjustment = " << z_adjustment;
          // Update rates Z's with adjustment
          for (auto& category : categories_) {
            for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
              // Double old_F = category.z_values_by_age_[i] - ((*category.m_) * ratio * category.selectivity_values_[i]);
              // Double new_F = z_adjustment * (category.z_values_by_age_[i] - ((*category.m_) * ratio * category.selectivity_values_[i]));
              // LOG_FINEST() << "age = " << model_->min_age() + i << " old F " << old_F << " new F " << new_F;
              category.z_values_by_age_[i]        = (*category.m_) * ratio * category.selectivity_values_[i]
                                                    + z_adjustment * (category.z_values_by_age_[i] - ((*category.m_) * ratio * category.selectivity_values_[i]));
              category.survivor_with_z_by_age_[i] = (1.0 - exp(-annual_duration_by_timestep_[time_step_index] * category.z_values_by_age_[i])) / category.z_values_by_age_[i];
              // LOG_FINEST() << " age = " << model_->min_age() << " Z " << category.z_values_by_age_[i] << " survivorship " << category.survivor_with_z_by_age_[i];
            }
          }
          // Now re-calculate a new pope rate using a vulnerable biomass based
          // on the newly adjusted F
          for (auto& fishery_category : fishery_categories_) {
            if (fishery_category.fishery_.time_step_index_ != time_step_index)
              continue;
            partition::Category* category   = fishery_category.category_.category_;
            Double               vulnerable = 0.0;
            if (fishery_category.category_.age_weight_) {
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                vulnerable += category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model()->min_age())
                              * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else if (is_catch_biomass_) {  // as biomass
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                vulnerable += category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                              * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else {  // as abundance
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            }
            fishery_category.fishery_.vulnerability_ += vulnerable;
            // LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << vulnerable;
          }
          // Update Pope's approximation
          for (auto& fishery_iter : fisheries_) {
            auto& fishery = fishery_iter.second;
            if (fishery.time_step_index_ != time_step_index)
              continue;
            fishery.popes_rate_    = fishery.catches_[year] / (fishery.vulnerability_ + 0.0001);       //  Pope's rate  robust A.1.27 of SS appendix
            fishery.steep_jointer_ = 1.0 / (1.0 + exp(30.0 * (fishery.popes_rate_ - 0.95 * max_F_)));  // steep logistic joiner at harvest rate of 0.95 including F max
            fishery.init_F_        = fishery.steep_jointer_ * fishery.popes_rate_ + (1.0 - fishery.steep_jointer_) * max_F_;
            fishery.final_F_       = fishery.init_F_;
            LOG_FINEST() << "fishery = " << fishery.label_ << " observed catch = " << fishery.catches_[year] << " vulnerable " << fishery.vulnerability_
                         << " popes_rate_ = " << fishery.popes_rate_ << " join = " << fishery.steep_jointer_ << " new F " << fishery.init_F_;
            // reset vulnerable
            fishery.vulnerability_ = 0.0;
          }
        }  // F-tune
        // Update last Zrates and calculate catch at age and actual Catch
        for (auto& category : categories_) {
          for (unsigned i = 0; i < category.category_->age_spread(); ++i) category.z_values_by_age_[i] = (*category.m_) * ratio * category.selectivity_values_[i];
        }
        // Use this F to calculate a Z for categories
        for (auto& fishery_category : fishery_categories_) {
          partition::Category* category = fishery_category.category_.category_;
          // Don't enter if this fishery is not executed in this time-step.
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;
          // M + sum(F_f_a * S_f_a)
          for (unsigned i = 0; i < category->data_.size(); ++i)
            fishery_category.category_.z_values_by_age_[i] += fishery_category.fishery_.final_F_ * fishery_category.selectivity_values_[i];
        }
        // The survivorship is calculated as:
        for (auto& category : categories_) {
          for (unsigned i = 0; i < category.category_->age_spread(); ++i)
            category.survivor_with_z_by_age_[i] = (1.0 - exp(-annual_duration_by_timestep_[time_step_index] * category.z_values_by_age_[i])) / category.z_values_by_age_[i];
        }
        // Calculate Numbers at age and actual catch. actual_catches_ is summed over categories below, so
        // it starts from zero here: DoReset() isn't an option as time varying calls it during a run
        for (auto& fishery_iter : fisheries_) {
          if (fishery_iter.second.time_step_index_ == time_step_index)
            fishery_iter.second.actual_catches_[year] = 0.0;
        }
        for (auto& fishery_category : fishery_categories_) {
          partition::Category* category = fishery_category.category_.category_;
          // Don't enter if this fishery is not executed in this time-step.
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;
          // M + sum(F_f_a * S_f_a)
          if (fishery_category.category_.age_weight_) {
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.fishery_.actual_catches_[year] += fishery_category.fishery_.final_F_ * category->data_[i]
                                                                 * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model()->min_age())
                                                                 * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
            }
          } else if (is_catch_biomass_) {  // as biomass
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.fishery_.actual_catches_[year] += fishery_category.fishery_.final_F_ * category->data_[i]
                                                                 * category->age_length_->mean_weight(time_step_index, category->min_age_ + i)
                                                                 * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
            }
          } else {  // as abundance
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.fishery_.actual_catches_[year]
                  += fishery_category.fishery_.final_F_ * category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
            }
          }
          // Catch at age
          LOG_FINEST() << "Catch at age for year ndx = " << this_year_iter.second << " fishery ndx = " << fishery_category.fishery_.fishery_ndx_ << " category ndx "
                       << fishery_category.category_.category_ndx_;
          // Years after final_year have no slot in removals_by_year_fishery_category_
          if (this_year_iter.first) {
            for (unsigned i = 0; i < model()->age_spread(); ++i) {
              unsigned age_offset = category->min_age_ - model()->min_age();
              if (i < age_offset)
                continue;
              removals_by_year_fishery_category_[this_year_iter.second][fishery_category.fishery_.fishery_ndx_][fishery_category.category_.category_ndx_][i]
                  = fishery_category.fishery_.final_F_ * category->data_[i - age_offset] * fishery_category.selectivity_values_[i]
                    * fishery_category.category_.survivor_with_z_by_age_[i];
            }
          }
        }
        // Update Final F by year for each fishery for reporting
        for (auto& fishery_iter : fisheries_) {
          if (fishery_iter.second.time_step_index_ != time_step_index)
            continue;
          fishery_iter.second.F_by_year_[year] = fishery_iter.second.final_F_;
          LOG_FINE() << "save final F = " << fishery_iter.second.final_F_ << " fishery = " << fishery_iter.second.label_
                     << " reported value = " << fishery_iter.second.F_by_year_[year];
          LOG_FINE() << "actual catches = " << fishery_iter.second.actual_catches_[year];
          // Flag a penalty if it exists
          if (fishery_iter.second.penalty_)
            fishery_iter.second.penalty_->Trigger(fishery_iter.second.catches_[year], fishery_iter.second.actual_catches_[year]);
        }
      }
    }
  }  //!=Kinitialise
  // now kill the partition if No F then Z = M else Z = F + M
  for (auto& category : categories_) {
    for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
      category.category_->data_[i] *= exp(-category.z_values_by_age_[i]);
      LOG_FINEST() << "category " << category.category_label_ << ": updated numbers at age = " << category.category_->data_[i] << " age " << i + model()->min_age()
                   << " Z = " << category.z_values_by_age_[i];
    }
  }
}  // DoExecute

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityHybrid::FillReportCache(ostringstream& cache) {
  LOG_FINE();
  // This one is niggly because we need to iterate over each year and time step to print the right information so we don't
  // these years are for M and F
  cache << "year: ";
  for (auto year : process_years_) cache << year << " ";
  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    cache << "\ncatch[" << fishery.label_ << "]: ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    cache << "\nactual_catch[" << fishery.label_ << "]: ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
    cache << "\nf_by_year[" << fishery.label_ << "]: ";
    for (auto f_year : fishery.F_by_year_) cache << AS_DOUBLE(f_year.second) << " ";
  }
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 */
void MortalityHybrid::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    // print header
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto catches : fishery.catches_) cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_) cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
    }
    cache << REPORT_EOL;
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
  }
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
