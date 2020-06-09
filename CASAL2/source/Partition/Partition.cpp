/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 7/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// defines
#define _USE_MATH_DEFINES

// Headers
#include "Partition.h"

#include <cmath>

#include "AgeLengths/AgeLength.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Logging/Logging.h"

#include "Utilities/Math.h"
// Namespaces
namespace niwa {

/**
 * Destructor
 */
Partition::~Partition() {
  for (auto partition : partition_) {
    if (partition.second != nullptr)
      delete partition.second;
  }
}

/**
 * Validate the objects
 */
void Partition::Validate() {
}

/**
 * Build the partition structure. This involves getting
 * the category information and building the raw structure.
 *
 * The range of years that each category has will be addressed
 * with the accessor objects.
 */
void Partition::Build() {
  Categories* categories        = model_->categories();
  vector<string> category_names = categories->category_names();

  for(string category : category_names) {
    LOG_FINEST() << "Adding category " << category << " to the partition";

    partition::Category* new_category = new partition::Category(model_);
    new_category->name_       = category;
    new_category->min_age_    = categories->min_age(category);
    new_category->max_age_    = categories->max_age(category);
    new_category->years_      = categories->years(category);
    new_category->age_length_ = categories->age_length(category);

    if (model_->partition_type() == PartitionType::kAge) {
      unsigned age_spread = (categories->max_age(category) - categories->min_age(category)) + 1;
      LOG_FINEST() << "resizing data_ to " << age_spread;
      new_category->data_.resize(age_spread, 0.0);

      // Allocate memory for our age_length matrix
      if (model_->length_bins().size() > 0) {
        unsigned length_bin_count = model_->length_plus() == true ? model_->length_bins().size() : model_->length_bins().size() - 1;

        unsigned year_count      = model_->year_spread();
        unsigned time_step_count = model_->time_steps().size();

        new_category->age_length_matrix_.resize(year_count);

        for (unsigned year = 0; year < year_count; ++year) {
          new_category->age_length_matrix_[year].resize(time_step_count);
          for (unsigned step_iter = 0; step_iter < time_step_count; ++step_iter) {
            new_category->age_length_matrix_[year][step_iter].resize(new_category->age_spread());
            for (unsigned age = 0; age < new_category->age_spread(); ++age) {
              new_category->age_length_matrix_[year][step_iter][age].resize(length_bin_count);
            }
          }
        }
      }

    } else if (model_->partition_type() == PartitionType::kLength) {
      unsigned length_bins = model_->length_bins().size();
      new_category->data_.resize(length_bins, 0.0);
    }

    partition_[category] = new_category;
  }
}

/**
 * This method builds the mean_length_by_time_step_age_ object on each of the categories.
 *
 * This method is called at the start of the model during the build phase. It allocates
 * memory and set the values to be used by the model. This is called now because the values
 * are needed by the Partition::BuildAgeLengthProportions() method to build all of the
 * proportions.
 */
void Partition::BuildMeanLengthData() {
  LOG_TRACE();
  auto years               = model_->years();
  unsigned time_step_count = model_->time_steps().size();
  unsigned age_spread      = 0;
  unsigned year_index      = 0;
  unsigned age_index       = 0;

  for (auto iter : partition_) {
    auto& category = *iter.second; // mean_length_by_time_step_age_
    if (category.age_length_ == nullptr)
      return;

    // Allocate memory for the mean_length_by_time_step_age if it hasn't been done previously
    if (category.mean_length_by_time_step_age_.size() == 0) {
      age_spread = category.age_spread();
      category.mean_length_by_time_step_age_.resize(years.size());

      for (unsigned i = 0; i < years.size(); ++i) {
        category.mean_length_by_time_step_age_[i].resize(time_step_count);
        for (unsigned j = 0; j < time_step_count; ++j) {
          category.mean_length_by_time_step_age_[i][j].resize(age_spread);
        }
      }
    }

    // Populate the mean length object now
    for (auto year : years) {
      year_index = year - model_->start_year();

      for (unsigned step_iter = 0; step_iter < time_step_count; ++step_iter) {
        for (unsigned age = category.min_age_; age <= category.max_age_; ++age) {
          age_index = age - category.min_age_;
          category.mean_length_by_time_step_age_[year_index][step_iter][age_index] = category.age_length_->GetMeanLength(year, step_iter, age);
        }
      }
    }

    category.UpdateMeanWeightData();
  } // for (auto iter : partition_)
}

/**
 * This method builds the age length proportions for each category to be used during
 * processes/observations that need to convert the age partition into length for use
 * in an age-based model
 */
void Partition::BuildAgeLengthProportions() {
  LOG_TRACE();
  // If we have age lengths defined, build our age-length proportions
  if (!model_->categories()->HasAgeLengths())
    return;

  unsigned year_count         = model_->years().size();
  unsigned time_step_count    = model_->time_steps().size();
  unsigned length_bin_count   = model_->length_bins().size();

  unsigned year           = 0;
  Double mu               = 0.0;
  Double cv               = 0.0;
  unsigned age            = 0;
  Double sigma            = 0.0;
  Double cv_temp          = 0.0;
  Double Lvar             = 0.0;
  Double z                = 0.0;
  Double tt               = 0.0;
  Double norm             = 0.0;
  Double ttt              = 0.0;
  Double tmp              = 0.0;
  Double sum              = 0.0;

  vector<Double> cum(length_bin_count, 0.0);
  auto model_length_bins = model_->length_bins();
  vector<double> length_bins(model_length_bins.size(), 0.0);

  LOG_MEDIUM() << "Calculating age-length proportions: year_count " << year_count;

  LOG_FINEST() << "years: " << year_count << "; time_steps: " << time_step_count << "; length_bins: " << length_bin_count;
  unsigned matrix_length_bin_count = model_->length_plus() ? length_bin_count : length_bin_count - 1;
  LOG_FINEST() << "matrix_length_bin_count: " << matrix_length_bin_count;

  for (auto iter : partition_) {
    LOG_FINEST() << "Working on " << iter.first;
    if (iter.second->age_length_->distribution() == Distribution::kLogNormal) {
      for (unsigned i = 0; i < model_length_bins.size(); ++i) {
        if (model_length_bins[i] < 0.0001)
          length_bins[i] = log(0.0001);
        else
          length_bins[i] = log(model_length_bins[i]);
      }
    } else {
      for (unsigned i = 0; i < model_length_bins.size(); ++i)
        length_bins[i] = model_length_bins[i];
    }

    auto age_length_proportion = new utilities::Vector4();
    age_length_proportion->resize(year_count);
    for (unsigned year = 0; year < year_count; ++year) {
      (*age_length_proportion)[year].resize(time_step_count);
      for (unsigned time_step = 0; time_step < time_step_count; ++time_step) {
        (*age_length_proportion)[year][time_step].resize(iter.second->age_spread());
        for (unsigned age_index = 0; age_index < iter.second->age_spread(); ++age_index) {
          (*age_length_proportion)[year][time_step][age_index].resize(matrix_length_bin_count);
        }
      }
    }

    iter.second->age_length_matrix_.resize(year_count);
    for (unsigned year = 0; year < year_count; ++year) {
      iter.second->age_length_matrix_[year].resize(time_step_count);
      for (unsigned time_step = 0; time_step < time_step_count; ++time_step) {
        iter.second->age_length_matrix_[year][time_step].resize(iter.second->age_spread());
        for (unsigned age_index = 0; age_index < iter.second->age_spread(); ++age_index) {
          iter.second->age_length_matrix_[year][time_step][age_index].resize(matrix_length_bin_count);
        }
      }
    }

    LOG_FINEST() << "Memory allocation done";

    bool casal_normal_cdf = iter.second->age_length_->casal_normal_cdf();
    for (unsigned year_iter = 0; year_iter < year_count; ++year_iter) {
      year = year_iter + model_->start_year();

      for (unsigned time_step = 0; time_step < time_step_count; ++time_step) {
        for (unsigned age_index = 0; age_index < iter.second->age_spread(); ++age_index) {

          age = age_index + iter.second->min_age_;
          mu = iter.second->mean_length_by_time_step_age_[year_iter][time_step][age_index];
          cv = iter.second->age_length_->cv(year, time_step, age);
          sigma = cv * mu;

//          LOG_FINEST() << "year: " << year << "; age: " << age << "; mu: " << mu << "; cv: " << cv << "; sigma: " << sigma;
          if (iter.second->age_length_->distribution() == Distribution::kLogNormal) {
            // Transform parameters in to log space
            cv_temp = sigma / mu;
            Lvar = log(cv_temp * cv_temp + 1.0);
            mu = log(mu) - Lvar / 2.0;
            sigma = sqrt(Lvar);
          }

          for (auto value : length_bins)
            LOG_FINEST() << "length_bin: " << value;

          LOG_FINEST() << "mu: " << mu;
          LOG_FINEST() << "sigma: " << sigma;

          sum = 0;
          vector<Double>& prop_in_length = (*age_length_proportion)[year_iter][time_step][age_index];
          for (unsigned j = 0; j < length_bin_count; ++j) {
            z = fabs((length_bins[j] - mu)) / sigma;

            // If we are using CASAL's Normal CDF function use this switch
            if (casal_normal_cdf) {
              tmp = 0.5 * pow((1 + 0.196854 * z + 0.115194 * z * z + 0.000344 * z * z * z + 0.019527 * z * z * z * z), -4);
            } else {
              tt = 1.0 / (1.0 + 0.2316419 * z);
              norm = 1.0 / sqrt(2.0 * M_PI) * exp(-0.5 * z * z);
              ttt = tt;
              tmp = 0.319381530 * ttt;
              ttt = ttt * tt;
              tmp = tmp - 0.356563782 * ttt;
              ttt = ttt * tt;
              tmp = tmp + 1.781477937 * ttt;
              ttt = ttt * tt;
              tmp = tmp - 1.821255978 * ttt;
              ttt = ttt * tt;
              tmp = tmp + 1.330274429 * ttt;
              tmp *= norm;
            }
            tmp = 1.0 - tmp;
            cum[j] = tmp;

            if (length_bins[j] < mu) {
              cum[j] = 1.0 - cum[j];
              LOG_FINEST() << "cum[" << j << "]: " << cum[j];
            }
            if (j > 0) {
              prop_in_length[j - 1] = cum[j] - cum[j - 1];
              sum += prop_in_length[j - 1];
            }
          } // for (unsigned j = 0; j < length_bin_count; ++j)

          if (model_->length_plus())
            prop_in_length[length_bin_count - 1] = 1.0 - sum - cum[0];

        } // for (unsigned age_index = 0; age_index < iter.second->age_spread(); ++age_index)
      } // for (unsigned time_step = 0; time_step < time_step_count; ++time_step)

      // If the age length object is not data, then it doesn't vary by year
      if (!(iter.second->age_length_->varies_by_years())) {
        auto& source = (*age_length_proportion)[0];

        if (year_count > 1)
          for (unsigned year_idx = 1; year_idx < year_count; ++year_idx) {
            auto& props = (*age_length_proportion)[year_idx];
            props = source;
          }

        break;
      }
    } // for (unsigned year_iter = 0; year_iter < year_count; ++year_iter)

    age_length_proportions_[iter.first] = age_length_proportion;
  }
}

/**
 * Reset the partition by setting all data values to 0.0
 */
void Partition::Reset() {
  for (auto iter = partition_.begin(); iter != partition_.end(); ++iter) {
    iter->second->data_.assign(iter->second->data_.size(), 0.0);
  }
}

/**
 *  Return a reference to one of the partition categories
 *
 *  @param category_label The name of the category
 *  @return reference to the category
 */
partition::Category& Partition::category(const string& category_label) {
  auto find_iter = partition_.find(category_label);
  if (find_iter == partition_.end())
    LOG_FATAL() << "The partition does not have a category " << category_label;

  return (*find_iter->second);
}

/**
 * Return a reference to one of the age length proportion objects
 *
 * @param category_label The name of the category
 * @return reference to the age length proportion object
 */
utilities::Vector4& Partition::age_length_proportions(const string& category_label) {
  auto find_iter = age_length_proportions_.find(category_label);
  if (find_iter == age_length_proportions_.end())
    LOG_FATAL() << "The partition does not have age length proportions for category " << category_label;

  return (*find_iter->second);
}

} /* namespace niwa */
