/**
 * @file MPD.h
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief This class loads an mpd file from an estimation so that we can use the information
 * for a MCMC run. The MPD contains the most recent estimate values and covariance matrix
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SOURCE_CONFIGURATIONLOADER_MPD_H_
#define SOURCE_CONFIGURATIONLOADER_MPD_H_

// headers
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Utilities/Types.h"

// namespaces
namespace niwa {
class Model;
}
namespace niwa::configuration {

using std::shared_ptr;
using std::string;
using std::vector;

// classes
class MPD {
public:
  MPD() = delete;
  explicit MPD(shared_ptr<Model> model) : model_(model){};
  virtual ~MPD() = default;
  bool LoadFromDiskToMemory(const string& file_name);
  void ParseFile();

protected:
  // members
  shared_ptr<Model> model_;
  string            file_name_ = "<unknown>";
  vector<string>    file_lines_;
};

}  // namespace niwa::configuration

#endif /* SOURCE_CONFIGURATIONLOADER_MPD_H_ */
