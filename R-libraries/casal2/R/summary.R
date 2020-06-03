#' Utility summary function
#'
#' This function summarises information in 'casal2MCMC', 'casal2TAB', and 'casal2MPD' objects.
#'
#' @author C. Marsh
#' @param model The <casal2MPD, casal2TAB, casal2MCMC> object that are generated from one of the extract() functions.
#' @return \code{NULL}
#' @rdname summary
#' @export summary
 "summary.default"<-
function(model){
  UseMethod("summary",model)
}

#' summary casal2MPD
#'
#' @param model The casal2MPD object
#' @return \code{NULL}
#' @usage \method{summary}{casal2MPD}(model)
#' @rdname summary
#' @export
"summary.casal2MPD" = function(model) {
  ## What do we want to summarise for each report.
  ## current report types that we will summarise = {estimate_value, process[recruitment_bevertont_holt], processp[mortality_instantaneous], warnings}
  ## iterate through each report and ndx which reports we can summarise
  allowed_report_types = c("estimate_value", "estimate_summary", "process","warnings_encounted")
  ## maybe think about prioritising some of this summary later, like put the warning at the end as that is likely where users will see it.
  for(i in names(model)) {
      this_report = get(i, model)
      # to account for the differences between run output and estimation output
      if (names(this_report)[1] == "1") {
        this_report = this_report[[1]]
      }
      report_type = this_report$type
      if (report_type %in% allowed_report_types) {
        if (report_type == "estimate_value") {
          summarise_estimate_values(this_report)
        } else if (report_type == "estimate_summary") {
            summarise_estimate_summary(this_report)
        } else if (report_type == "process") {
          if(this_report$sub_type %in% c("recruitment_beverton_holt","mortality_instantaneous")) {
            cat(Paste("Summarising process ", i,"\n"))
            summarise_process(this_report)
          }
        } else if (report_type == "warnings_encounted") {
          summarise_warnings_encounted(this_report)
        }
      }
  }
}

#' summary casal2MCMC
#'
#' @param model The casal2MCMC object
#' @return \code{NULL}
#' @usage \method{summary}{casal2MCMC}(model)
#' @rdname summary
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


#' summary casal2TAB
#'
#' @param model The casal2TAB object
#' @return \code{NULL}
#' @usage \method{summary}{casal2TAB}(model)
#' @rdname summary
#' @export
"summary.casal2TAB" = function(model) {

}

