#' @title summary default
#'
#' @description
#' A summary function for 'casal2MCMC' 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author C. Marsh
#' @param model <casal2MPD, casal2TAB, casal2MCMC> object that are generated from one of the extract() functions.
#' @rdname summary
#' @export summary
#'

"summary.default"<-
function(model){
  UseMethod("summary",model)
}

#' @return \code{NULL}
#'
#' @rdname summary
#' @method summary casal2MPD
#' @export
"summary.casal2MPD" = function(model) {
  ## What do we want to summarise for each report.
  ## current report types that we will summarise = {estimate_value, process[recruitment_bevertont_holt], processp[mortality_instantaneous], warnings}
  ## iterate through each report and ndx which reports we can summarise
  allowed_report_types = c("estimate_value","process","warnings_encounted")
  ## maybe think about prioritising some of this summary later, like put the warning at the end as that is likely where users will see it.
  for(i in names(model)) {
      this_report = get(i, model)
      report_type = this_report$'1'$type
      if (report_type %in% allowed_report_types) {
        if (report_type == "estimate_value") {
          summarise_estimate_values(this_report$'1')
        } else if (report_type == "process") {
          if(this_report$'1'$process_type %in% c("recruitment_beverton_holt","mortality_instantaneous")) {
            cat(Paste("Summarising process ", i,"\n"))
            summarise_process(this_report$'1')
          }
        } else if (report_type == "warnings_encounted") {
          summarise_warnings_encounted(this_report$'1')
        }
      }
  }
}

## method for class casal2MCMC
#' @return \code{NULL}
#'
#' @rdname summary
#' @method summary casal2MCMC
#' @export
"summary.casal2MCMC" = function(model) {
  ## What do we want to summarise for each MCMC
  ## Trace plot of Objective function
  ## marginal posterior's of b0,M,q
  
  ##############
  ## Trace Plot
  #############
  plot(model$"sample"/1000, model$"objective_score", xlab = "Iteration (000's)", ylab = "Objective Score", main = "Trace Plot", type = "l")

  
}


## method for class casal2TAB


