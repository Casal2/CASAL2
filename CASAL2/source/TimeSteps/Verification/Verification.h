// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa::timesteps::verification {

// Function List
void AgeingProcessInAnnualCycle(shared_ptr<Model> model);
void GrowthProcessInAnnualCycle(shared_ptr<Model> model);
void GrowthIncrementConsistentWithGrowthProcess(shared_ptr<Model> model);


// Execute the functions
void DoVerification(shared_ptr<Model> model) {
  if(model->partition_type() == PartitionType::kAge) {
    LOG_FINE() << "verifying Age based models";
    AgeingProcessInAnnualCycle(model);
  } else if (model->partition_type() == PartitionType::kLength) {
    LOG_FINE() << "verifying length based models";
    GrowthProcessInAnnualCycle(model);
    GrowthIncrementConsistentWithGrowthProcess(model);
  }
}

}  // namespace niwa::processes::verification