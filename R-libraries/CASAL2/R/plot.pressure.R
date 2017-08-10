#' @title plot.pressure plot fishing pressure if there has been an exploitation process reported.
#'
#' @description
#' A plotting function to plot fishing presuure (U's )for the 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author C. Marsh
#' @param model <casal2MPD, casal2TAB> object that are generated from one of the extract.mpd() and extract.tabular() functions.
#' @param report_label <string>
#' @param ... remaining plotting functions.
#' @rdname plot.pressure
#' @export plot.pressure
#'
#' @examples
#' library(casal2)
#' data <- extract.mpd(file = system.file("extdata", "MPD.log", package="casal2"))
#' names(data)
#' plot.pressure(model = data, report_label = "exploit", col = c("black", "red"))
#' # if you are unhappy with the default plotting you can use plot.it = FALSE and create a plot of your own.
#' Fish_pressure = plot.pressure(model = data, report_label = "exploit", plot.it = FALSE)
"plot.pressure" <-
function(model, report_label="", xlim, ylim, xlab, ylab, main, col,plot.it = T, ...){
  UseMethod("plot.pressure",model)
}

#' @return \code{NULL}
#'
#' @rdname plot.pressure
#' @method plot.pressure casal2MPD
#' @export
"plot.pressure.casal2MPD" = function(model, report_label="", xlim = NULL, ylim = NULL, xlab = NULL, ylab = NULL, main = NULL, col = NULL ,plot.it = T, ...) {
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
  if (this_report$'1'$process_type != "mortality_instantaneous") {
    stop(Paste("The process type in report ", report_label, " is not a mortality_instantaneous plz Check you have specified the correct report_label."))     
  }  
  if (length(this_report) > 1) {
      print("multi iteration report found")
      muliple_iterations_in_a_report = TRUE;
      N_runs = length(this_report);
  }
  if (!muliple_iterations_in_a_report) {
    ## only a single trajectory
    f_ndx = grepl(pattern = "fishing_pressure", names(this_report$'1'))
    start_index = as.numeric(regexpr(pattern = "\\[",text = names(this_report$'1')[f_ndx])) + 1
    stop_index = as.numeric(regexpr(pattern = "\\]",text = names(this_report$'1')[f_ndx])) - 1
    fisheries = substring(names(this_report$'1')[f_ndx], start_index,last = stop_index)
    years =  this_report$'1'$year
    first_fishery = TRUE;
    if(missing(col)) {
      palette(gray(seq(0.,.90,len = length(fisheries))))
      Cols = palette()
    } else {
      Cols = col;
    }
    ## create a plot
    for ( i in 1:length(fisheries)) {
      values = this_report$'1'[[which(f_ndx)[i]]]
      ## does the user want it plotted as percent B0
      
      if(missing(ylim)) {
        ymax = max(values) + quantile(values, 0.05) 
        ylim = c(0, ymax)
      }    
      if(missing(xlim)) 
        xlim = c(min(years) - 1, max(years) + 1)    
      if(missing(ylab)) 
        ylab = "Fishing pressure (Exploitation Rate)"
      
      if(missing(xlab)) 
        xlab = "Years"     
      if(missing(main)) 
        main = ""
      if (plot.it == TRUE && first_fishery) {
        plot(years, values, xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, type = "o", col = Cols[i], ...)
        first_fishery = FALSE;        
      } else if (plot.it == TRUE && !first_fishery) {
        lines(years, values, col = Cols[i], type = "o")
      } else {
        temp_DF = cbind(values,temp_DF);
      }
    }  
    if (plot.it == FALSE) {
      colnames(temp_DF) = fisheries
    } else {
      legend('topright', legend = fisheries, col = Cols, lty = 1)
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