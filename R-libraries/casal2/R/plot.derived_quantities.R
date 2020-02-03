#' @title plot.derived_quantities default
#'
#' @description
#' A plotting function to plot Derived Quantities for the 'casal2TAB' and 'casal2MPD' objects.
#'
#' @author Craig Marsh
#' @param model <casal2MPD, casal2TAB> object that are generated from one of the extract() functions.
#' @param report_label <string>
#' @param type <string> whether numbers or scaled by B0.
#' @param plot.it Whether to generate a default plot or return the values as a matrix.
#' @param ... remaining plotting options
#' @return A plot of derived quantities over time if plot.it = T, if plot.it = F it will return a matrix of derived quantities.
#' @rdname plot.derived_quantities
#' @export plot.derived_quantities

"plot.derived_quantities"<-
function(model, report_label="", type = "number", xlim, ylim, xlab, ylab, main, col,plot.it = T, ...){
  UseMethod("plot.derived_quantities",model)
}

#' @return \code{NULL}
#'
#' @rdname plot.derived_quantities
#' @method plot.derived_quantities casal2MPD
#' @export
"plot.derived_quantities.casal2MPD" = function(model, report_label="", type = "number", xlim, ylim, xlab, ylab, main, col,plot.it = T, ...) {
  if (!type %in% c("number", "percent")) {
    stop ("the parameter type must be: 'number' or 'percent'")
  }
  muliple_iterations_in_a_report = FALSE;
  N_runs = 1;
  temp_DF = NULL;

  ## check report label exists
  if (!report_label %in% names(model))
    stop(Paste("The report label '", report_label, "' was not found. The report labels available are: ", paste(names(model),collapse = ", ")))
  ## get the report out
  this_report = get(report_label, model)
  ## check that the report label is of type derived_quantity

  if (any(names(this_report) == "type")) {
    if (this_report$type != "derived_quantity")
      stop(Paste("The report label '", report_label, "' is not a derived_quantity. Please check that the correct report_label was specified."))

  } else {
    print("multi iteration report found")
    muliple_iterations_in_a_report = TRUE;
    N_runs = length(this_report);
    if (this_report$'1'$type != "derived_quantity")
      stop(Paste("The report label '", report_label, "' is not a derived_quantity. Please check that the correct report_label was specified."))
  }
  if (!muliple_iterations_in_a_report) {
    ## only a single trajectory
    derived_quantities = names(this_report)[names(this_report) != "type"]
    ## create a multi-plot panel
    if (plot.it)
      par(mfrow = c(1,length(derived_quantities)))
    for (i in 1:length(derived_quantities)) {
      this_derived_q = get(derived_quantities[i], this_report)
      values = this_derived_q$values
      zero_ndx = values == 0;
      if (any(zero_ndx)) {
        ## I am going to assume that these are in the projection phase and am going to remove them
        values = values[!zero_ndx]
      }
      years = as.numeric(names(values))
      ## does the user want it plotted as percent B0
      if (type == "percent") {
        B0 = "hello"
        values = values / this_derived_q$`initialisation_phase[1]` * 100
      }
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
        main = derived_quantities[i]
      if (plot.it == TRUE) {
        plot(years, values, xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, type = "o", ...)
      } else {
        temp_DF = cbind(temp_DF,values);
      }
    }
    if (plot.it == FALSE)
      colnames(temp_DF) = derived_quantities
  } else {
    ## Multiple trajectory's
    derived_quantities = names(this_report$'1')[names(this_report$'1') != "type"]

    #print(Paste("found - ",length(this_report), " iterations"))
    if(missing(col)) {
      palette(gray(seq(0.,.90,len = N_runs)))
      Cols = palette()
    } else {
      Cols = col;
    }
    temp_DF = list();
    if (plot.it)
      par(mfrow = c(1,length(derived_quantities)))
    for (k in 1:length(derived_quantities)) {
    temp_df = c()
      first_run = TRUE;
      Legend_txt = c();
      for (i in 1:length(this_report)) {
        this_derived_quantity = this_report[[i]]
        this_derived_quantity = get(derived_quantities[k], this_derived_quantity)
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
            temp_df = values
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
            main = derived_quantities[k]
          if (plot.it == TRUE) {
            plot(years, values , xlim = xlim, ylim = ylim, xlab = xlab, ylab = ylab, main = main, type = "o", lwd = 3, col = Cols[i] ,...)
          }
        } else {
          if (plot.it == TRUE) {
            lines(years, values, type = "o", lwd = 3, col = Cols[i])
          } else {
            temp_df = rbind(temp_df,values)
          }
        }
        first_run = FALSE;
      }
      if (plot.it == TRUE) {
        legend('bottomleft',legend = Legend_txt, col = Cols, lty = 2, lwd = 3)
      } else {
        rownames(temp_df) = as.character(1:length(this_report))
        temp_DF[[derived_quantities[k]]] = temp_df
      }
    }
  }
  if (plot.it == FALSE)
    return(temp_DF)

  invisible();
}

