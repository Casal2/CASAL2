/**
 * @file Selectivity.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */
#include "Selectivity.h"

#include "../../Model/Model.h"
#include "../../Selectivities/Manager.h"

namespace niwa {
namespace reports {

/**
 *
 */
Selectivity::Selectivity() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);
  parameters_.Bind<string>(PARAM_SELECTIVITY, &selectivity_label_, "Selectivity name", "", "");
  parameters_.Bind<double>(PARAM_LENGTH_VALUES, &length_values_, "Length values to evaluate the a length-based selectivity in an age based model.", "", true);

}

/**
 * Validate object
 */
void Selectivity::DoValidate(shared_ptr<Model> model) {
  if(!model->global_configuration().print_tabular()) {
    if (selectivity_label_ == "") 
      selectivity_label_ = label_;
  }
}
/**
 * Build object
 */
void Selectivity::DoBuild(shared_ptr<Model> model) {
  LOG_FINE() << "getting selectivity = " << label_;
  if (selectivity_label_ != "")  {
    selectivity_ = model->managers()->selectivity()->GetSelectivity(selectivity_label_);
    if (!selectivity_) {
  #ifndef TESTMODE
      LOG_WARNING() << "The report for " << PARAM_SELECTIVITY << " with label '" << selectivity_label_ << "' was requested. This " << PARAM_SELECTIVITY
                    << " was not found in the input configuration file and the report will not be generated";
  #endif
      is_valid_ = false;
    } else {
      if(selectivity_->IsSelectivityLengthBased()) {
        LOG_FINE() <<" length based";
        if(!parameters_.Get(PARAM_LENGTH_VALUES)->has_been_defined()) {
          LOG_ERROR_P(PARAM_SELECTIVITY) << " this is a length-based selectivity in an age based model. If you want to report this you need to supply the subcommand " << PARAM_LENGTH_VALUES;
        }
      }
    }
  }
}

void Selectivity::DoExecute(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  LOG_TRACE();
  if(model->partition_type() == PartitionType::kAge) {
    LOG_FINEST() << "Printing age-based selectivity";
    cache_ << ReportHeader(type_, selectivity_label_, format_);
    const map<string, Parameter*> parameters = selectivity_->parameters().parameters();

    for (auto iter : parameters) {
      Parameter* x = iter.second;
      cache_ << iter.first << ": ";

      vector<string> values = x->current_values();
      for (string value : values) cache_ << value << " ";
      cache_ << REPORT_EOL;
    }
    cache_ << "Values " << REPORT_R_VECTOR << REPORT_EOL;

    if (!selectivity_->IsSelectivityLengthBased()) {
      for (unsigned i = model->min_age(); i <= model->max_age(); ++i) 
        cache_ << i << " " << AS_DOUBLE(selectivity_->GetAgeResult(i, nullptr)) << "\n";
      ready_for_writing_ = true;
    } else {
      LOG_FINE() << "calculate length based";
      for(unsigned i = 0; i < length_values_.size(); i++) 
        cache_ << length_values_[i] << " " << AS_DOUBLE(selectivity_->get_value(length_values_[i])) << "\n";
      ready_for_writing_ = true;  
    }
  } else if(model->partition_type() == PartitionType::kLength) {
    LOG_FINEST() << "Printing age-based selectivity";
    cache_ << ReportHeader(type_, selectivity_label_, format_);
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
    ready_for_writing_ = true;
  }
}
/*
* write the header for tabular report
*/
void Selectivity::DoPrepareTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;
  LOG_FINE() << "prepare tabular";
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
  if (selectivity_label_ != "") { 
    LOG_FINE() << "given a specific selectivity " << selectivity_label_;
    if(model->partition_type() == PartitionType::kAge) {
      if (!selectivity_->IsSelectivityLengthBased()) {
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
      string length, selectivity_by_length_label;
      for (auto length_mid_vals : model->length_bin_mid_points()) {
        if (!utilities::To<double, string>(length_mid_vals, length))
          LOG_CODE_ERROR() << "Could not convert the value " << length_mid_vals << " to a string for storage in the tabular report";
        selectivity_by_length_label = "selectivity[" + selectivity_->label() + "]." + length;
        cache_ << selectivity_by_length_label << " ";
      }
      cache_ << REPORT_EOL;
    }
  } else {
    LOG_FINE() << "all selectivities printing";
    selectivities::Manager& SelectivityManager = *model->managers()->selectivity();
    for (auto object : SelectivityManager.objects()) {
      if(model->partition_type() == PartitionType::kAge) {
        if (!object->IsSelectivityLengthBased()) {;
          string age, selectivity_by_age_label;

          for (unsigned i = model->min_age(); i <= model->max_age(); ++i) {
            if (!utilities::To<unsigned, string>(i, age))
              LOG_CODE_ERROR() << "Could not convert the value " << i << " to a string for storage in the tabular report";
            selectivity_by_age_label = "selectivity[" + object->label() + "]." + age;
            cache_ << selectivity_by_age_label << " ";
          }
          cache_ << REPORT_EOL;
        }
      } else if(model->partition_type() == PartitionType::kLength) {
        string length, selectivity_by_length_label;
        for (auto length_mid_vals : model->length_bin_mid_points()) {
          if (!utilities::To<double, string>(length_mid_vals, length))
            LOG_CODE_ERROR() << "Could not convert the value " << length_mid_vals << " to a string for storage in the tabular report";
          selectivity_by_length_label = "selectivity[" + object->label() + "]." + length;
          cache_ << selectivity_by_length_label << " ";
        }
        cache_ << REPORT_EOL;
      }
    }
  }
}

void Selectivity::DoExecuteTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;
  if (selectivity_label_ != "") { 
    LOG_FINE() << "given a specific selectivity";
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
  } else {
    LOG_FINE() << "all selectivities printing";
    selectivities::Manager& SelectivityManager = *model->managers()->selectivity();
    for (auto object : SelectivityManager.objects()) {
      if(model->partition_type() == PartitionType::kAge) {
        if (!object->IsSelectivityLengthBased()) {;
          for (unsigned i = model->min_age(); i <= model->max_age(); ++i) {
            cache_ << AS_DOUBLE(object->GetAgeResult(i, nullptr)) << " ";
          }
          cache_ << REPORT_EOL;
        }
      } else if(model->partition_type() == PartitionType::kLength) {
        for (unsigned i = 0; i < model->get_number_of_length_bins(); ++i) {
          cache_ << AS_DOUBLE(object->GetLengthResult(i)) << " ";
        }
        cache_ << REPORT_EOL;
      }
    }
  }
}

void Selectivity::DoFinaliseTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
