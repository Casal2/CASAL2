#' @title plot.ycs plot true Year Class Strengths from a Casal2 model.
#'
#' @description
#' A plotting function to plot YCS for the 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author Craig Marsh
#' @param model <casal2MPD, casal2TAB> object that are generated from one of the extract.mpd() and extract.tabular() functions.
#' @param report_label <string>
#' @param plot.it Whether to generate a default plot or return the values as a matrix.
#' @param ... remaining plotting options
#' @return generate a plot over time if plot.it = T, if plot.it = F it will return a matrix of values.
#' @rdname plot.ycs
#' @export plot.ycs


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

  if (any(names(this_report) == "type")) {
    print("single iteration report found")
    if (this_report$type != "process") 
      stop(Paste("The report label ", report_label, " in model is not a process plz Check you have specified the correct report_label."))    
    if (substring(this_report$sub_type, first = 1, last = 11) != "recruitment") 
      stop(Paste("The report label ", report_label, " in model is a process, that needs to be of type recruitment"))    
  } else {
    print("multi iteration report found")
    muliple_iterations_in_a_report = TRUE;
    N_runs = length(this_report);
    if (this_report$'1'$type != "process") 
      stop(Paste("The report label ", report_label, " in model is not a process plz Check you have specified the correct report_label."))     
    if (substring(this_report$'1'$sub_type, first = 1, last = 11) != "recruitment") 
      stop(Paste("The report label ", report_label, " in model is a process, that needs to be of type recruitment"))    
  }   
  
  if (!muliple_iterations_in_a_report) {
    values = this_report$true_ycs
    ycs_years = this_report$ycs_years
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

## method for class casal2TAB
#' @return \code{NULL}
#'
#' @rdname plot.ycs
#' @method plot.ycs casal2TAB
#' @export
"plot.ycs.casal2TAB" = function(model, report_label="", xlim, ylim, xlab, ylab, main, col,plot.it = T, ...) {
  ## check report label exists
  if (!report_label %in% names(model))
    stop(Paste("In model the report label '", report_label, "' could not be found. The report labels available are ", paste(names(model),collapse = ", ")))
  ## get the report out
  this_report = get(report_label, model)
  ## check that the report label is of type derived_quantity
  if (this_report$type != "process") {
    stop(Paste("The report label ", report_label, " in model is not a derived quantity plz Check you have specified the correct report_label."))     
  }
#  if (this_report$process_type != "recruitment_beverton_holt" || is.null(this_report$process_type)) {
#    stop(Paste("The process type in report ", report_label, " is not a recruitment_beverton_holt plz Check you have specified the correct report_label."))     
#  }  
    
  if (plot.it) {
    Labs = colnames(this_report$values);
    true_ycs = this_report$values[,grepl(pattern = "true_ycs", x = Labs)]
    start_index = as.numeric(regexpr(pattern = "\\[",text = colnames(true_ycs))) + 1
    stop_index = as.numeric(regexpr(pattern = "\\]",text = colnames(true_ycs))) - 1
    years = as.numeric(substring(colnames(true_ycs), start_index,last = stop_index))
    
    vals = apply(true_ycs,2,quantile,c(0.025,0.5,0.975))
    ## create a multi-plot panel
    plot(years,vals["50%",],ylim = c(0, max(vals)), xlab = "years", ylab = "True YCS", type = "l", main = "DQ_s")
    polygon(x = c(years, rev(years)), y = c(vals["2.5%",], rev(vals["97.5%",])), col = "gray60")
    lines(years,vals["50%",], col = "red", lwd = 2)
    abline(h = 1.0, lty = 2)
  } else {
    return(this_report$values)
  }
  invisible();  
}
