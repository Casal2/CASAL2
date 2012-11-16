/**
 * @file Loader.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for loading the configuration files into
 * memory and then passes each @config block off to the appropriate objects
 * after calling the factory.
 *
 * This class really creates a bunch of Base::Object objects and then assigns
 * parameters to them for verification and validation.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LOADER_H_
#define LOADER_H_

// Headers
#include <vector>
#include <string>

#include "BaseClasses/Object.h"
#include "ParameterList/Table.h"

// Namespaces
using std::vector;
using std::string;
using isam::base::Object;
using isam::base::ObjectPtr;
using isam::parameter::TablePtr;

namespace isam {
namespace configuration {

/**
 * Class Definition
 */
class Loader {
public:
  // Methods
  Loader() = default;
  virtual                     ~Loader() = default;
  void                        LoadConfigFile();
  bool                        CreateNewObject();

  // Accessors
  string                      block_type() { return block_type_; }
  void                        set_block_type(string new_value) { block_type_ = new_value; }
  string                      block_label() { return block_label_; }
  void                        set_block_label(string new_value) { block_label_ = new_value; }
  string                      object_type() { return object_type_; }
  void                        set_object_type(string new_value) { object_type_ = new_value; }
  bool                        needs_new_object() { return needs_new_object_; }
  void                        set_needs_new_object(bool new_value) { needs_new_object_ = new_value; }
  ObjectPtr                   current_object() { return current_object_; }
  bool                        loading_table() { return loading_table_; }
  void                        set_loading_table(bool new_value) { loading_table_ = new_value; }
  TablePtr                    current_table() { return current_table_; }
  void                        set_current_table(TablePtr new_value) { current_table_ = new_value; }

private:
  // Members
  string                      block_type_       = "";
  string                      block_label_      = "";
  string                      object_type_      = "";
  bool                        needs_new_object_ = true;
  ObjectPtr                   current_object_;
  TablePtr                    current_table_;
  bool                        loading_table_    = false;
};

} /* namespace configuration */
} /* namespace isam */
#endif /* LOADER_H_ */
