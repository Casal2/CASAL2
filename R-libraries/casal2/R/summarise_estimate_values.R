#' Utility summarise_estimate_values function
#'
#' @description
#' used in the summarise function for casal2MPD
#' @keywords internal
#'

summarise_estimate_values <- function(report_object) {
  if (length(report_object$values) > 0) {
    df <- data.frame(Value=cbind(t(report_object$values)))

    # check for standard deviations
    if (length(report_object$std_dev) > 0) {
        df <- data.frame(df, sd=cbind(t(report_object$std_dev)))
        names(df)[2] <- "Std. Dev."
    }

    cat("Table of estimated values\n")
    print(df)
  }

  cat("\n\n")
}
