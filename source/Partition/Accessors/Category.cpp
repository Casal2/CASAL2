/**
 * @file Category.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 29/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Category.h"

#include <iostream>
#include <iomanip>

#include "Categories/Categories.h"
#include "Partition/Partition.h"
#include "Model/Model.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace partition {
namespace accessors {

using std::cout;
using std::endl;
namespace util = isam::utilities;

/**
 * Default constructor.
 *
 * Construction of this object involves building the category map
 */
Category::Category() {
  LOG_TRACE();

  ModelPtr model = Model::Instance();
  unsigned start_year = model->start_year();
  unsigned run_length = model->run_length();
  LOG_INFO("Model details: start_year: " << start_year << "; run_length: " << run_length);

  // TODO: Get category specific year information
  CategoriesPtr categories = Categories::Instance();
  vector<string> category_names = categories->category_names();

  Partition& partition = Partition::Instance();
  map<string, vector<Double> > grid = partition.grid();

  for (unsigned i = 0; i < run_length; ++i) {

    for (string category : category_names) {
      LOG_INFO("Adding category " << category << " @ year " << start_year + i);
    // if (category in start_year + i
      pair<string, vector<Double>& > category_reference = { category, grid[category] };
      category_map_[start_year + i].push_back(category_reference);
    }
  }

  // TODO: DEBUG
  cout << "** CATEGORY ACCESSOR DEBUG" << endl;
  cout << std::left << std::setw(10) << "year" << std::setw(20) << "category" << endl;
  for (auto i = category_map_.begin(); i != category_map_.end(); ++i) {

    for (auto j = i->second.begin(); j != i->second.end(); ++j) {
      cout << std::left << std::setw(10) << i->first << std::setw(20) << j->first << endl;
    }
  }
  cout << endl << endl;

}

/**
 *
 */
Category::~Category() {
}

void Category::Begin() { }
bool Category::Next() { return true; }

} /* namespace accessors */
} /* namespace partition */
} /* namespace isam */
