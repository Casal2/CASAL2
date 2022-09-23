/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/10/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Partition.h"

#include "../../Categories/Categories.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Partition/Category.h"
#include "../../Partition/Partition.h"
#include "../../Utilities/Math.h"

// namespaces
namespace niwa {
namespace asserts {

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
Partition::Partition(shared_ptr<Model> model) : Assert(model) {
  parameters_.Bind<string>(PARAM_CATEGORY, &category_label_, "Category to check population values for", "", "");
  parameters_.Bind<Double>(PARAM_VALUES, &values_, "Values expected in the partition", "");
}

/**
 * Populate any parameters
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void Partition::DoValidate() {
  if (category_label_ == "")
    category_label_ = label_;
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void Partition::DoBuild() {
  model_->Subscribe(State::kFinalise, this);  // Note this compares the partition at the end of the model run, i.e., the final state

  if (!model_->categories()->IsValid(category_label_))
    LOG_FATAL_P(PARAM_CATEGORY) << " category " << category_label_ << " is not a valid category";
}

/**
 * Execute/Run/Process the object.
 */
void Partition::Execute() {
  std::streamsize prec = std::cout.precision();
  std::cout.precision(12);

  auto category = &model_->partition().category(category_label_);
  auto data     = category->data_;

  if (data.size() != values_.size())
    LOG_FATAL_P(PARAM_VALUES) << ": number of values provided (" << values_.size() << ") does not match partition size (" << data.size() << ")";

  for (unsigned i = 0; i < values_.size(); ++i) {
    Double diff = values_[i] - data[i];
    if (error_type_ == PARAM_ERROR && (!utilities::math::IsBasicallyEqual(values_[i], data[i], tol_))) {
      LOG_ERROR() << "Assert Failure: The Assert with label '" << label_ << "' and Partition.Category " << category_label_ << " has value " << data[i] << " and " << values_[i]
                  << " was expected for element " << i + 1 << ". The difference was " << diff;
    } else if (!utilities::math::IsBasicallyEqual(values_[i], data[i], tol_)) {
      LOG_WARNING() << "Assert Failure: The Assert with label '" << label_ << "' and Partition.Category " << category_label_ << " has value " << data[i] << " and " << values_[i]
                    << " was expected for element " << i + 1 << ". The difference was " << diff;
    } else {
      LOG_INFO() << "Assert Passed: The Assert with label '" << label_ << "' and Partition.Category " << category_label_ << " has value " << data[i] << " and " << values_[i]
                 << " was expected for element " << i + 1 << ".";
    }
  }

  std::cout.precision(prec);
}

} /* namespace asserts */
} /* namespace niwa */
