#' @title plot.ycs plot Year Class Strengths from a Casal2 model.
#'
#' @description
#' A plotting function to plot YCS for the 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author C. Marsh
#' @param model <casal2MPD, casal2TAB> object that are generated from one of the extract.mpd() and extract.tabular() functions.
#' @param report_label <string>
#' @param ... remaining plotting functions.
#' @rdname plot.ycs
#' @export plot.ycs
#'

"plot.ycs"<-
function(model, report_label="", xlim, ylim, xlab, ylab, main, col,plot.it = T, ...){
  UseMethod("plot.ycs",model)
}

#' @return \code{NULL}
#'
#' @rdname plot.ycs
#' @method plot.ycs casal2MPD
#' @export
"plot.ycs.casal2MPD" = function(model, report_label="", xlim = NULL, ylim = NULL, xlab = NULL, ylab = NULL, main = NULL, col = NULL ,plot.it = T, ...) {
  muliple_iterations_in_a_report = FALSE;
  N_runs = 1;
  temp_DF = NULL;

  ## check report label exists
  if (!report_label %in% names(model))
    stop(Paste("In model the report label '", report_label, "' could not be found. The report labels available are ", paste(names(model),collapse = ", ")))
  ## get the report out
  this_report = get(report_label, model)
  ## check that the report label is of type "process"
  if (this_report$'1'$type != "process") {
    stop(Paste("The report label ", report_label, " in model is not a process plz Check you have specified the correct report_label."))     
  }
  if (this_report$'1'$process_type != "recruitment_beverton_holt") {
    stop(Paste("The process type in report ", report_label, " is not a recruitment_beverton_holt plz Check you have specified the correct report_label."))     
  }  
  if (length(this_report) > 1) {
      print("multi iteration report found")
      muliple_iterations_in_a_report = TRUE;
      N_runs = length(this_report);
  }
  if (!muliple_iterations_in_a_report) {
    values = this_report$'1'$true_ycs
    ycs_years = this_report$'1'$ycs_years
    ## does the user want it plotted as percent B0
    if(missing(ylim)) {
      ymax = max(values) + quantile(values, 0.05) 
      ylim = c(0, ymax)
    }  
    if(missing(col))    
      Col = "black"
    if(missing(xlim)) 
      xlim = c(min(ycs_years) - 1, max(ycs_years) + 1)    
    if(missing(ylab)) 
      ylab = "True YCS"

    if(missing(xlab)) 
      xlab = "YCS Years (y - ssb_offset)"     
    if(missing(main)) 
      main = ""
    if (plot.it == TRUE) {
      plot(ycs_years, values, xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, type = "l", col = Col, ...)
      abline(h = 1.0, lty = 2)
    } else {
      temp_DF = cbind(values,temp_DF);
    }  
  } else {
    ## Multiple trajectory's
    stop("haven't written this function to take multiple outputs yet.")
  }
  if (plot.it == FALSE)
    return(temp_DF)
  invisible();
}
