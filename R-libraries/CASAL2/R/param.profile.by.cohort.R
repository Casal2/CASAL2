#' Generate an objective score profile plot for a particular parameter and age compositional dataset as a consequence from a casal2 -p run.
#'
#' This function reads an extracted list from Casal2 and plots the likelihood/objective profile for an age compositional observation over the specified parameters range. This range is defined in the profile block of teh Casal2 configuration file. Casal2 calculates the profile steps as even intervals between the upper and lower bound by = (upper_bound - lower_bound) / (steps + 1). The plot will generate a line for each cohort and category. The reason I made this function was for the specific investigation of the natural mortality parameter M. Sometimes running a profile of aggregated compositional data on a parameter is not that informative. The aim of this plot is to show which cohorts are suggestive of certain parameters values, this will either create more confusion (most likely) or at least give the user something interesting to think about (perhaps cohort based processes).
#'
#' @author Craig Marsh
#' @param obs "string" A report label that correspond to the age compositional observation that you would like to plot the profile for. Can be many observtions
#' @param param "string" The parameter label that the profile was run for. It should follow the syntax that is used in Casal2 that is, "block[label].parameter" e.g. process[Recruitment].b0
#' @param extract_list "string" The name of the R object that was extracted into R usign the extract() function.
#' @param Rescale "bool/logical" If true it will rescale the likelihoods to have a minimum on 0. This is done by subtracting off the minimum value of the likelihood series.
#' @param threshold "numeric" the minimum number of years a cohort is seen in the observation, for being used in the plot. if threshold = 5 then any cohort that has been sampled for a minimum of 5 years will be automatically plotted.
#' @param ylab "string" optional, y-axis label
#' @param xlab "string" optional, x-axis label
#' @param ymin "numeric" optional, minimum value parsed to the ylim parameter in plot()
#' @export
#'
#' @examples
#' library(CASAL2)
#' Profile = extract("profile.log") ## this is an output file from a casal2 -p run
#' param.profile.by.cohort(obs = c("Tangaroa_propn_at_age_Jan"), param = "process[Instantaneous_Mortality].m(male)",extract_list = "Profile", Rescale = T,ylab = "Objective contribution", xlab = "M",threshold = 10)
#' 
"param.profile.by.cohort" <-
  function(obs, param,extract_list, Rescale = F,ylab = "Objective contribution", xlab = "Parameter",y_min = -0.5, threshold = 10) {
    ## -1 check that the extract list exists.
    ## -2 check that there is a report of type "estimate_value"
    ## -3 check that the parameter exists
    ## -4 check that the observation report exists
    ## -5 check observation is an age_compositional dataset
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
    ## Check 5
    if (length(obs) != 1)
       stop("You can only specify a single compositional dataset for this function")
    this_ob = Paste(extract_list,"$",obs,"$'1'$Comparisons")
    this_ob = evalit(this_ob)
    if(any(this_ob$age == 0)) {
      stop("Found values of 0 in the ages. This function will only work if there are non-zero values in the ages")
    }
    n_years = length(unique(this_ob$year));
    n_categories = length(unique(this_ob$category))
    categories = unique(this_ob$category)
    years = as.numeric(unique(this_ob$year))
    min_age = min(this_ob$age)
    Tab = table(this_ob$year - this_ob$age) / n_categories
    cohorts_considered = Tab[Tab >= threshold];
    cohort_years = names(cohorts_considered)
    ## Pull out all the relevant information
    Param_vals = vector();
    iter = 1;
    for(i in names(evalit(Paste(extract_list,"$",param_list)))) {
      Param_vals[iter] = as.numeric(eval(parse(text = Paste(extract_list,"$",param_list,"$'",i,"'$values['",param,"']",sep = ""))))
      iter = iter + 1;
    }
    
    ## get the objective values
    like_mat = array(NA, dim = c(n_categories,length(cohorts_considered),length(Param_vals)))
    step_iter = 1;
    this_ob = Paste(extract_list,"$",obs)
    steps = names(evalit(this_ob))
    for(j in steps) {
      step_ob = evalit(Paste(this_ob,"$'",j,"'$Comparisons"))
      for(Category in 1:n_categories) { 
        this_cat = step_ob[step_ob$category == categories[Category],]
        this_cohort_years = this_cat$year - this_cat$age
        for(k in 1:length(cohorts_considered)) {
            ndx = this_cohort_years == cohort_years[k];
            like_mat[Category,k,step_iter] = Sum(this_cat[ndx,"score"])
        }
      }
    step_iter = step_iter + 1;
    }
  
    ## now plot
    if(Rescale) {
      for(Category in 1:n_categories) {     
        like_mat[Category,,] = like_mat[Category,,] - apply(like_mat[Category,,],1,min)
      }
    }
    y_max = Max(as.vector(like_mat))
    palette(rainbow(length(cohorts_considered)))
    par(mfrow = c(1,n_categories))
    for(Category in 1:n_categories) {     
      plot(Param_vals,like_mat[Category,1,], ylim = c(y_min,y_max), type = "o", col = 1, ylab = ylab, xlab = xlab,main = categories[Category])  
      for(i in 2:length(cohorts_considered)) {
        lines(Param_vals,like_mat[Category,i,],type = "o", col = i)
      }
      if (Rescale)
        abline(h = 0)
    }  
    legend('topright', col = 1:length(cohorts_considered), legend = cohort_years, lty = 1, pch = 1,cex = 0.6, title = "cohort")
}
