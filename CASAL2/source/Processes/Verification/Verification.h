// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa::processes::verification {

// Function List
void AgeingProcessExists(shared_ptr<Model> model);
void RecruitmentCategoriesVerification(shared_ptr<Model> model);
void AllCategoriesHaveAnM(shared_ptr<Model> model);

// Execute the functions
void DoVerification(shared_ptr<Model> model) {
  if(model->partition_type() == PartitionType::kAge) {
    AgeingProcessExists(model);
  } 
  RecruitmentCategoriesVerification(model);
  AllCategoriesHaveAnM(model);
}

}  // namespace niwa::processes::verification