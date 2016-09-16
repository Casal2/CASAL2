#' Utility extract function
#'
#' @author Craig Marsh
#'

"get.casal2_list" <-
    function() {
  ## @model
  model_type = c(rep("single_value",8),rep("vector",2))
  model = c("start_year","final_year","min_age", "max_age","age_plus","base_weight_units", "projection_final_year", "initialisation_phases", "time_steps","length_bins")

  ## @categories
  categories = c("format","names", "age_lengths")
  categories_type = c("single_value",rep("vector",2))

  ## @initialisation_phase
  init = c("insert_processes","exclude_processes","convergence_years","lambda")
  init_types = c(rep("vector",3),"single_value")

  ## Common to all blocks
  common = c("label", "type", "table", "end_table","categories","selectivities", "years")
  common_types = c(rep("single_value",2),"table_label", "end_table",rep("vector",3))

  ## Processes
  pro = c("m","time_step_ratio","from", "to","rates","catches","proportions","ycs_values","standardise_ycs_years","penalty","b0","r0","R0","age","ssb_offset", "steepness","ssb","b0_intialisation_phase","prior_standardised_ycs")
  pro_type = c(rep("vector",9),rep("single_value",10))
  
  ## Growth_model
  grow = c("a", "b", "units", "cv_first", "cv_last","distribution","casal_switch", "linf","k","t0","length_weight","by_length","y1","y2","tau1","tau2","time_step_proportions")
  grow_type = c(rep("single_value",16),rep("vector",1))

  ## Observations
  obs = c("likelihood","catchability","delta","process_error","tolerance","mortality_instantaneous_process","method_of_removal","ageing_error", "time_step","obs","error_values")
  obs_type = c(rep("single_value",8),rep("vector",3))

  ## selectivities
  sel = c("length_based","mu","sigma","sigma_l","sigma_r", "a50", "ato95", "alpha","x0","x1","x2","y0","l","h","v")
  sel_type = c(rep("single_value",14),rep("vector",1))

  ## Combine them all.
  casal2_list = list( command = c(model, categories, common, init, pro, obs, sel, grow), type = c(model_type,   categories_type,common_types,init_types,pro_type,obs_type,sel_type,grow_type))
  if (TRUE %in% duplicated(casal2_list$command)) {
    stop("can not have duplicated subcommand in this object")
  }
return(casal2_list)
}

