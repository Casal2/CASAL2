/**
 * @file MPDLoader.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 2/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for loading the MPD file produced
 * by the MPD report so we can start an estimation of MCMC with
 * an already minimised point and covariance matrix
 */
#ifndef SOURCE_CONFIGURATIONLOADER_MPDLOADER_H_
#define SOURCE_CONFIGURATIONLOADER_MPDLOADER_H_

// namespaces
namespace niwa {
class Model;

namespace configuration {

// classes
class MPDLoader {
public:
  // methods
  MPDLoader() = delete;
  explicit MPDLoader(Model* model);
  virtual                     ~MPDLoader() = default;
  bool                        LoadMPDFile();

private:
  // members
  Model*                      model_;
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* SOURCE_CONFIGURATIONLOADER_MPDLOADER_H_ */
