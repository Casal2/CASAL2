#' Generate an objective score profile plot for a particular parameter as a consequence from a casal2 -p run 
#'
#' This function reads an extracted list from Casal2 and plots the likelihood/objective profile for one or many observations over the specified parameters range. This range is defined in the profile block of the Casal2 configuration file. Casal2 calculates the profile steps as even intervals between the upper and lower bound by = (upper_bound - lower_bound) / (steps + 1)
#'
#' @author Craig Marsh
#' @param obs "vector<string>" A vector of report labels that correspond to observations that you would like to plot the profile for. Can be many observtions
#' @param param "string" The parameter label that the profile was run for. It should follow the syntax that is used in Casal2 that is, "block[label].parameter" e.g. process[Recruitment].b0
#' @param extract_list "string" The name of the R object that was extracted into R usign the extract() function.
#' @param Rescale "bool/logical" If true it will rescale the likelihoods to have a minimum on 0. This is done by subtracting off the minimum value of the likelihood series.
#' @param ylab "string" optional, y-axis label
#' @param xlab "string" optional, x-axis label
#' @param ymin "numeric" optional, minimum value parsed to the ylim parameter in plot()
#' @export
#'
"param.profile" <-
  function(obs, param,extract_list, Rescale = F,ylab = "Objective contribution", xlab = "Parameter",y_min = -0.5) {
  ## -1 check that the extract list exists.
  ## -2 check that there is a report of type "estimate_value"
  ## -3 check that the parameter exists
  ## -4 check that the observation report exists
  ## Check 1
  if (!evalit(Paste("exists('",extract_list,"')"))) {
    stop(Paste("extract_list object '", extract_list,"' does not seem to exist in your R workspace please check spelling"))
  }
  list_labs = evalit(Paste("names(",extract_list,")"));
  param_list = ""
  ## Check 2
  for (lab in list_labs) {
    this_list_type = evalit(Paste(extract_list,"$",lab,"$'1'$type"))
    if (this_list_type == "estimate_value") {
      param_list = lab;
    }
  }
  if (param_list == "") {
    stop("Could not find a report of type 'estimate_value'. This report type is needed for this function to work")
  }
  ## Check 3
  params = names(evalit(Paste(extract_list,"$",param_list,"$'1'$values")))
  if (!param %in% params) {
    stop(Paste("Could not find parameter '",param,"' in the estimate_value report, please check spelling"))
  }
  ## Check 4
  if (!all(obs %in% list_labs)) {
     stop(Paste("Could not find observation report '", paste(obs[!obs %in% list_labs],collapse = ", "),"' in extract_list, please check spelling"))
  }  
  ## Pull out all the relevant information
  Param_vals = vector();
  iter = 1;
  for(i in names(evalit(Paste(extract_list,"$",param_list)))) {
    Param_vals[iter] = as.numeric(eval(parse(text = Paste(extract_list,"$",param_list,"$'",i,"'$values['",param,"']",sep = ""))))
    iter = iter + 1;
  }
  ## get the objective values
  n_obs = length(obs)
  like_mat = matrix(NA, ncol = length(Param_vals), nrow = n_obs)
  ob_iter = 1; 
  for(i in 1:n_obs) {
    step_iter = 1;
    this_ob = Paste(extract_list,"$",obs[i])
    steps = names(evalit(this_ob))
    for(j in steps) {
      step_ob = evalit(Paste(this_ob,"$'",j,"'$Comparisons"))
      like_mat[ob_iter, step_iter] = Sum(step_ob$score)
      step_iter = step_iter + 1;
    }
  ob_iter = ob_iter + 1;
  }  
  ## now plot
  if(Rescale) {
    like_mat = like_mat - apply(like_mat,1,min)
  }
  y_max = Max(as.vector(like_mat))
  palette(rainbow(n_obs))
  if(Rescale) {
    plot(Param_vals,like_mat[1,], ylim = c(y_min,y_max), type = "o", col = 1, ylab = Paste("Rescaled ", ylab), xlab = xlab)
  } else {
    plot(Param_vals,like_mat[1,], ylim = c(y_min,y_max), type = "o", col = 1, ylab = ylab, xlab = xlab)  
  }
  for(i in 2:n_obs) {
    lines(Param_vals,like_mat[i,],type = "o", col = i)
  }
  if (Rescale)
    abline(h = 0)
  legend('topright', col = 1:n_obs, legend = obs, lty = 1,pch = 1)
}

