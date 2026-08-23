/**
 * @file MortalityDiseaseRate.cpp
 * @author  C.Marsh/A. Dunn
 * @version 1.0
 * @date 2023
 * @section LICENSE
 * @description
 * This class applies an exponential mortality rate to certain years. A copy of the length based process
 */

// Headers
#include "MortalityDiseaseRate.h"

#include <numeric>

#include "../../../Categories/Categories.h"
#include "../../../Selectivities/Manager.h"
#include "../../../TimeSteps/Manager.h"
#include "MortalityRateFormulas.h"

// Namespaces
namespace niwa::processes::common {

/**
 * Default constructor
 */
MortalityDiseaseRate::MortalityDiseaseRate(shared_ptr<Model> model) : MortalityRateBase(model) {
  LOG_TRACE();
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge | PartitionType::kLength;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories labels")->flag_is_category();
  parameters_.Bind<Double>(PARAM_DISEASE_MORTALITY_RATE, &dm_input_, "The disease mortality rate");
  parameters_.Bind<Double>(PARAM_YEAR_EFFECTS, &year_effect_input_, "Annual deviations around the disease mortality rate");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The selectivity labels");
  parameters_.Bind<unsigned>(PARAM_YEARS, &process_years_, "Years to apply disease mortality in");

  RegisterAsAddressable(PARAM_YEAR_EFFECTS, &year_effect_by_year_);
  RegisterAsAddressable(PARAM_DISEASE_MORTALITY_RATE, &dm_);
}

/**
 * Validate the Mortality Disease Rate process
 */
void MortalityDiseaseRate::DoValidate() {
  parameters_.ValidateVector(PARAM_DISEASE_MORTALITY_RATE)
      ->GreaterThanOrEqualTo(0.0)
      ->LessThanOrEqualTo(10.0)
      ->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)
      ->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_YEAR_EFFECTS)->GreaterThanOrEqualTo(0.0)->ExpandToSameNumberOfElementsAs(PARAM_YEARS)->SameNumberOfElementsAs(PARAM_YEARS);
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_YEARS)->IsModelYear();

  dm_                  = utilities::OrderedMap<string, Double>::create(category_labels_, dm_input_);
  year_effect_by_year_ = utilities::Map::create(process_years_, year_effect_input_);
}

/**
 * Build any runtime relationships
 *
 * - Build the partition accessor
 * - Build the list of selectivities
 * - Build the ratios for the number of time steps
 */
void MortalityDiseaseRate::DoBuild() {
  LOG_FINE() << "MortalityDiseaseRate::DoBuild()";
  partition_.Init(category_labels_);
  selectivities_ = ResolveSelectivities(model(), parameters(), selectivity_names_, PARAM_SELECTIVITIES, ": The selectivity with label ");

  results_.resize(process_years_.size());
  vector<unsigned> category_bin_counts;
  category_bin_counts.reserve(partition_.size());
  for (auto category : partition_) {
    category_bin_counts.push_back(category->data_.size());
  }

  for (unsigned i = 0; i < process_years_.size(); i++) {
    results_[i].resize(category_bin_counts.size());
    for (unsigned j = 0; j < category_bin_counts.size(); j++) {
      results_[i][j].resize(category_bin_counts[j], 0.0);
    }
  }
}

/**
 * Execute the process
 */
void MortalityDiseaseRate::DoExecute() {
  LOG_FINE() << "MortalityDiseaseRate::DoExecute()";
  if (model()->state() != State::kInitialise) {
    unsigned year = model()->current_year();
    if (find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) {
      auto     it       = std::find(process_years_.begin(), process_years_.end(), year);
      unsigned year_ndx = 0;
      if (it != process_years_.end()) {
        year_ndx = distance(process_years_.begin(), it);
      } else {
        LOG_CODE_ERROR() << "Unable to find year, this should have been caught in the above if statement";
      }

      LOG_FINE() << "apply disease mortality in year " << year;

      // get the ratio to apply first
      unsigned i = 0;
      for (auto category : partition_) {
        Double   dm     = dm_[category->name_];
        Double   amount = 0.0;
        unsigned j      = 0;
        LOG_FINEST() << "category " << category->name_;
        // StoreForReport(category->name_ + " ratio", ratio);
        for (Double& data : category->data_) {
          Double selectivity_value
              = (process_profile_ == ProcessProfile::kAge) ? selectivities_[i]->GetAgeResult(category->min_age_ + j, category->age_length_) : selectivities_[i]->GetLengthResult(j);
          amount = CalculateRemoval(RemovalFormulation::kExponentialDecay, data, dm, selectivity_value, year_effect_by_year_[year]);
          if (i >= results_[year_ndx].size() || j >= results_[year_ndx][i].size()) {
            LOG_CODE_ERROR() << "Results index out of range for year index " << year_ndx << ", category index " << i << ", bin index " << j;
          } else {
            results_[year_ndx][i][j] = amount;
          }
          LOG_FINEST() << "j = " << j << " data " << data << " dm " << dm << " Year effect " << year_effect_by_year_[year];
          data -= amount;
          ++j;
        }
        ++i;
      }
    }
  }
}

/**
 * Reset the Mortality Process
 */
void MortalityDiseaseRate::DoReset() {
  unsigned counter = 0;
  for (auto dm_val : dm_) {
    dm_input_[counter] = dm_val.second;
    counter++;
  }
  counter = 0;
  for (auto year_val : year_effect_by_year_) {
    year_effect_input_[counter] = year_val.second;
    counter++;
  }
}
/**
 * Fill the report cache
 */
void MortalityDiseaseRate::FillReportCache(ostringstream& cache) {
  cache << "year: ";
  for (auto iter : year_effect_by_year_) cache << iter.first << " ";
  cache << "\nyear_effect: ";
  for (auto iter : year_effect_by_year_) cache << AS_DOUBLE(iter.second) << " ";

  cache << "\n" << PARAM_DISEASE_MORTALITY_RATE << " " << REPORT_R_DATAFRAME << REPORT_EOL;
  cache << "category " << PARAM_DISEASE_MORTALITY_RATE << REPORT_EOL;
  for (auto iter : dm_) {
    cache << iter.first << " " << iter.second << REPORT_EOL;
  }
  // print results
  for (unsigned year_ndx = 0; year_ndx < process_years_.size(); year_ndx++) {
    cache << "Abundance_removed_in_year_" << process_years_[year_ndx] << " " << REPORT_R_DATAFRAME_ROW_LABELS << REPORT_EOL;
    // header
    cache << "category ";
    if (process_profile_ == ProcessProfile::kAge) {
      for (unsigned i = 0; i < model()->age_spread(); i++) cache << model()->min_age() + i << " ";
    } else {
      for (unsigned i = 0; i < model()->get_number_of_length_bins(); i++) cache << model()->length_bin_mid_points()[i] << " ";
    }
    cache << REPORT_EOL;
    // into it
    for (unsigned j = 0; j < category_labels_.size(); j++) {
      cache << category_labels_[j] << " ";
      for (unsigned i = 0; i < results_[year_ndx][j].size(); i++) cache << results_[year_ndx][j][i] << " ";
      cache << REPORT_EOL;
    }
  }
}

/**
 * Fill the tabular report cache
 */
void MortalityDiseaseRate::FillTabularReportCache(ostringstream& cache, bool first_run) {}

}  // namespace niwa::processes::common
