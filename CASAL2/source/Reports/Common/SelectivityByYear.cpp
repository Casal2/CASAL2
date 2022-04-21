/**
 * @file SelectivityByYear.cpp
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */
#include "SelectivityByYear.h"

#include "../../Model/Model.h"
#include "../../Selectivities/Manager.h"

namespace niwa {
namespace reports {

/**
 *
 */
SelectivityByYear::SelectivityByYear() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;
  skip_tags_   = true;
  parameters_.Bind<string>(PARAM_SELECTIVITY, &selectivity_label_, "Selectivity label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step when to print the selectivity", "", "");


}

/**
 * Validate object
 */
void SelectivityByYear::DoValidate(shared_ptr<Model> model) {
  LOG_FINE() << " prepping report for " << label_;
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
  if (selectivity_label_ == "")
    selectivity_label_ = label_;
    
}
/**
 * Build object
 */
void SelectivityByYear::DoBuild(shared_ptr<Model> model) {
  selectivity_ = model->managers()->selectivity()->GetSelectivity(selectivity_label_);
  if (!selectivity_) {
#ifndef TESTMODE
    LOG_WARNING() << "The report for " << PARAM_SELECTIVITY << " with label '" << selectivity_label_ << "' was requested. This " << PARAM_SELECTIVITY
                  << " was not found in the input configuration file and the report will not be generated";
#endif
    is_valid_ = false;
#ifndef TESTMODE
  } else if (selectivity_->IsSelectivityLengthBased() & (model->partition_type() == PartitionType::kAge)) {
    LOG_WARNING() << "Cannot report the length-based selectivity values. This report (" << label_ << ") is being ignored. "
                  << "This can be done using the Casal2 R package. See the User Manual for more information";
#endif
    is_valid_ = false;
  }
}

void SelectivityByYear::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  if(model->partition_type() == PartitionType::kAge) {
    if (!selectivity_->IsSelectivityLengthBased()) {
      LOG_FINEST() << "Printing age-based selectivity";
      selectivity_ = model->managers()->selectivity()->GetSelectivity(selectivity_label_);

      cache_ << ReportHeader(type_, label_, format_);
      cache_ << "selectivity: " << selectivity_label_ << REPORT_EOL;
      cache_ << "year: " << model->current_year() << REPORT_EOL;
      const map<string, Parameter*> parameters = selectivity_->parameters().parameters();

      for (auto iter : parameters) {
        Parameter* x = iter.second;
        cache_ << iter.first << ": ";

        vector<string> values = x->current_values();
        for (string value : values) cache_ << value << " ";
        cache_ << REPORT_EOL;
      }

      cache_ << "Values " << REPORT_R_VECTOR << REPORT_EOL;
      for (unsigned i = model->min_age(); i <= model->max_age(); ++i) cache_ << i << " " << AS_DOUBLE(selectivity_->GetAgeResult(i, nullptr)) << "\n";
      cache_ << REPORT_END << REPORT_EOL;
      ready_for_writing_ = true;
    }
  } else if(model->partition_type() == PartitionType::kLength) {
    LOG_FINEST() << "Printing age-based selectivity";
    selectivity_ = model->managers()->selectivity()->GetSelectivity(selectivity_label_);

    cache_ << ReportHeader(type_, label_, format_);
    cache_ << "year: " << model->current_year() << REPORT_EOL;
    cache_ << "selectivity: " << selectivity_label_ << REPORT_EOL;
    const map<string, Parameter*> parameters = selectivity_->parameters().parameters();

    for (auto iter : parameters) {
      Parameter* x = iter.second;
      cache_ << iter.first << ": ";

      vector<string> values = x->current_values();
      for (string value : values) cache_ << value << " ";
      cache_ << REPORT_EOL;
    }

    cache_ << "Values " << REPORT_R_VECTOR << REPORT_EOL;
    for (unsigned i = 0; i < model->get_number_of_length_bins(); ++i)
      cache_ << model->length_bin_mid_points()[i] << " " << AS_DOUBLE(selectivity_->GetLengthResult(i)) << "\n";
    cache_ << REPORT_END << REPORT_EOL;
    ready_for_writing_ = true;
  }

}
/*
* write the header for tabular report
*/
void SelectivityByYear::DoPrepareTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;
  if(model->partition_type() == PartitionType::kAge) {
    if (!selectivity_->IsSelectivityLengthBased()) {
      cache_ << ReportHeader(type_, label_, format_);
      cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
      string age, selectivity_by_age_label;

      for (unsigned i = model->min_age(); i <= model->max_age(); ++i) {
        if (!utilities::To<unsigned, string>(i, age))
          LOG_CODE_ERROR() << "Could not convert the value " << i << " to a string for storage in the tabular report";
        selectivity_by_age_label = "selectivity[" + selectivity_->label() + "]." + age;
        cache_ << selectivity_by_age_label << " ";
      }
      cache_ << REPORT_EOL;
    }
  } else if(model->partition_type() == PartitionType::kLength) {
      cache_ << ReportHeader(type_, label_, format_);
      cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
      string length, selectivity_by_length_label;

      for (auto length_mid_vals : model->length_bin_mid_points()) {
        if (!utilities::To<double, string>(length_mid_vals, length))
          LOG_CODE_ERROR() << "Could not convert the value " << length_mid_vals << " to a string for storage in the tabular report";
        selectivity_by_length_label = "selectivity[" + selectivity_->label() + "]." + length;
        cache_ << selectivity_by_length_label << " ";
      }
      cache_ << REPORT_EOL;
  }

}

void SelectivityByYear::DoExecuteTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;
  if(model->partition_type() == PartitionType::kAge) {
    if (!selectivity_->IsSelectivityLengthBased()) {
      for (unsigned i = model->min_age(); i <= model->max_age(); ++i) {
        cache_ << AS_DOUBLE(selectivity_->GetAgeResult(i, nullptr)) << " ";
      }
      cache_ << REPORT_EOL;
    }
  } else if(model->partition_type() == PartitionType::kLength) {
      for (unsigned i = 0; i < model->get_number_of_length_bins(); ++i) {
        cache_ << AS_DOUBLE(selectivity_->GetLengthResult(i)) << " ";
      }
      cache_ << REPORT_EOL;
  }

}

void SelectivityByYear::DoFinaliseTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