## method for class casal2TAB
#' @return \code{NULL}
#'
#' @rdname plot.derived_quantities
#' @method plot.derived_quantities casal2TAB
#' @export
"plot.derived_quantities.casal2TAB" = function(model, report_label="", type = "number", xlim, ylim, xlab, ylab, main, col,plot.it = T, ...) {
  if (!type %in% c("number", "percent")) {
    stop ("The parameter type must be: 'number' or 'percent'")
  }

  ## check report label exists
  if (!report_label %in% names(model))
    stop(Paste("The report label '", report_label, "' was not found. The report labels available are: ", paste(names(model),collapse = ", ")))
  ## get the report out
  this_report = get(report_label, model)
  ## check that the report label is of type derived_quantity
  if (this_report$type != "derived_quantity") {
    stop(Paste("The report label '", report_label, "' is not a derived quantity. Please check that the correct report_label was specified."))
  }
  if (plot.it) {
    ## Can be multiple SSB's
    Labs = colnames(this_report$values);
    start_index = as.numeric(regexpr(pattern = "\\[",text =Labs)) + 1
    stop_index = as.numeric(regexpr(pattern = "\\]",text = Labs)) - 1
    DQ_s = unique(substring(Labs, start_index,last = stop_index))
    ## create a multi-plot panel
    par(mfrow = c(1,length(DQ_s)))
    for (i in 1:length(DQ_s)) {
      ## pull out label and years
      ndx = grepl(pattern = DQ_s[i], x = Labs)
      this_ssb = this_report$values[,ndx]
      start_nd = as.numeric(regexpr(pattern = "\\]",text = colnames(this_ssb))) + 2
      ## check initialisation phases.
      n_init = sum(grepl(pattern = "initialisation_phase_", x = colnames(this_ssb)))
      B0 = this_ssb[,n_init]
      this_ssb = this_ssb[,n_init:ncol(this_ssb)]
      years = as.numeric(substring(colnames(this_ssb),first = start_nd, last = nchar(colnames(this_ssb)) - 1))
      if (type == "percent") {
        this_ssb = this_ssb /  B0 * 100
      }
      vals = apply(this_ssb, 2, quantile, c(0.025,0.5,0.975))
      ## pull out type of derived Quantity i.e abundance or biomass.
      end_index = as.numeric(regexpr(pattern = "\\[",text = colnames(this_ssb))) - 1
      DQ_type = unique(substring(colnames(this_ssb),first = 0, last = end_index))
      ## cut off 0's they are most likely Projections years.
      zero_nd = vals == 0.0
      zero_cols = apply(zero_nd,2,all)
      vals = vals[,!zero_cols]
      years = years[!zero_cols]
      plot(years,vals["50%",],ylim = c(0, max(vals)), ylab = DQ_type, xlab = "years", type = "l", main = DQ_s[i])
      polygon(x = c(years, rev(years)), y = c(vals["2.5%",], rev(vals["97.5%",])), col = "gray60")
      lines(years,vals["50%",], col = "red", lwd = 2)
    }
  } else {
    return(this_report$values)
  }
  invisible();
}
