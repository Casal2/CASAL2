#' Utility summarise_warnings_encounted function
#'
#' @author Craig Marsh
#' @description 
#' used in the summarise function for casal2MPD
#' @keywords internal
#'

summarise_warnings_encounted = function(report_object) {
  cat("Summarising warnings\n")
  cat(paste0("Found: ", report_object$warnings_found, " warnings\n"))
  ## get warnings
  ndx = grep(pattern = "warning_", x = names(report_object))
  for (i in 1:length(ndx)) {
    cat(paste0(report_object[ndx[i]], "\n"))
  }
  cat("\n\n")
}