#' generate.starting.pars Generates a parameter file that is formatted for -i input into Casal2.
#'
#' This function reads a Casal2 estimation configuration file and returns a par file. Where each parameter is drawn from the prior defined in an @estimate block.
#'
#' @author Craig Marsh
#' @param Estimation_csl2_file the name of the configuration file containing the @estimate blocks.
#' @param path Optionally, the path to the file
#' @param N the number of random samples you want.
#' @param par_file_name = the filename of the file created 
#' @param all_uniform = logical if TRUE draw from a uniform between bounds regardless of prior distribution
#' @return a file named 'parms.out' in the path directory
#' @export
#'

'generate.starting.pars' = function(path = "", Estimation_csl2_file = "Estimation.csl2", N = 10, par_file_name = "starting_pars.out", all_uniform = FALSE) {
  estimate_config = extract.csl2.file(file = Estimation_csl2_file, path = path);
  ## pull out the estimates only, they are the only components interested in at the moment
  estimate_ndx = grepl(pattern = "estimate\\[", x = names(estimate_config))  
  estimate_block_labels = names(estimate_config)[estimate_ndx]
  ## allowed types, currently I have only coded to generate candidates from these distributions
  allowed_types = c("uniform", "uniform_log", "lognormal","normal","normal_by_stdev", "normal_log");

  ## pull out parameter labels, bounds, parameters and distributions and store, will be the header
  n_estimate_blocks = sum(estimate_ndx)
  param_labels = NULL;
  param_values = NULL;
  conditional_parameters = list();
  value_labels = NULL;
  for (i in 1:n_estimate_blocks) {
    this_estimate = get(estimate_block_labels[i], estimate_config)
    ## pull out label, might need it later for reporting errors
    start_nd = as.numeric(regexpr(pattern = "\\[",text = estimate_block_labels[i]))
    end_nd = as.numeric(regexpr(pattern = "\\]",text = estimate_block_labels[i]))
    Label = substr(estimate_block_labels[i], start_nd + 1, end_nd - 1)
    ## Check to see if we have code to simulate this type of prior
    if (!this_estimate$type %in% allowed_types) {       
      stop(Paste("Sorry, we haven't coded a random number generator for the distribution of type " ,this_estimate$type , " we currently only have the following types ", paste(allowed_types, collapse = ", ")))
    }
    #########################################
    ## Check priors don't related to EstimateTransformations?
    ## these will need to be taken care of
    deal_with_transformations = FALSE;
    skip_parameter = FALSE
    ## this gets tricky we need to iterate over the estimate_transfomration blocks so we can see if there is another parameter involved?  
    if(!is.null(this_estimate$prior_applies_to_transform) && (this_estimate$prior_applies_to_transform$value == "TRUE" || this_estimate$prior_applies_to_transform$value == "t" || this_estimate$prior_applies_to_transform$value == "true")) {
      ## Check to see if we haven't already found this parameter in a previous transformation
      if (length(names(conditional_parameters)) > 0) {
        for (previous_trans in names(conditional_parameters)) {
          previous_transformation = get(previous_trans, conditional_parameters)
            if (previous_transformation$other_label == Label) {
              ## this parameter has been associated with a previous transformatin block.
              skip_parameter = TRUE;
            }
        }
      }
      if (!skip_parameter) {   
        estimate_transformation_ndx = grepl(pattern = "estimate_transformation", x = names(estimate_config)) 
        if (any(estimate_transformation_ndx)) {
          for (trans in 1:sum(estimate_transformation_ndx)) {
            this_trans = get(names(estimate_config)[estimate_transformation_ndx][trans],estimate_config)
            if (this_trans$type$value == "average_difference") {
              if (this_trans$estimate_label$value == Label) {
                conditional_parameters[[Label]]$other_label = this_trans$difference_estimate$value
                conditional_parameters[[Label]]$type = "average_difference"
                conditional_parameters[[Label]]$first_estimate = "true"              
              } else if (this_trans$difference_estimate$value == Label) {
                conditional_parameters[[Label]]$other_label = this_trans$estimate_label$value
                conditional_parameters[[Label]]$type = "average_difference"
                conditional_parameters[[Label]]$first_estimate = "false"                            
              }
            } else if (this_trans$type$value == "log_sum") {
              if (this_trans$estimate_label$value == Label) {
                conditional_parameters[[Label]]$other_label = this_trans$second_estimate$value
                conditional_parameters[[Label]]$type = "log_sum"
                conditional_parameters[[Label]]$first_estimate = "true"                            
              } else if (this_trans$second_estimate$value == Label) {
                conditional_parameters[[Label]]$other_label = this_trans$estimate_label$value
                conditional_parameters[[Label]]$type = "log_sum"
                conditional_parameters[[Label]]$first_estimate = "false"                            
              }            
            } else if (this_trans$type$value == "orthogonal") {
              if (this_trans$estimate_label$value == Label) {
                conditional_parameters[[Label]]$other_label = this_trans$second_estimate$value
                conditional_parameters[[Label]]$first_estimate = "true"                            
                conditional_parameters[[Label]]$type = "orthogonal"
              } else if (this_trans$second_estimate$value == Label) {
                conditional_parameters[[Label]]$other_label = this_trans$estimate_label$value
                conditional_parameters[[Label]]$type = "orthogonal"
                conditional_parameters[[Label]]$first_estimate = "false"                            
              }            
            } else if (this_trans$type$value == "simplex") {
              if (this_trans$estimate_label$value == Label) {
                stop("This code cannot deal with the simplex transformation?")
              }
            }
          }
        }
      }
    }  
    ## check if this parameter needs to be split out into multiple parameters
    start_nd = as.numeric(regexpr(pattern = "\\{",text = this_estimate$parameter$value))
    end_nd = as.numeric(regexpr(pattern = "\\}",text = this_estimate$parameter$value))
    colon_nd = as.numeric(regexpr(pattern = "\\:",text = this_estimate$parameter$value))    
    if ((start_nd == -1 || end_nd == -1) || colon_nd == -1) {
      ## single parameter found.
      ## check that the user hasn't specified many lower_bounds or upper_bounds.
      ## and check short hand syntax, which is checking for the same as above.
      short_hand_lower = which(this_estimate$lower_bound$value == "*")       
      if (length(short_hand_lower) > 0 || length(this_estimate$lower_bound$value) > 1) {
        ## split out sntax.
        n_lower_bounds = length(this_estimate$lower_bound$value)
        if (length(short_hand_lower) > 0) {
          left_value = as.numeric(this_estimate$lower_bound$value[short_hand_lower - 1])
          n_lower_bounds = as.numeric(this_estimate$lower_bound$value[short_hand_lower + 1])
        } 
        stop(Paste("For the @estimate block " , Label , " you specified the parameter " , this_estimate$parameter$value, " which in this function is intepretted as a single parameter
        however you have specified lower bounds with " , n_lower_bounds , " lower bounds. For this function to work, you must specify the index of this parameter. For example 
        this would not work, parameter process[Recruitment].ycs_values, what this function requires of you is to write the year specific values i.e. parameter process[Recruitment].ycs_values{1974:2014}.
        The same goes with vector parameters, you need to specify process[Recruitment].proportions[1:2]"))        
      }     
      value_labels = c(value_labels, Label)
      param_labels = c(param_labels, this_estimate$parameter$value)      
      ## Now simulate data.
      values = NULL;
      if (this_estimate$type == "uniform" || all_uniform) {
        values = runif(n = N, min = as.numeric(this_estimate$lower_bound$value), max = as.numeric(this_estimate$upper_bound$value))
      } else if (this_estimate$type == "uniform_log") {
        values = exp(runif(n = N, min = log(as.numeric(this_estimate$lower_bound$value)), max = log(as.numeric(this_estimate$upper_bound$value))))
      } else if (this_estimate$type == "normal") {
        std_dev = as.numeric(this_estimate$mu$value) * as.numeric(this_estimate$cv$value)
        values = rnorm(n = N, mean = as.numeric(this_estimate$mu$value), sd = std_dev)        
      } else if (this_estimate$type == "lognormal") {
        values = Rlnorm(n = N, mu = as.numeric(this_estimate$mu$value), cv = as.numeric(this_estimate$cv$value))
      } else if (this_estimate$type == "normal_by_stdev") {
        values = rnorm(n = N, mean = as.numeric(this_estimate$mu$value), sd = as.numeric(this_estimate$sigma$value))                
      } else if (this_estimate$type == "normal_log") {
        values = exp(rnorm(n = N, mean = as.numeric(this_estimate$mu$value), sd = as.numeric(this_estimate$sigma$value)))                           
      }
      ## set to bounds if generated past them
      values[values < as.numeric(this_estimate$lower_bound$value)] = as.numeric(this_estimate$lower_bound$value)
      values[values > as.numeric(this_estimate$upper_bound$value)] = as.numeric(this_estimate$upper_bound$value)
      param_values = cbind(param_values, values)         
    } else {
      ## need to expand out the parameter labels.
      colon_ndx = regexpr(pattern = "\\:",text = this_estimate$parameter$value)
      left_value = as.numeric(substr(this_estimate$parameter$value, start_nd + 1, stop = colon_ndx - 1))
      right_value = as.numeric(substr(this_estimate$parameter$value, colon_ndx + 1, end_nd - 1))
      n_params = length(left_value:right_value)
      param_lab = Paste(substr(this_estimate$parameter$value, 0,start_nd-1), "{",left_value:right_value, "}")
      param_labels = c(param_labels, param_lab)
      ## expand if any short hand syntax has been used
      if (length(this_estimate$lower_bound$value) != n_params) {
        short_hand_lower = which(this_estimate$lower_bound$value == "*")       
        left_value = as.numeric(this_estimate$lower_bound$value[short_hand_lower - 1])
        this_estimate$lower_bound$value = rep(left_value, n_params)      
      }
      if (length(this_estimate$upper_bound$value) != n_params) {
        short_hand_upper = which(this_estimate$upper_bound$value == "*")       
        left_value = as.numeric(this_estimate$upper_bound$value[short_hand_lower - 1])
        this_estimate$upper_bound$value = rep(left_value, n_params)      
      }
        
      ## Now simulate data for each param      
      if (this_estimate$type == "uniform" || all_uniform) {
        for (param in 1:n_params) {
          values = runif(n = N, min = as.numeric(this_estimate$lower_bound$value[param]), max = as.numeric(this_estimate$upper_bound$value[param]))
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)
        }
      } else if (this_estimate$type == "uniform_log") {
        for (param in 1:n_params) {
          values = exp(runif(n = N, min = log(as.numeric(this_estimate$lower_bound$value[param])), max = log(as.numeric(this_estimate$upper_bound$value[param]))))
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)
        }
      } else if (this_estimate$type == "normal") {        
        if (length(this_estimate$mu$value) != n_params) {
          short_hand_mu = which(this_estimate$mu$value == "*")        
          left_value = as.numeric(this_estimate$mu$value[short_hand_mu - 1])
          this_estimate$mu$value = rep(left_value, n_params)
        }     
        if (length(this_estimate$cv$value) != n_params) {
          short_hand_cv = which(this_estimate$cv$value == "*")        
          left_value = as.numeric(this_estimate$cv$value[short_hand_cv - 1])
          this_estimate$cv$value = rep(left_value, n_params)
        }   
        std_dev = as.numeric(this_estimate$mu$value) * as.numeric(this_estimate$cv$value)

        for (param in 1:n_params) {        
          values = rnorm(n = N, mean = as.numeric(this_estimate$mu$value[param]), sd = std_dev[param])
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)          
        }  
      } else if (this_estimate$type == "lognormal") {
        if (length(this_estimate$mu$value) != n_params) {
          short_hand_mu = which(this_estimate$mu$value == "*")        
          left_value = as.numeric(this_estimate$mu$value[short_hand_mu - 1])
          this_estimate$mu$value = rep(left_value, n_params)
        }     
        if (length(this_estimate$cv$value) != n_params) {
          short_hand_cv = which(this_estimate$cv$value == "*")        
          left_value = as.numeric(this_estimate$cv$value[short_hand_cv - 1])
          this_estimate$cv$value = rep(left_value, n_params)
        } 
        for (param in 1:n_params) {             
          values = Rlnorm(n = N, mu = as.numeric(this_estimate$mu$value[param]), cv = as.numeric(this_estimate$cv$value[param]))
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)                    
        }  
      } else if (this_estimate$type == "normal_by_stdev") {
        if (length(this_estimate$mu$value) != n_params) {
          short_hand_mu = which(this_estimate$mu$value == "*")        
          left_value = as.numeric(this_estimate$mu$value[short_hand_mu - 1])
          this_estimate$mu$value = rep(left_value, n_params)
        }     
        if (length(this_estimate$sigma$value) != n_params) {
          short_hand_sigma = which(this_estimate$sigma$value == "*")        
          left_value = as.numeric(this_estimate$sigma$value[short_hand_sigma - 1])
          this_estimate$sigma$value = rep(left_value, n_params)
        }  
        for (param in 1:n_params) {                     
          values = rnorm(n = N, mean = as.numeric(this_estimate$mu$value[param]), sd = as.numeric(this_estimate$sigma$value[param]))      
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)                              
        }
      } else if (this_estimate$type == "normal_log") {
        if (length(this_estimate$mu$value) != n_params) {
          short_hand_mu = which(this_estimate$mu$value == "*")        
          left_value = as.numeric(this_estimate$mu$value[short_hand_mu - 1])
          this_estimate$mu$value = rep(left_value, n_params)
        }     
        if (length(this_estimate$sigma$value) != n_params) {
          short_hand_sigma = which(this_estimate$sigma$value == "*")        
          left_value = as.numeric(this_estimate$sigma$value[short_hand_sigma - 1])
          this_estimate$sigma$value = rep(left_value, n_params)
        }      
        for (param in 1:n_params) {                     
          values = exp(rnorm(n = N, mean = as.numeric(this_estimate$mu$value), sd = as.numeric(this_estimate$sigma$value))) 
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)   
        }
      }
    value_labels = c(value_labels, rep(Label,n_params))
    }
  }
  colnames(param_values) = value_labels;
  ## Deal with transformations if any of the priors are described for the transformed space.
  if (length(names(conditional_parameters)) > 0) {
    for (previous_trans in names(conditional_parameters)) {
      previous_transformation = get(previous_trans, conditional_parameters)
      if (previous_transformation$type == "average_difference") {
        ## get the correct columns from the values matrix to transform
        X2 = param_values[ ,previous_transformation$other_label]
        X1 = param_values[ ,previous_trans]
        param1 = X1 + (X2 / 2)
        param2 = X1 - (X2 / 2)
        ## override values
        param_values[ ,previous_transformation$other_label] = param2
        param_values[ ,previous_trans] = param1
      } else if (previous_transformation$type == "log_sum") {
        ## get the correct columns from the values matrix to transform
        X2 = param_values[ ,previous_transformation$other_label]
        X1 = param_values[ ,previous_trans]
        total = exp(X1);
        param1 = total * X2
        param2 = total * (1 - X2)
        param_values[ ,previous_transformation$other_label] = param2
        param_values[ ,previous_trans] = param1        
      } else if (previous_transformation$type == "orthogonal") {
        ## get the correct columns from the values matrix to transform
        X2 = param_values[ ,previous_transformation$other_label]
        X1 = param_values[ ,previous_trans]
        param1 = sqrt(X1 * X2);
        param2 = sqrt(X1 / X2);    
        param_values[ ,previous_transformation$other_label] = param2
        param_values[ ,previous_trans] = param1                
      }
    }
  }
  ## write the file.
  filename = make.filename(path = path, file = par_file_name)
  cat(param_labels, file = filename, sep = " ", fill = F, labels = NULL, append = F)
  cat("\n", file = filename, sep = " ", fill = F, labels = NULL, append = T)
  for (i in 1:N) {
    cat(param_values[i,], file = filename, sep = " ", fill = F, labels = NULL, append = T)
    cat("\n", file = filename, sep = " ", fill = F, labels = NULL, append = T)    
  }
}