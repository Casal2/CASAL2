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


"plot.recruitment"<-
function(model, report_label="", xlim, ylim, xlab, ylab, main, col, ...){
  UseMethod("plot.ycs",model)
}

#' @return \code{NULL}
#'
#' @rdname plot.recruitment
#' @method plot.recruitment casal2MPD
#' @export
"plot.recruitment.casal2MPD" = function(model, report_label="", xlim = NULL, ylim = NULL, xlab = NULL, ylab = NULL, main = NULL, col = NULL, ...) {
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
#  if (this_report$'1'$type != "recruitment_beverton_holt" || is.null(this_report$'1'$type)) {
#    stop(Paste("The process type in report ", report_label, " is not a recruitment_beverton_holt plz Check you have specified the correct report_label."))     
#  }  
  if (length(this_report) > 1) {
      print("multi iteration report found")
      muliple_iterations_in_a_report = TRUE;
      N_runs = length(this_report);
  }
  if (!muliple_iterations_in_a_report) {
    Ry = this_report$'1'$Recruits / 1000
    SSBy = this_report$'1'$SSB
    ycs_years = this_report$'1'$ycs_years
    ## does the user want it plotted as percent B0
    if(missing(ylim)) {
      ymax = max(Ry) + quantile(Ry, 0.05) 
      ylim = c(0, ymax)
    } 
    
    if(missing(col))    
      Col = "black"
    else 
      Col = col
    if(missing(xlim)) {
      xmax = max(SSBy) + 1000
      xlim = c(0, xmax)
    } 
    
    if(missing(ylab)) 
      ylab = "Recruitment (000's)"

    if(missing(xlab)) 
      xlab = "Spawning stock biomass"     
    if(missing(main)) 
      main = ""
    plot(SSBy, Ry, xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, col = Col, pch = 19, ...)

    ## fit the S-R relationship
    #if (Beverton_holt) {
      ssb_ratio = 0:xmax / this_report$'1'$b0;
      SR = ssb_ratio / (1.0 - ((5.0 * this_report$'1'$steepness - 1.0) / (4.0 * this_report$'1'$steepness)) * (1.0 - ssb_ratio));
      Ry_hat = (this_report$'1'$r0 * SR) / 1000
      lines(x = 0:xmax, y = Ry_hat, lwd = 2, col = "red")
      abline(h = this_report$'1'$r0 / 1000, lty = 2)
      
    #}
    
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
#' @rdname plot.recruitment
#' @method plot.recruitment casal2TAB
#' @export
"plot.recruitment.casal2TAB" = function(model, report_label="", xlim, ylim, xlab, ylab, main, col, ...) {
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
