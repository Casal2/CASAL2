#' generate.starting.pars Generates a parameter file that is formatted for -i input into Casal2.
#'
#' This function reads a Casal2 estimation configuration file and returns a par file. Where each parameter is drawn from the prior defined in an @estimate block.
#'
#' @author Craig Marsh
#' @param estimation_csl2_file the name of the configuration file containing the @estimate blocks.
#' @param path Optionally, the path to the file
#' @param n_sim the number of random samples you want.
#' @param par_file_name = the filename of the file created 
#' @param all_uniform = logical if TRUE draw from a uniform between bounds regardless of prior distribution
#' @param quiet Optional, supresses printing statements
#' @param fileEncoding Optional, allows the R-library to read in files that have been encoded in alternative UTF formats, see the manual for the error message that would indicate when to use this switch.

#' @return a file named 'parms.out' in the path directory
#' @export
#'

generate.starting.pars <- function(path = "", estimation_csl2_file = "Estimation.csl2", n_sim = 10, par_file_name = "starting_pars.out", all_uniform = FALSE, fileEncoding = "", quiet = T) {
  estimate_config = extract.csl2.file(file = estimation_csl2_file, path = path, fileEncoding = fileEncoding, quiet = quiet);
  ## pull out the estimates only, they are the only components interested in at the moment
  estimate_ndx = grepl(pattern = "estimate\\[", x = names(estimate_config))
  estimate_block_labels = names(estimate_config)[estimate_ndx]
  ## allowed types, currently I have only coded to generate candidates from these distributions
  allowed_types = c("uniform", "uniform_log", "lognormal", "normal", "normal_by_stdev", "normal_log");

  ## pull out parameter labels, bounds, parameters and distributions and store, will be the header
  n_estimate_blocks = sum(estimate_ndx)
  param_labels = NULL;
  param_values = NULL;
  conditional_parameters = list();
  value_labels = NULL;
  for (i in 1:n_estimate_blocks) {
    this_estimate = get(estimate_block_labels[i], estimate_config)
    ## pull out label, might need it later for reporting errors
    start_nd = as.numeric(regexpr(pattern = "\\[", text = estimate_block_labels[i]))
    end_nd = as.numeric(regexpr(pattern = "\\]", text = estimate_block_labels[i]))
    Label = substr(estimate_block_labels[i], start_nd + 1, end_nd - 1)
    ## Check to see if we have code to simulate this type of prior
    if (!this_estimate$type %in% allowed_types) {
      stop(paste0("Sorry, we haven't coded a random number generator for the distribution of type ", this_estimate$type, " we currently only have the following types ", paste(allowed_types, collapse = ", ")))
    }
    
    ## check if this parameter needs to be split out into multiple parameters
    start_nd = as.numeric(regexpr(pattern = "\\{", text = this_estimate$parameter$value))
    end_nd = as.numeric(regexpr(pattern = "\\}", text = this_estimate$parameter$value))
    colon_nd = as.numeric(regexpr(pattern = "\\:", text = this_estimate$parameter$value))
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
        stop(paste0("For the @estimate block ", Label, " you specified the parameter ", this_estimate$parameter$value, " which in this function is intepretted as a single parameter
        however you have specified lower bounds with ", n_lower_bounds, " lower bounds. For this function to work, you must specify the index of this parameter. For example 
        this would not work, parameter process[Recruitment].ycs_values, what this function requires of you is to write the year specific values i.e. parameter process[Recruitment].ycs_values{1974:2014}.
        The same goes with vector parameters, you need to specify process[Recruitment].proportions[1:2]"))
      }
      value_labels = c(value_labels, Label)
      param_labels = c(param_labels, this_estimate$parameter$value)
      ## Now simulate data.
      values = NULL;
      if (this_estimate$type$value == "uniform" || all_uniform) {
        values = runif(n = n_sim, min = as.numeric(this_estimate$lower_bound$value), max = as.numeric(this_estimate$upper_bound$value))
      } else if (this_estimate$type$value == "uniform_log") {
        values = exp(runif(n = n_sim, min = log(as.numeric(this_estimate$lower_bound$value)), max = log(as.numeric(this_estimate$upper_bound$value))))
      } else if (this_estimate$type$value == "normal") {
        std_dev = as.numeric(this_estimate$mu$value) * as.numeric(this_estimate$cv$value)
        values = rnorm(n = n_sim, mean = as.numeric(this_estimate$mu$value), sd = std_dev)
      } else if (this_estimate$type == "lognormal") {
        values = Rlnorm(n = n_sim, mu = as.numeric(this_estimate$mu$value), cv = as.numeric(this_estimate$cv$value))
      } else if (this_estimate$type == "normal_by_stdev") {
        values = rnorm(n = n_sim, mean = as.numeric(this_estimate$mu$value), sd = as.numeric(this_estimate$sigma$value))
      } else if (this_estimate$type == "normal_log") {
        values = exp(rnorm(n = n_sim, mean = as.numeric(this_estimate$mu$value), sd = as.numeric(this_estimate$sigma$value)))
      }
      ## set to bounds if generated past them
      values[values < as.numeric(this_estimate$lower_bound$value)] = as.numeric(this_estimate$lower_bound$value)
      values[values > as.numeric(this_estimate$upper_bound$value)] = as.numeric(this_estimate$upper_bound$value)
      param_values = cbind(param_values, values)
    } else {
      ## need to expand out the parameter labels.
      colon_ndx = regexpr(pattern = "\\:", text = this_estimate$parameter$value)
      left_value = as.numeric(substr(this_estimate$parameter$value, start_nd + 1, stop = colon_ndx - 1))
      right_value = as.numeric(substr(this_estimate$parameter$value, colon_ndx + 1, end_nd - 1))
      n_params = length(left_value:right_value)
      param_lab = paste0(substr(this_estimate$parameter$value, 0, start_nd - 1), "{", left_value:right_value, "}")
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
          values = runif(n = n_sim, min = as.numeric(this_estimate$lower_bound$value[param]), max = as.numeric(this_estimate$upper_bound$value[param]))
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)
        }
      } else if (this_estimate$type == "uniform_log") {
        for (param in 1:n_params) {
          values = exp(runif(n = n_sim, min = log(as.numeric(this_estimate$lower_bound$value[param])), max = log(as.numeric(this_estimate$upper_bound$value[param]))))
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
          values = rnorm(n = n_sim, mean = as.numeric(this_estimate$mu$value[param]), sd = std_dev[param])
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
          values = Rlnorm(n = n_sim, mu = as.numeric(this_estimate$mu$value[param]), cv = as.numeric(this_estimate$cv$value[param]))
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
          values = rnorm(n = n_sim, mean = as.numeric(this_estimate$mu$value[param]), sd = as.numeric(this_estimate$sigma$value[param]))
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
          values = exp(rnorm(n = n_sim, mean = as.numeric(this_estimate$mu$value), sd = as.numeric(this_estimate$sigma$value)))
          ## check within bounds
          values[values < as.numeric(this_estimate$lower_bound$value[param])] = as.numeric(this_estimate$lower_bound$value[param])
          values[values > as.numeric(this_estimate$upper_bound$value[param])] = as.numeric(this_estimate$upper_bound$value[param])
          param_values = cbind(param_values, values)
        }
      }
      value_labels = c(value_labels, rep(Label, n_params))
    }
  }
  colnames(param_values) = value_labels;
  ## write the file.
  filename = make.filename(path = path, file = par_file_name)
  cat(param_labels, file = filename, sep = " ", fill = F, labels = NULL, append = F)
  cat("\n", file = filename, sep = " ", fill = F, labels = NULL, append = T)
  for (i in 1:n_sim) {
    cat(param_values[i,], file = filename, sep = " ", fill = F, labels = NULL, append = T)
    cat("\n", file = filename, sep = " ", fill = F, labels = NULL, append = T)
  }
}
