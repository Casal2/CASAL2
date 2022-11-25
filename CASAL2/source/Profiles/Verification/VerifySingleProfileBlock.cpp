/**
 * @file VerifySingleProfileBlock.cpp
 * @author C.Marsh
 * @brief Verify only a single @profile block has been supplied
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright (c) 2021 - NIWA
 *
 */

// headers
#include "../../Model/Model.h"
#include "../Manager.h"
#include "../Profile.h"

// namespaces
namespace niwa::profiles::verification {

/**
 * @brief Check to see if a profile block has been supplied ther is only one profile block has been supplied
 *
 * @param model Model pointer
 */
void VerifySingleProfileBlock(shared_ptr<Model> model) {
  if (model->run_mode() == RunMode::kProfiling) {
    LOG_FINE() << "verifying profile class";
    auto profile_blocks = model->managers()->profile()->objects();
    if (profile_blocks.size() > 0) {
      LOG_FINE() << "Found a profile block";
      vector<string> labels_;
      unsigned       profile_block_count = 0;
      for (auto* profile : profile_blocks) {
        labels_.push_back(profile->label());
        profile_block_count++;
      }
      if (profile_block_count > 1) {
        LOG_VERIFY() << "Found " << profile_block_count << " @profile blocks. Casal2 can only be run with one profile block.";
      }
    } else {
      LOG_VERIFY() << "Could not find a single @profile block. Casal2 needs a profile block to run in a profile.";
    }
  }
}

}  // namespace niwa::profiles::verification
