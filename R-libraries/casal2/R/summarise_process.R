#' Utility summarise_process function
#'
#' @author Craig Marsh
#' @description
#' used in the summarise function for casal2MPD
#' @keywords internal
#'

summarise_process = function(report_object) {
  cat(Paste("Process type: ",report_object$process_type, "\n\n"))
  cat(Paste("Process label: ",report_object$process, "\n"))
  if (report_object$sub_type == "mortality_instantaneous") {
    ## Summarise Instantaneous Mortality
    cat("Natural mortality by category (does not take selectivity into account)\n")
    cat(Paste("Category: ",paste(report_object$categories, collapse = " "), "\n"))
    cat(Paste("M: ",paste(report_object$m, collapse = " "), "\n\n"))

    cat("Fishing Pressure by fishery and year \n")
    F = grep(pattern = "fishing_pressure", names(report_object))
    stop_index = as.numeric(regexpr(pattern = "\\]",text = names(report_object[F]))) - 1
    fisheries = substring(names(report_object[F]), first = 18,last = stop_index)
    ## create a matrix
    data = c()
    for (i in F) {
      data = cbind(data, report_object[[i]])
    }
    data = round(data,3)
    colnames(data) = c(fisheries)
    rownames(data) = report_object$year
    print(data)
    cat("\n\n")

    cat("Actual Catch by fishery and year \n")
    Catch = grep(pattern = "actual_catch", names(report_object))
    stop_index = as.numeric(regexpr(pattern = "\\]",text = names(report_object[Catch]))) - 1
    fisheries = substring(names(report_object[Catch]), first = 14,last = stop_index)

    ## create a matrix
    data = c()
    for (i in Catch) {
      data = cbind(data, report_object[[i]])
    }
    data = round(data,0)
    colnames(data) = c(fisheries)
    rownames(data) = report_object$year
    print(data)
    cat("\n\n")
  } else if (report_object$sub_type == "recruitment_beverton_holt") {
    cat(Paste("B0: ", report_object$b0), "\n")
    cat(Paste("R0: ", report_object$r0), "\n")
    cat(Paste("SSB_label: ", report_object$ssb), "\n")
    cat(Paste("steepness: ", report_object$steepness), "\n")
    cat(Paste("age enter partition: ", report_object$age), "\n")
    ## Standarsised YCS range
    cat(Paste("standardised ycs in years: ", paste(report_object$standardise_ycs_years, collapse = " "), "\n"))
    ## print ycs, true_ycs, and standardised_ycs
    data = cbind(report_object$ycs_values,report_object$true_ycs,report_object$standardised_ycs)
    data = round(data,2)
    dimnames(data) = list(report_object$ycs_years,c("Estimated YCS","True YCS","Standardised YCS"))
    print(data)
    cat("\n\n")
  }
}

