#' Utility function
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
  common = c("label","parameter", "type", "table", "end_table","categories","selectivities", "years")
  common_types = c(rep("single_value",3),"table_label", "end_table",rep("vector",3))

  ## Processes
  pro = c("processes","m","time_step_ratio","from", "to","rates","catches","proportions","ycs_values","standardise_ycs_years","penalty","b0","r0","R0","age","ssb_offset", "steepness","ssb","b0_intialisation_phase","prior_standardised_ycs")
  pro_type = c(rep("vector",10),rep("single_value",10))
  
  ## Growth_model
  grow = c("a", "b", "units", "cv_first", "cv_last","distribution","casal_switch", "linf","k","t0","length_weight","by_length","y1","y2","tau1","tau2","time_step_proportions")
  grow_type = c(rep("single_value",16),rep("vector",1))

  ## Observations
  obs = c("detection","dispersion","plus_group","likelihood","catchability","delta","process_error","tolerance","mortality_instantaneous_process","method_of_removal","ageing_error", "time_step","obs","error_values","process_errors","selectivities2")
  obs_type = c(rep("single_value",11),rep("vector",5))

  ## priors 
  pri = c("mu","sigma","lower_bound","upper_bound","multiplier")
  pri_type = c(rep("vector",4),"single_value")
  
  ## selectivities
  sel = c("length_based","sigma_l","sigma_r", "a50", "ato95", "alpha","x0","x1","x2","y0","l","h","c","v")
  sel_type = c(rep("single_value",13),rep("vector",1))

  ## Time varying 
  tim = c("exogeneous_variable","value","values","scaling_years", "slope", "intercept")
  tim_type = c(rep("vector",4),rep("single_value",2))

  ## Ageing error
  age = c("p1","p2")
  age_type = rep("single_value",2)
  
  ## reporting
  report = c("file_name","write_mode","observation","process","selectivity")
  report_type = rep("single_value",5)
  ## Combine them all.
  casal2_list = list( command = c(model, categories, common, init, pro, obs, sel, grow, pri, tim, age, report), type = c(model_type, categories_type, common_types, init_types, pro_type, obs_type, sel_type, grow_type, pri_type, tim_type, age_type, report_type))
  if (TRUE %in% duplicated(casal2_list$command)) {
    stop("can not have duplicated subcommand in this object")
  }
return(casal2_list)
}

