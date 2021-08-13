#' Utility summarise_estimate_summary function
#'
#' @description
#' used in the summarise function for casal2MPD
#' @keywords internal
#'

summarise_estimate_summary <- function(report_object) {
  n_est <- length(report_object)
  if (n_est > 1) {
    # omit last row, which is "type"
    data <- data.frame()
    for (i in 1:(n_est - 1)) {
      ro <- report_object[i][[1]]
      df_row <- data.frame(Parameter = ro$parameter, Label = ro$label, Dist = ro$sub_type, LB = ro$lower_bound, UB = ro$upper_bound, Value = ro$value)
      if (length(ro$std_dev) > 0) {
        df_row <- data.frame(df_row, SD = ro$std_dev)
      }

      data <- rbind(data, df_row)
    }

    names(data) <- c("Parameter", "Label", "Distribution", "Lower Bound", "Upper Bound", "Value")
    if (dim(data)[2] >= 7) {
      names(data)[7] <- "Std. Dev."
    }

    cat("Table of estimated values and characteristics\n")
    print(data)
  }

  cat("\n\n")
}
