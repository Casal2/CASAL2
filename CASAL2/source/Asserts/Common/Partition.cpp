/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/10/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Partition.h"

#include "Model/Model.h"
#include "Model/Objects.h"
#include "Categories/Categories.h"
#include "Partition/Category.h"
#include "Partition/Partition.h"
#include "Utilities/DoubleCompare.h"

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
Partition::Partition(Model* model) : Assert(model) {
  parameters_.Bind<string>(PARAM_CATEGORY, &category_label_, "Category to check population values for", "", "");
  parameters_.Bind<double>(PARAM_VALUES, &values_, "Values expected in the partition", "");
}

/**
 * Populate any parameters,
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
  model_->Subscribe(State::kFinalise, this);

  if (!model_->categories()->IsValid(category_label_))
    LOG_FATAL_P(PARAM_CATEGORY) << " category " << category_label_ << " is not a valid category";
}

/**
 * Execute/Run/Process the object.
 */
void Partition::Execute() {
  auto category = model_->partition().category(category_label_);

  auto data = category.data_;
  if (data.size() != values_.size())
    LOG_FATAL_P(PARAM_VALUES) << ": number of values provided (" << values_.size() << ") does not match partition size (" << data.size() << ")";

  for (unsigned i = 0; i < values_.size(); ++i) {
    if (!utilities::doublecompare::IsBasicallyEqual(values_[i], data[i])) {
      std::streamsize prec = std::cout.precision();
      std::cout.precision(9);

      LOG_ERROR() << "Assert Failure: Partition.Category: " << category_label_ << " had value " << data[i] << " when " << values_[i]
        << " values are expected for age " << (category.min_age_ + i);

      std::cout.precision(prec);
    }
  }
}

} /* namespace asserts */
} /* namespace niwa */
