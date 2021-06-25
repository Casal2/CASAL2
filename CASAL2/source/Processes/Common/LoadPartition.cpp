/**
 * @file LoadPartition.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-06-26
 *
 * @copyright Copyright (c) 2021
 *
 */

// headers
#include "LoadPartition.h"

#include "../../Partition/Partition.h"

// namespaces
namespace niwa::processes {

/**
 * Empty constructor
 */
LoadPartition::LoadPartition(shared_ptr<Model> model) : Process(model) {
  process_type_        = ProcessType::kNullProcess;
  partition_structure_ = PartitionType::kAge;
  data_table_          = new parameters::Table(PARAM_DATA);

  parameters_.BindTable(PARAM_DATA, data_table_, "Partition information to load", "", true, false);
}

/**
 * @brief Destructor
 *
 */
LoadPartition::~LoadPartition() {
  if (data_table_)
    delete data_table_;
}

/**
 * @brief
 *
 */
void LoadPartition::DoValidate() {
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();

  auto columns = data_table_->columns();
  if (columns.size() != model_->age_spread() + 1)  // category min_age min_age+1 min_age+n
    LOG_FATAL() << "data table must have columns: category < model ages >";

  unsigned age = 0.0;
  for (const auto& column : columns) {
    if (column == PARAM_CATEGORY)
      continue;

    if (!utilities::To<string, unsigned>(column, age))
      LOG_FATAL() << "Could not convert value " << column << " to an age in the data table column header";
    if (age < min_age || age > max_age)
      LOG_FATAL() << "Age entered into data table column header " << age << " is not between the model min and max ages";
  }
}

/**
 * @brief
 *
 */
void LoadPartition::DoBuild() {
  unsigned min_age = model_->min_age();

  auto columns = data_table_->columns();
  auto rows    = data_table_->data();
  for (auto row : rows) {
    string category_label = row[0];
    auto&  category       = model_->partition().category(category_label);

    for (unsigned i = 1; i < row.size(); ++i) {
      unsigned age   = utilities::ToInline<string, unsigned>(columns[i]);
      Double   value = utilities::ToInline<string, Double>(row[i]);

      category.data_[age - min_age] = value;
    }
  }
}

}  // namespace niwa::processes
