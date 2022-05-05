/*
 * DefaultReports.cpp
 *
 *  Created on: 15/08/2021
 *      Author: Admin
 */

#include "DefaultReports.h"

namespace niwa {
namespace reports {

/**
 *
 */
DefaultReports::DefaultReports() {
  parameters_.Bind<bool>(PARAM_CATCHABILITIES, &report_catchabilities_, "Report catchabilities", "", false);
  parameters_.Bind<bool>(PARAM_DERIVED_QUANTITIES, &report_derived_quantities_, "Report derived quantities", "", false);
  parameters_.Bind<bool>(PARAM_OBSERVATIONS, &report_observations_, "Report observations", "", false);
  parameters_.Bind<bool>(PARAM_PROCESSES, &report_processes_, "Report processes", "", false);
  parameters_.Bind<bool>(PARAM_PROJECTS, &report_projects_, "Report projects", "", false);
  parameters_.Bind<bool>(PARAM_SELECTIVITIES, &report_selectivities_, "Report selectivities", "", false);
  parameters_.Bind<bool>(PARAM_PARAMETER_TRANSFORMATIONS, &addressable_transformations_, "Report parameters transformations", "", false);
  parameters_.Bind<bool>(PARAM_TIME_VARYING, &time_varying_, "Report Time-Varying objects", "", false);

  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling | RunMode::kMCMC);
  model_state_ = (State::Type)(State::kIterationComplete);
}

void DefaultReports::DoBuild(shared_ptr<Model> model) {
  LOG_TRACE();

  if (model->is_primary_thread_model()) {
    if (report_catchabilities_) {
      if(model->global_configuration().print_tabular()) {
        string report_label = "__catchabilities__";
        LOG_INFO() << "Creating default report for tabular catchability " << report_label;
        reports::Catchability* report = new reports::Catchability();
        report->set_is_default(true);
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_CATCHABILITY, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      } else {
        catchabilities::Manager& CatchabilityManager = *model->managers()->catchability();
        auto catchability = CatchabilityManager.objects();
        for (auto object : catchability) {
          string label        = object->label();
          string report_label = "__" + label + "__";
          LOG_INFO() << "Creating default report for catchability " << label;
          reports::Catchability* report = new reports::Catchability();
          report->set_is_default(true);
          report->set_block_type(PARAM_REPORT);
          report->set_defined_file_name(__FILE__);
          report->set_defined_line_number(__LINE__);
          report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
          report->parameters().Add(PARAM_TYPE, PARAM_CATCHABILITY, __FILE__, __LINE__);
          report->parameters().Add(PARAM_CATCHABILITY, label, __FILE__, __LINE__);
          model->managers()->report()->AddInternalObject(report);
        }
      }
    }
    if (time_varying_) {
      timevarying::Manager& TimeVaryingManager = *model->managers()->time_varying();
      auto time_vary = TimeVaryingManager.objects();
      for (auto object : time_vary) {
        string label        = object->label();
        string report_label = "__" + label + "__";
        LOG_INFO() << "Creating default report for time-varying " << label;
        reports::TimeVarying* report = new reports::TimeVarying();
        report->set_is_default(true);
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_TIME_VARYING, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TIME_VARYING, label, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      }
    }
    if (report_derived_quantities_) {
      derivedquantities::Manager& DerivedQuantityManager = *model->managers()->derived_quantity();
      auto derivedquantity = DerivedQuantityManager.objects();
      for (auto object : derivedquantity) {
        string label        = object->label();
        string report_label = "__" + label + "__";
        LOG_INFO() << "Creating default report for derived quantity " << label;
        reports::DerivedQuantity* report = new reports::DerivedQuantity();
        report->set_is_default(true);
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_DERIVED_QUANTITY, __FILE__, __LINE__);
        report->parameters().Add(PARAM_DERIVED_QUANTITY, label, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      }
    }
    if (addressable_transformations_) {
      addressabletransformations::Manager& TransformationManager = *model->managers()->addressable_transformation();
      auto transformation = TransformationManager.objects();
      for (auto object : transformation) {
        string label        = object->label();
        string report_label = "__" + label + "__";
        LOG_INFO() << "Creating default report for addressable transformations " << label;
        reports::AddressableTransformation* report = new reports::AddressableTransformation();
        report->set_is_default(true);
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_PARAMETER_TRANSFORMATIONS, __FILE__, __LINE__);
        report->parameters().Add(PARAM_PARAMETER_TRANSFORMATION, label, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      }
    }
    if (report_observations_) {
      observations::Manager& ObservationManager = *model->managers()->observation();

      auto observation = ObservationManager.objects();
      for (auto object : observation) {
        string label        = object->label();
        string report_label = "__" + label + "__";
        LOG_INFO() << "Creating default report for observation " << label;
        reports::Observation* report = new reports::Observation();
        report->set_is_default(true);
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_OBSERVATION, __FILE__, __LINE__);
        report->parameters().Add(PARAM_OBSERVATION, label, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      }
    }
    if (report_processes_) {
      processes::Manager& ProcessManager = *model->managers()->process();

      auto process = ProcessManager.objects();
      for (auto object : process) {
        string label        = object->label();
        string report_label = "__" + label + "__";
        LOG_INFO() << "Creating default report for process " << label;
        reports::Process* report = new reports::Process();
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_PROCESS, __FILE__, __LINE__);
        report->parameters().Add(PARAM_PROCESS, label, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      }
    }
    if (report_projects_) {
      projects::Manager& ProjectManager = *model->managers()->project();

      auto project = ProjectManager.objects();
      for (auto object : project) {
        string label        = object->label();
        string report_label = "__" + label + "__";
        LOG_INFO() << "Creating default report for project " << label;
        reports::Project* report = new reports::Project();
        report->set_is_default(true);
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_PROJECT, __FILE__, __LINE__);
        report->parameters().Add(PARAM_PROJECT, label, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      }
    }
    if (report_selectivities_) {
      if(model->global_configuration().print_tabular()) {
        reports::Selectivity* report = new reports::Selectivity();
        string report_label = "__selectivities__";
        LOG_INFO() << "Creating default report for selectivity " << report_label;

        report->set_is_default(true);
        report->set_block_type(PARAM_REPORT);
        report->set_defined_file_name(__FILE__);
        report->set_defined_line_number(__LINE__);
        report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
        report->parameters().Add(PARAM_TYPE, PARAM_SELECTIVITY, __FILE__, __LINE__);
        model->managers()->report()->AddInternalObject(report);
      } else {
        selectivities::Manager& SelectivityManager = *model->managers()->selectivity();

        auto selectivity = SelectivityManager.objects();
        for (auto object : selectivity) {
          string label        = object->label();
          string report_label = "__" + label + "__";

          if(!object->IsSelectivityLengthBased()) {
            LOG_INFO() << "Creating default report for selectivity " << label;
            reports::Selectivity* report = new reports::Selectivity();
            report->set_is_default(true);
            report->set_block_type(PARAM_REPORT);
            report->set_defined_file_name(__FILE__);
            report->set_defined_line_number(__LINE__);
            report->parameters().Add(PARAM_LABEL, report_label, __FILE__, __LINE__);
            report->parameters().Add(PARAM_TYPE, PARAM_SELECTIVITY, __FILE__, __LINE__);
            report->parameters().Add(PARAM_SELECTIVITY, label, __FILE__, __LINE__);
            model->managers()->report()->AddInternalObject(report);
          } else {
            LOG_INFO() << "skipping the default report for " << label << " this is a length based selectivity. Create a seperate report for this";
          }
        }
      }
    }
  }
}

} /* namespace reports */
} /* namespace niwa */
