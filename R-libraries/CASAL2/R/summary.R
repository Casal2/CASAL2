#' @title summary default
#'
#' @description
#' A summary function for 'casal2MCMC' 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author C. Marsh
#' @param model <casal2MPD, casal2TAB, casal2MCMC> object that are generated from one of the extract() functions.
#' @rdname summary
#' @export summary
#'

"summary.default"<-
function(model){
  UseMethod("summary",model)
}

#' @return \code{NULL}
#'
#' @rdname summary
#' @method summary casal2MPD
#' @export
"summary.casal2MPD" = function(model) {
  if (!type %in% c("number", "percent")) {
    stop ("the parameter type must be: 'number' or 'percent'")
  }
  muliple_iterations_in_a_report = FALSE;
  N_runs = 1;
  temp_DF = NULL;

  ## check report label exists
  if (!report_label %in% names(model))
    stop(Paste("In model the report label '", report_label, "' could not be found. The report labels available are ", paste(names(model),collapse = ", ")))
  ## get the report out
  this_report = get(report_label, model)
  ## check that the report label is of type derived_quantity
  if (this_report$'1'$type != "derived_quantity") {
    stop(Paste("The report label ", report_label, " in model is not a derived quantity plz Check you have specified the correct report_label."))     
  }
  if (length(this_report) > 1) {
      print("multi iteration report found")
      muliple_iterations_in_a_report = TRUE;
      N_runs = length(this_report);
  }
  if (!muliple_iterations_in_a_report) {
    ## only a single trajectory
    this_report = get(report_label, this_report$'1')
    values = this_report$values
    zero_ndx = values == 0;
    if (any(zero_ndx)) {
      ## I am going to assume that these are in the projection phase and am going to remove them
      values = values[!zero_ndx]      
    }
    years = as.numeric(names(values))
    ## does the user want it plotted as percent B0
    if (type == "percent")
      values = values / this_report$B0 * 100
    if(missing(ylim)) {
      ymax = max(values) + quantile(values, 0.05) 
      ylim = c(0, ymax)
    }    
    if(missing(xlim)) 
      xlim = c(min(years) - 1, max(years) + 1)    
    if(missing(ylab)) {
      if (type == "percent")
        ylab = "%B0"
      else
        ylab = "Biomass (t)"
    }
    if(missing(xlab)) 
      xlab = "Years"
    if(missing(col)) 
      col = "black"      
    if(missing(main)) 
      main = report_label  
    if (plot.it == TRUE) {
      plot(years, values, xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, type = "o", ...)
    } else {
      temp_DF = values;
    }
  } else {
    ## Multiple trajectory's
    first_run = TRUE;
    Legend_txt = c();
    #print(Paste("found - ",length(this_report), " iterations"))
    if(missing(col)) {
      palette(gray(seq(0.,.90,len = N_runs)))
      Cols = palette()
    } else {
      Cols = col;
    }
    for (i in 1:length(this_report)) {
      this_derived_quantity = this_report[[i]]
      this_derived_quantity = get(report_label, this_derived_quantity)
      values = this_derived_quantity$values
      zero_ndx = values == 0;
      if (any(zero_ndx)) {
        ## I am going to assume that these are in the projection phase and am going to remove them
        values = values[!zero_ndx]      
      }
      years = as.numeric(names(values))
      ## does the user want it plotted as percent B0
      if (type == "percent")
        values = values / this_derived_quantity$B0 * 100
        
      Legend_txt = c(Legend_txt,this_derived_quantity$B0)  

      if (first_run) {      
        if(plot.it == FALSE) {
          temp_DF = values
        }      
        if(missing(ylim)) {
          ymax = max(values) + quantile(values, 0.15) 
          ylim = c(0, ymax)
        }    
        if(missing(xlim)) 
          xlim = c(min(years) - 1, max(years) + 1)    
        if(missing(ylab)) {
          if (type == "percent")
            ylab = "%B0"
          else
            ylab = "Biomass (t)"
        }
        if(missing(xlab)) 
          xlab = "Years"    
        if(missing(main)) 
          main = report_label      
        if (plot.it == TRUE) {  
          plot(years, values , xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, type = "o", lwd = 3, col = Cols[i] ,...)        
        }
      } else {
        if (plot.it == TRUE) {
          lines(years, values, type = "o", lwd = 3, col = Cols[i])          
        } else {
          temp_DF = rbind(values, temp_DF)
        }
      }
      first_run = FALSE;
    }  
    if (plot.it == TRUE)
      legend('bottomleft',legend = Legend_txt, col = Cols, lty = 2, lwd = 3)

  }
  if (plot.it == FALSE)
    return(temp_DF)
}

## method for class casal2TAB