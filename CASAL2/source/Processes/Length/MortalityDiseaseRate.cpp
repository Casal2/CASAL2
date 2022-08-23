/**
 * @file MortalityDiseaseRate.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 * @description
 * This class applies an exponential mortality rate to certain years
 */

// Headers
#include "MortalityDiseaseRate.h"

#include <numeric>

#include "../../Categories/Categories.h"
#include "../../TimeSteps/Manager.h"
#include "Selectivities/Manager.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {

/**
 * Default constructor
 */
MortalityDiseaseRate::MortalityDiseaseRate(shared_ptr<Model> model) : Process(model), partition_(model) {
  LOG_TRACE();
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kLength;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories labels", "");
  parameters_.Bind<Double>(PARAM_DISEASE_MORTALITY_RATE, &dm_input_, "The Disease mortality rate", "")->set_range(0.0, 10, true, true);
  parameters_.Bind<Double>(PARAM_YEAR_EFFECTS, &year_effect_input_, "Annual deviations around the disease mortality rate", "")->set_lower_bound(0.0, true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The selectivity labels", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &process_years_, "Years to apply disease mortality in", "");


  RegisterAsAddressable(PARAM_YEAR_EFFECTS, &year_effect_by_year_);
  RegisterAsAddressable(PARAM_DISEASE_MORTALITY_RATE, &dm_);
}

/**
 * Validate the Mortality Constant Rate process
 *
 * - Validate the required parameters
 * - Assign the label from the parameters
 * - Assign and validate remaining parameters
 * - Duplicate 'm' and 'selectivities' if only one value specified
 * - Check the categories are real
 */
void MortalityDiseaseRate::DoValidate() {
  if(year_effect_input_.size() != process_years_.size()) {
    LOG_ERROR_P(PARAM_YEAR_EFFECTS) << "need to supply a value for each year the process is applied. You supplied '" << year_effect_input_.size() << "' but '" << process_years_.size() << "' years";
  }

  if (dm_input_.size() == 1) {
    auto val_m = dm_input_[0];
    dm_input_.assign(category_labels_.size(), val_m);
  }

  if (selectivity_names_.size() == 1) {
    auto val_sel = selectivity_names_[0];
    selectivity_names_.assign(category_labels_.size(), val_sel);
  }

  if (dm_input_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_DISEASE_MORTALITY_RATE) << ": The number of rates provided (" << dm_input_.size() << ") does not match the number of categories provided (" << category_labels_.size() << ").";
  }

  if (selectivity_names_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES) << ": The number of selectivity ogives provided (" << selectivity_names_.size() << ") does not match the number of categories provided ("
                                         << category_labels_.size() << ").";
  }

  for (unsigned i = 0; i < dm_input_.size(); ++i) 
    dm_[category_labels_[i]] = dm_input_[i];
  for (unsigned i = 0; i < year_effect_input_.size(); ++i) 
    year_effect_by_year_[process_years_[i]] = year_effect_input_[i];
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
  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": selectivities  ogive label " << label << " was not found.";
    selectivities_.push_back(selectivity);
  }

  results_.resize(process_years_.size());
  for(unsigned i = 0; i < process_years_.size(); i++) {
    results_[i].resize(category_labels_.size());
    for(unsigned j = 0; j < category_labels_.size(); j++) {
      results_[i][j].resize(model_->get_number_of_length_bins(), 0.0);
    }
  }
}

/**
 * Execute the process
 */
void MortalityDiseaseRate::DoExecute() {
  LOG_FINE() << "MortalityDiseaseRate::DoExecute()";
  if(model_->state() != State::kInitialise) {
    unsigned year = model_->current_year();
    if(find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) {
      auto it = std::find(process_years_.begin(), process_years_.end(), year);
      unsigned year_ndx = 0;
      if (it != process_years_.end()) {
        year_ndx = distance(process_years_.begin(), it);
      } else {
        LOG_CODE_ERROR() << "couldn't find year, this should have been caught in the above if statement";
      }

      LOG_FINE() << "apply disease mortality in regular year " << year;
      
      // get the ratio to apply first
      unsigned i = 0;
      for (auto category : partition_) {
        Double dm = dm_[category->name_];
        Double   amount = 0.0;
        unsigned j = 0;
        LOG_FINEST() << "category " << category->name_;
        // StoreForReport(category->name_ + " ratio", ratio);
        for (Double& data : category->data_) {
          amount = data * (1 - exp(-selectivities_[i]->GetLengthResult(j) * dm * year_effect_by_year_[year]));
          results_[year_ndx][i][j] = amount;
          LOG_FINEST() << "j = " << j << " data " << data << " dm " << dm << " Year effect " << year_effect_by_year_[year];
          data *=  exp(-selectivities_[i]->GetLengthResult(j) * dm * year_effect_by_year_[year]);
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
  for(auto dm_val :dm_) {
    dm_input_[counter] = dm_val.second;
    counter++;
  }
  counter = 0;
  for(auto year_val : year_effect_by_year_) {
    year_effect_input_[counter] = year_val.second;
    counter++;
  }
}
/**
 * Fill the report cache
 */
void MortalityDiseaseRate::FillReportCache(ostringstream& cache) {
  cache << "year: ";
  for (auto iter : year_effect_by_year_) 
    cache << iter.first << " ";
  cache << "\nyear_effect: ";
  for (auto iter : year_effect_by_year_) 
    cache << AS_DOUBLE(iter.second) << " ";
    
  cache << "\n" << PARAM_DISEASE_MORTALITY_RATE << " " << REPORT_R_DATAFRAME  << REPORT_EOL;
  cache << "category " << PARAM_DISEASE_MORTALITY_RATE << REPORT_EOL;
  for (auto iter : dm_) {
    cache << iter.first << " " << iter.second << REPORT_EOL;
  }
  // print results
  for(unsigned year_ndx = 0; year_ndx < process_years_.size(); year_ndx++) {
    cache << "Abundance_removed_in_year_" << process_years_[year_ndx] << " " << REPORT_R_DATAFRAME_ROW_LABELS << REPORT_EOL;
    // header
    cache << "category ";
    for(unsigned i = 0; i < model_->get_number_of_length_bins(); i++) 
      cache << model_->length_bin_mid_points()[i] << " ";
    cache << REPORT_EOL;
    // into it
    for(unsigned j = 0; j < category_labels_.size(); j++) {
      cache << category_labels_[j] << " ";
      for(unsigned i = 0; i < results_[year_ndx][j].size(); i++) 
        cache << results_[year_ndx][j][i] << " ";
      cache << REPORT_EOL;  
    }
  }
}



/**
 * Fill the tabular report cache
 */
void MortalityDiseaseRate::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {

  }
}



} /* namespace length */
} /* namespace processes */
} /* namespace niwa */
