#' @title plot.selectivities plot selectivities from a Casal2 model.
#'
#' @description
#' A plotting function to plot selectivities for the 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author Craig Marsh
#' @param model <casal2MPD, casal2TAB> object that are generated from one of the extract.mpd() and extract.tabular() functions.
#' @param report_label vector<string>
#' @param ... remaining plotting options
#' @return generate a plot by age or length
#' @rdname plot.selectivities
#' @export plot.selectivities


"plot.selectivities" <-
function(model, report_labels = c(""), xlim, ylim, xlab, ylab, main, col, ...) {
  UseMethod("plot.selectivities", model)
}

#' @return \code{NULL}
#'
#' @rdname plot.selectivities
#' @method plot.selectivities casal2MPD
#' @export
"plot.selectivities.casal2MPD" = function(model, report_labels = c(""), xlim = NULL, ylim = NULL, xlab = NULL, ylab = NULL, main = NULL, col = NULL, ...) {
  muliple_iterations_in_a_report = FALSE;
  N_runs = 1;
  temp_DF = NULL;
  n_reports = length(report_labels)
  if (n_reports <= 0) {
    stop("At least one report_label is required")
  }
  ## check report label exists
  if (!all(report_labels %in% names(model)))
    stop(paste0("The report labels '", paste(report_labels[!report_labels %in% names(model)], collapse = ", "), "' were not found. The report labels available are: ", paste(names(model), collapse = ", ")))

  ## Check we aren't dealing with an output that is multiple reports
  all_ages = c();
  for (i in 1:n_reports) {
    this_report = get(report_labels[i], model)
    if (any(names(this_report) == "type")) {
      if (this_report$type != "selectivity")
        stop(paste0("The report label '", report_labels[i], "' is not a selectivity. Please check that the correct report_label was specified."))
      all_ages = c(all_ages, names(this_report$Values))
    } else
      stop("This function does not take multiple inputs.")
  }
  all_ages = as.numeric(all_ages)
  ## prep for plotting
  if (missing(col)) {
    palette(gray(seq(0.4, .90, len = n_reports)))
    Cols = palette()
  } else {
    if (length(col) != length(report_labels))
      stop(paste0("If the col parameter is specified, it needs to be the same length (", length(report_labels), ") as the number of report labels (", length(report_labels), ")"))
    Cols = col;
  }
  if (missing(ylim)) {
    ymax = 1.2
    ylim = c(0, ymax)
  }
  if (missing(xlim))
    xlim = c(min(all_ages) - 1, max(all_ages) + 1)
  if (missing(ylab))
    ylab = "Selectivity"

  if (missing(xlab))
    xlab = "Ages/Lengths"
  if (missing(main))
    main = ""
  labs =
  for (i in 1:n_reports) {
    this_report = get(report_labels[i], model)
    if (i == 1) {
      plot(names(this_report$Values), this_report$Values, xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, type = "l", col = Cols[i], ...)
    } else {
      lines(names(this_report$Values), this_report$Values, col = Cols[i], ...)
    }
  }
  legend('topright', legend = report_labels, col = Cols, lty = 1, lwd = 2)

  invisible();
}
