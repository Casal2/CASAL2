/**
 * @file MPD.h
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief This class loads, parses and builds MPDs. MPDs are the estimate values and
 * covariance matrix at the minimum. We use these when determining how good
 * our minimisation was or passing the information to a MCMC algorithm so it knows
 * how to generate new candidates.
 *
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SOURCE_MPD_MPD_H_
#define SOURCE_MPD_MPD_H_

// headers
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Utilities/Types.h"

// namespaces
namespace niwa {
class Model;

using std::shared_ptr;
using std::string;
using std::vector;

// classes
class MPD {
public:
  MPD() = delete;
  explicit MPD(shared_ptr<Model> model) : model_(model){};
  virtual ~MPD() = default;
  void ParseString(const string& mpd_block);
  bool LoadFromDiskToMemory(const string& file_name);
  void ParseFile();
  void CreateMPD(shared_ptr<Model> master_model);
  void WriteToFile(shared_ptr<Model> master_model);

  // accessors
  string value() const { return value_; }  // return our mpd

protected:
  // members
  shared_ptr<Model> model_;
  string            file_name_ = "<unknown>";
  vector<string>    file_lines_;
  string            value_;  // stored mpd as a string
};

}  // namespace niwa

#endif /* SOURCE_MPD_MPD_H_ */
