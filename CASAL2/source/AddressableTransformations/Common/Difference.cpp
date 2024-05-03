/**
 * @file Difference.cpp
 * @author A Dunn
 * @github https://github.com/Zaita
 * @date May, 2023
 * @section LICENSE
 *
 *
 **/

// headers
#include "Difference.h"

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

/**
 * Default constructor
 */
Difference::Difference(shared_ptr<Model> model) : AddressableTransformation(model) {
  parameters_.Bind<Double>(PARAM_DIFFERENCE_PARAMETER, &difference_parameter_, "The difference between the parameters", "", false);

  RegisterAsAddressable(PARAM_PARAMETER, &first_parameter_);
  RegisterAsAddressable(PARAM_DIFFERENCE_PARAMETER, &difference_parameter_);
}
/**
 * Validate objects
 */
void Difference::DoValidate() {
  if (parameter_labels_.size() != 2) {  // must be 2
    LOG_ERROR_P(PARAM_PARAMETERS) << "The difference transformation can only transform 2 parameters at a time. You supplied " << parameter_labels_.size() << " parmaters";
  }
  restored_values_.resize(2, 0.0);
  // if -i use given values average_parameter_ and difference_parameter_ and restore.
  restored_values_[0] = first_parameter_;
  restored_values_[1] = first_parameter_ - difference_parameter_;

  // else use config addressables and calculate first_parameter_ and difference_parameter_
  first_parameter_  = init_values_[0];
  second_parameter_ = first_parameter_ - init_values_[1];
  LOG_FINE() << "first parameter = " << first_parameter_;
  LOG_FINE() << "difference parameter = " << difference_parameter_;

  restored_values_[0] = first_parameter_;
  restored_values_[1] = first_parameter_ - difference_parameter_;
}

/**
 * Build objects
 */
void Difference::DoBuild() {
  LOG_FINE() << "check values";
  // Check the transformations are correct
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) {
    LOG_FINE() << " i  = " << i << " restored = " << restored_values_[i];
  }

  restored_values_[0] = first_parameter_;
  restored_values_[1] = first_parameter_ - difference_parameter_;
  (this->*restore_function_)(restored_values_);
}
/**
 * Restore objects
 */
void Difference::DoRestore() {
  restored_values_[0] = first_parameter_;
  restored_values_[1] = first_parameter_ - difference_parameter_;
  LOG_MEDIUM() << restored_values_[0] << " " << restored_values_[1];
  (this->*restore_function_)(restored_values_);
}

/**
 * GetScore
 * @return log(Jacobian) if transformed with Jacobian, otherwise 0.0
 */
Double Difference::GetScore() {
  LOG_TRACE()
  // -ln(J) = 0.0
  jacobian_ = 0.0;
  return jacobian_;
}

/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Difference::PrepareForObjectiveFunction() {
  if (prior_applies_to_restored_parameters_)
    second_parameter_ = first_parameter_ - difference_parameter_;
}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Difference::RestoreForObjectiveFunction() {
  if (prior_applies_to_restored_parameters_)
    second_parameter_ = first_parameter_ - difference_parameter_;
}

/**
 * Report stuff for this transformation
 */
void Difference::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for (unsigned i = 0; i < restored_values_.size(); ++i) cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_PARAMETER << ": " << first_parameter_ << REPORT_EOL;
  cache << PARAM_SECOND_PARAMETER << ": " << second_parameter_ << REPORT_EOL;
  cache << PARAM_DIFFERENCE << ": " << difference_parameter_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}

/**
 * Report stuff for this transformation
 */
void Difference::FillTabularReportCache(ostringstream& cache, bool first_run) {
  LOG_FINEST() << "FillTabularReportCache";
  if (first_run) {
    cache << PARAM_PARAMETER << " " << PARAM_SECOND_PARAMETER << " " << PARAM_DIFFERENCE << " " << parameter_labels_[0] << " " << parameter_labels_[1] << " negative_log_jacobian"
          << REPORT_EOL;
  }
  cache << first_parameter_ << " " << second_parameter_ << " " << difference_parameter_ << " " << restored_values_[0] << " " << restored_values_[1] << " " << jacobian_
        << REPORT_EOL;
}
} /* namespace addressabletransformations */
} /* namespace niwa */
