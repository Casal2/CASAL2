#' @title plot.recruitment plot the stock recruitment relationship from a Casal2 model output.
#'
#' @description
#' A plotting function to plot the stock recruitment relationship for the 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author Craig Marsh
#' @param model <casal2MPD, casal2TAB> object that are generated from one of the extract.mpd() and extract.tabular() functions.
#' @param report_label <string> the report label containing recruitment process
#' @param ... remaining plotting options
#' @return generate a plot of SSBy vs Ry with the assumed Stock recruitment relationship through it.
#' @rdname plot.recruitment
#' @export plot.recruitment

"plot.recruitment" <-
function(model, report_label = "", xlim, ylim, xlab, ylab, main, col, ...) {
  UseMethod("plot.recruitment", model)
}

#' @return \code{NULL}
#'
#' @rdname plot.recruitment
#' @method plot.recruitment casal2MPD
#' @export
"plot.recruitment.casal2MPD" = function(model, report_label = "", add_BH_curve = TRUE, xlim = NULL, ylim = NULL, xlab = NULL, ylab = NULL, main = NULL, col = NULL, ...) {
  muliple_iterations_in_a_report = FALSE;
  N_runs = 1;
  temp_DF = NULL;

  ## check report label exists
  if (!report_label %in% names(model))
    stop(paste0("The report label '", report_label, "' was not found. The report labels available are: ", paste(names(model), collapse = ", ")))
  ## get the report out
  this_report = get(report_label, model)
  ## check that the report label is of type "process"

  if (any(names(this_report) == "type")) {
    if (this_report$type != "process")
      stop(paste0("The report label '", report_label, "' is not a process. Please check that the correct report_label was specified."))
    if (substring(this_report$sub_type, first = 1, last = 11) != "recruitment")
      stop(paste0("The report label '", report_label, "' is a process that should be type 'recruitment'"))

  } else {
    print("multi iteration report found")
    muliple_iterations_in_a_report = TRUE;
    N_runs = length(this_report);
    if (this_report$'1'$type != "process")
      stop(paste0("The report label '", report_label, "' is not a process. Please check that the correct report_label was specified."))
    if (substring(this_report$'1'$sub_type, first = 1, last = 11) != "recruitment")
      stop(paste0("The report label '", report_label, "' is a process that should be type 'recruitment'"))
  }

  if (!muliple_iterations_in_a_report) {
    Ry = this_report$Recruits / 1000
    SSBy = this_report$SSB
    ycs_years = this_report$ycs_years
    ## does the user want it plotted as percent B0
    if (missing(ylim)) {
      ymax = max(Ry) + quantile(Ry, 0.05)
      ylim = c(0, ymax)
    }

    if (missing(col))
      Col = "black"
    else
      Col = col
    if (missing(xlim)) {
      xmax = max(SSBy) + 1000
      xlim = c(0, xmax)
    }

    if (missing(ylab))
      ylab = "Recruitment (000's)"

    if (missing(xlab))
      xlab = "Spawning stock biomass"
    if (missing(main))
      main = ""
    plot(SSBy, Ry, xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, col = Col, pch = 19, ...)

    ## fit the S-R relationship
    if (add_BH_curve) {
      ssb_ratio = 0:xmax / this_report$b0;
      SR = ssb_ratio / (1.0 - ((5.0 * this_report$steepness - 1.0) / (4.0 * this_report$steepness)) * (1.0 - ssb_ratio));
      Ry_hat = (this_report$r0 * SR) / 1000
      lines(x = 0:xmax, y = Ry_hat, lwd = 2, col = "red")
      abline(h = this_report$r0 / 1000, lty = 2)

    }

  } else {
    ## Multiple trajectory's
    stop("This function does not take multiple inputs.")
  }
  invisible();
}

## method for class casal2TAB
#' @return \code{NULL}
#'
#' @rdname plot.recruitment
#' @method plot.recruitment casal2TAB
#' @export
"plot.recruitment.casal2TAB" = function(model, report_label = "", xlim, ylim, xlab, ylab, main, col, ...) {
  ## check report label exists
  if (!report_label %in% names(model))
    stop(paste0("The report label '", report_label, "' was not found. The report labels available are: ", paste(names(model), collapse = ", ")))
  ## get the report out
  this_report = get(report_label, model)
  ## check that the report label is of type derived_quantity
  if (this_report$type != "process") {
    stop(paste0("The report label '", report_label, "' is not a derived quantity. Please check that the correct report_label was specified."))
  }
  #  if (this_report$process_type != "recruitment_beverton_holt" || is.null(this_report$process_type)) {
  #    stop(paste0("The process type in report '", report_label, "' is not 'recruitment_beverton_holt'. Please check that the correct report_label was specified."))
  #  }


  Labs = colnames(this_report$values);
  true_ycs = this_report$values[, grepl(pattern = "true_ycs", x = Labs)]
  start_index = as.numeric(regexpr(pattern = "\\[", text = colnames(true_ycs))) + 1
  stop_index = as.numeric(regexpr(pattern = "\\]", text = colnames(true_ycs))) - 1
  years = as.numeric(substring(colnames(true_ycs), start_index, last = stop_index))

  vals = apply(true_ycs, 2, quantile, c(0.025, 0.5, 0.975))
  ## create a multi-plot panel
  plot(years, vals["50%",], ylim = c(0, max(vals)), xlab = "years", ylab = "True YCS", type = "l", main = "DQ_s")
  polygon(x = c(years, rev(years)), y = c(vals["2.5%",], rev(vals["97.5%",])), col = "gray60")
  lines(years, vals["50%",], col = "red", lwd = 2)
  abline(h = 1.0, lty = 2)

  return(this_report$values)
  invisible();

}
