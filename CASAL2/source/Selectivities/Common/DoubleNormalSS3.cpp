/**
 * @file DoubleNormalSS3.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2020 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "DoubleNormalSS3.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"
#include "../../TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
DoubleNormalSS3::DoubleNormalSS3(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_PEAK, &peak_, "beginning size (or age) for the plateau (in cm or year).", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_Y0, &first_sel_val_, "Selectivity at first bin, as logistic between 0 and 1", "")->set_range(-20, 0.0, true, true);
  parameters_.Bind<Double>(PARAM_Y1, &last_sel_val_, "Selectivity at last bin, as logistic between 0 and 1", "")->set_range(-20, 10.0, true, true);
  parameters_.Bind<Double>(PARAM_ASCENDING, &asc_width_, "Ascending width: parameter value is ln(width).", "");
  parameters_.Bind<Double>(PARAM_DESCENDING, &desc_width_, "Descending width: parameter value is ln(width).", "");
  parameters_.Bind<Double>(PARAM_WIDTH, &top_width_, "width of plateau, as logistic between peak and maximum length (or age)", "");
  parameters_.Bind<Double>(PARAM_L, &min_x_val_, "The low value (L)", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_H, &max_x_val_, "The high value (H)", "")->set_lower_bound(1.0, false);
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false); // The equivalent of a_max

  
  RegisterAsAddressable(PARAM_PEAK, &peak_);
  RegisterAsAddressable(PARAM_Y0, &first_sel_val_);
  RegisterAsAddressable(PARAM_Y1, &last_sel_val_);
  RegisterAsAddressable(PARAM_ASCENDING, &asc_width_);
  RegisterAsAddressable(PARAM_DESCENDING, &desc_width_);
  RegisterAsAddressable(PARAM_WIDTH, &top_width_);
  RegisterAsAddressable(PARAM_ALPHA, &alpha_);

}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * Then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void DoubleNormalSS3::DoValidate() {

}

/**
 * Reset this selectivity so it is ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age or length in the model.
 */
void DoubleNormalSS3::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    Double temp = 0.0;
    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      Double sel_val = 0.0;
      temp = age;
      // NOTE: NEED to get the model min and max age or min and max size, and the bin width (assumed to be 1 below)
      // min age is popn.state.col_min or get_int("min_age")
      // max age is popn.state.col_max or get_int("max_age")
      // length bins is popn.state.class_mins; min size is the value in the first bin and max size is the value in the last bin

      // from Adam Langley, sent on 2022-02-22
      if (temp > min_x_val_) {
        Double p1trans = peak_;
        // see also https://github.com/nmfs-stock-synthesis/stock-synthesis/blob/main/SS_selex.tpl, lines 436 and 504
        Double p2trans = p1trans + 1.0 + (0.99 + max_x_val_ - p1trans - 1.0) / (1.0 + exp(-1.0 * top_width_));
        Double p3trans = exp(asc_width_);
        Double p4trans = exp(desc_width_);
        Double p5trans = 1.0 / (1.0 + exp(-1.0 * first_sel_val_));
        Double p6trans = 1.0 / (1.0 + exp(-1.0 * last_sel_val_));

        Double x_p1trans   = temp - p1trans;
        Double x_p2trans   = temp - p2trans;
        Double asc         = exp(-1.0 * (x_p1trans * x_p1trans / p3trans));
        Double asc_scaled  = (p5trans + (1.0 - p5trans) * (asc - 0.0) / (1.0 - 0.0));
        Double desc        = exp(-1.0 * (x_p2trans * x_p2trans / p4trans));
        Double stj         = exp(-1.0 * ((40.0 - p2trans) * (40.0 - p2trans) / p4trans));
        Double desc_scaled = (1.0 + (p6trans - 1.0) * (desc - 1.0) /(stj - 1.0));
        Double join1       = 1.0 / (1.0 + exp(-1.0 * (20.0 * x_p1trans / (1.0 + fabs(x_p1trans)))));
        Double join2       = 1.0 / (1.0 + exp(-1.0 * (20.0 * x_p2trans / (1.0 + fabs(x_p2trans)))));
        sel_val = asc_scaled * (1.0 - join1) + join1 * ((1.0 - join2) + (desc_scaled * join2));
      } else {
        sel_val = exp(first_sel_val_);
      }
      values_[age - age_index_] = sel_val;
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<double> length_bins = model_->length_bins();
    Double         temp        = 0.0;
    Double         bin_width = 0.0;
    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      temp = length_bins[length_bin_index];
      Double sel_val = 0.0;
      if(model_->length_plus() & (length_bin_index < (length_bins.size() - 1)))
        bin_width = length_bins[length_bin_index + 1] - length_bins[length_bin_index];
      else if (model_->length_plus() & (length_bin_index == (length_bins.size() - 1))) {
        bin_width = model_->length_plus_group() - length_bins[length_bin_index];
      }
      if (temp > min_x_val_) {
        Double p1trans = peak_;
        // see also https://github.com/nmfs-stock-synthesis/stock-synthesis/blob/main/SS_selex.tpl, lines 436 and 504
        Double p2trans = p1trans + bin_width + (0.99 + max_x_val_ - p1trans - bin_width) / (1.0 + exp(-1.0 * top_width_));
        Double p3trans = exp(asc_width_);
        Double p4trans = exp(desc_width_);
        Double p5trans = 1.0 / (1.0 + exp(-1.0 * first_sel_val_));
        Double p6trans = 1.0 / (1.0 + exp(-1.0 * last_sel_val_));

        Double x_p1trans   = temp - p1trans;
        Double x_p2trans   = temp - p2trans;
        Double asc         = exp(-1.0 * (x_p1trans * x_p1trans / p3trans));
        Double asc_scaled  = (p5trans + (1.0 - p5trans) * (asc - 0.0) / (1.0 - 0.0));
        Double desc        = exp(-1.0 * (x_p2trans * x_p2trans / p4trans));
        Double stj         = exp(-1.0 * ((40.0 - p2trans) * (40.0 - p2trans) / p4trans));
        Double desc_scaled = (1.0 + (p6trans - 1.0) * (desc - 1.0) /(stj - 1.0));
        Double join1       = 1.0 / (1.0 + exp(-1.0 * (20.0 * x_p1trans / (1.0 + fabs(x_p1trans)))));
        Double join2       = 1.0 / (1.0 + exp(-1.0 * (20.0 * x_p2trans / (1.0 + fabs(x_p2trans)))));
        sel_val = asc_scaled * (1.0 - join1) + join1 * ((1.0 - join2) + (desc_scaled * join2));
      } else {
        sel_val = exp(first_sel_val_);
      }
      length_values_[length_bin_index] = sel_val;
    }
  }
}

/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @param year
 * @param time_step_index
 * @return Double selectivity for an age based on age length distribution_label
 */

Double DoubleNormalSS3::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  LOG_ERROR_P(PARAM_LENGTH_BASED) << ": This selectivity type has not been implemented for length-based selectivities in an age-based model";
  return 0.0;
}

} /* namespace selectivities */
} /* namespace niwa */
