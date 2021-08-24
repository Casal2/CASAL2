#ifdef TESTMODE
#ifndef SOURCE_MPD_MPDMOCK_H_
#define SOURCE_MPD_MPDMOCK_H_

// headers
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <memory>
#include <string>
#include <vector>

#include "../Model/Model.h"
#include "MPD.h"

// Namespaces
namespace niwa {

const string TwoSex_MPD =
    R"(*mpd[__mpd__]
estimate_values:
catchability[CPUEq].q process[Recruitment].R0 selectivity[FishingSel].a50 selectivity[FishingSel].ato95 
1.10693e-05 2.37219e+06 10.1887 4.89097 
covariance_matrix:
2.13934e-12 -0.321068 -4.45289e-09 -9.74773e-10 
-0.321068 6.699e+10 -519.28 -143.374 
-4.45289e-09 -519.28 0.0123193 0.0100754 
-9.74773e-10 -143.374 0.0100754 0.0129463 
*end
)";

/**
 * @brief Mock MPD
 *
 */
class MockMPD : public MPD {
public:
  MockMPD() = delete;
  explicit MockMPD(shared_ptr<Model> model) : MPD(model){};
  ~MockMPD() = default;

  void add_string_to_file_lines(string s) {
    vector<string> lines;
    boost::split(lines, s, boost::is_any_of("\n"));
    for (string line : lines) file_lines_.push_back(line);
  }

  void ParseFile() { MPD::ParseFile(); }
};

}  // namespace niwa

#endif  // TESTMODE
#endif  // SOURCE_MPD_MPDMOCK_H_
