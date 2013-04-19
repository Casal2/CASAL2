/**
 * @file Table.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents a table block loaded from the configuration file. It's
 * used for processing data and ensuring consistency of the table data when loaded
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TABLE_H_
#define TABLE_H_

// Headers
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

// Namespaces
namespace isam {
namespace parameterlist {

using std::string;
using std::vector;

/**
 * Class definition
 */
class Table {
public:
  explicit Table(const string &label);
  virtual                     ~Table() = default;
  void                        AddColumns(const vector<string> &columns);
  void                        AddRow(vector<string> &row);
  bool                        HasColumns() { return columns_.size() != 0; }
  unsigned                    ColumnCount() { return columns_.size(); }

private:
  string                      label_;
  vector<string>              columns_;
  vector<vector<string> >     data_;
};

/**
 * Typedefs
 */
typedef boost::shared_ptr<isam::parameterlist::Table> TablePtr;

} /* namespace parameterlist */
} /* namespace isam */
#endif /* TABLE_H_ */
