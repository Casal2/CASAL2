#' @title plot.fits default
#'
#' @description
#' A plotting function to fits to Casal2 observations from a model run.
#'
#' @author Craig Marsh
#' @param model <casal2MPD, casal2TAB> object that are generated from one of the extract() functions.
#' @param report_label <string>
#' @param plot.it Whether to generate a default plot or return the values as a matrix.
#' @param type Whether to plot an observed vs expected (fit) or plot the residuals (resid)
#' @param ... remaining plotting options
#' @return A plot of derived quantities over time if plot.it = T, if plot.it = F it will return a matrix of derived quantities.
#' @rdname plot.fits
#' @export plot.fits
#' @examples
#' library(casal2)
#' # plotting Standard Output
#' data <- extract.mpd(file = system.file("extdata", "estimate.out", package="casal2"))
#' names(data)
#' par(mfrow = c(1,2))
#' plot.fits(model = data, report_label = "westF_at_age")
#' plot.fits(model = data, report_label = "eastF_at_age")
#' # if you are unhappy with the default plotting you can use plot.it = FALSE and create a plot of your own.
#' Tangaroa_fits = plot.fits(model = data, report_label = "eastF_at_age", plot.it = FALSE)

"plot.fits" <- function(model, report_label="", plot.it = T, xlim, ylim, xlab, ylab, main, col, ...) {
  UseMethod("plot.fits",model)
}

#' @return \code{NULL}
#'
#' @rdname plot.fits
#' @method plot.fits casal2MPD
#' @export
"plot.fits.casal2MPD" <- function(model, report_label="", type = "fit" ,plot.it = T, xlim, ylim, xlab, ylab, main, col, ...) {
  multiple_iterations_in_a_report <- FALSE
  N_runs <- 1
  temp_DF <- NULL

  if (type != "fit") {
    stop("This function cannot be used with resid for class 'casal2MPD'. Use type = 'fit'")
  }

  ## check report label exists
  if (!report_label %in% names(model)) {
    stop(Paste("The report label '", report_label, "' was not found. The report labels available are: ", paste(names(model),collapse = ", ")))
  }

  ## get the report out
  this_report <- get(report_label, model)

  likelihoods_allowed  <- c("lognormal", "multinomial", "normal")
  observations_allowed <- c("biomass", "abundance", "proportions_at_age", "process_proportions_at_age", "proportions_at_length", "process_proportions_at_length", "process_removals_by_age", "process_removals_by_length")

  if (any(names(this_report) == "type")) {
    if (this_report$type != "observation") {
      stop(Paste("The report label '", report_label, "' is not a observation. Please check that the correct report label was specified."))
    }
    if (!this_report$observation_type %in% observations_allowed) {
      stop(Paste("This function can be used with observation types " , paste(observations_allowed, collapse = ", "), " only."))
    }
  } else {
    print("multi iteration report found")
    multiple_iterations_in_a_report <- TRUE
    N_runs <- length(this_report)

    if (this_report$'1'$type != "observation") {
      stop(Paste("The report label '", report_label, "' is not an observation. Please check that the correct report label was specified."))
    }
    if (!this_report$'1'$observation_type %in% observations_allowed) {
      stop(Paste("This function can be used with observation types " , paste(observations_allowed, collapse = ", "), " only."))
    }
  }

  if (!multiple_iterations_in_a_report) {
    ## pull out the comparisons oject
    Comparisons <- this_report$Values

    ## transform comparisons to something easier to call
    t_comp <- transform.comparisons(Comparisons)

    if (plot.it) {
      if (type == "fit") {
        if (this_report$likelihood == "lognormal") {
          ## Now standardise by Mean and add confidence intervals.
          total_sigma <- sqrt(log(1 + t_comp$actual_error^2))
          Mean <- log(t_comp$obs) - 0.5*(total_sigma^2)
          U_CI <- exp(Mean + 1.96 * total_sigma)
          L_CI <- exp(Mean - 1.96 * total_sigma)

          ## plot it
          if (missing(ylim)) {
            ylim <- c(min(L_CI), max(U_CI))
          }
          if (missing(xlim)) {
            xlim <- range(t_comp$year)
          }
          if (missing(xlab)) {
            xlab <- "Year"
          }
          if (missing(ylab)) {
            ylab <- this_report$type
          }

          plot(t_comp$year, t_comp$obs, main = report_label, pch = 1, xlab = xlab, ylab = ylab, ylim = ylim, xlim = xlim,...)
          segments(x0 = t_comp$year, x1 = t_comp$year, y0 = L_CI, y1 = U_CI, col = "black")
          points(t_comp$year, t_comp$fits, col = "red" , pch = 20, cex = 1.3)
        } else if (this_report$likelihood == "normal") {
          warning("Untested code\nAssumption: sigma = Standard error and so 95%CI = Obs +- 1.96 * sigma\n")
          total_sigma <- t_comp$obs * t_comp$actual_error
          U_CI <- t_comp$obs + 1.96 * total_sigma
          L_CI <- t_comp$obs - 1.96 * total_sigma

          ## plot it
          if (missing(ylim)) {
            ylim <- c(min(L_CI), max(U_CI))
          }
          if (missing(xlim)) {
            xlim <- range(t_comp$year)
          }
          if (missing(xlab)) {
            xlab = "Year"
          }
          if (missing(ylab)) {
            ylab = this_report$type
          }

          plot(t_comp$year, t_comp$obs, main = report_label, pch = 1, xlab = xlab, ylab = ylab, ylim = ylim, xlim = xlim,...)
          segments(x0 = t_comp$year, x1 = t_comp$year, y0 = L_CI, y1 = U_CI, col = "black")
          points(t_comp$year, t_comp$fits, col = "red" , pch = 20, cex = 1.3)
        } else if (this_report$likelihood == "multinomial") {
          cat("Plotting mean age.\n")
          Nassumed <- Ry <- Sy <- c()
          Obs <- as.matrix(t_comp$obs)
          Obs <- sweep(Obs, 1, apply(Obs, 1, sum), "/")
          Exp <- as.matrix(t_comp$fit)
          Exp <- sweep(Exp, 1, apply(Exp, 1, sum), "/")
          Nassumed <- c(Nassumed, apply(as.matrix(t_comp$actual_error), 1, mean))
          years <- as.numeric(dimnames(Obs)[[1]])

          ## pull out bins
          first_char  <- regexpr("\\[", dimnames(Obs)[[2]], fixed = F, useBytes = F)
          second_char <- regexpr("]", dimnames(Obs)[[2]], fixed = F, useBytes = F)
          aa <- as.numeric(substr(dimnames(Obs)[[2]], start = first_char + 1, stop = second_char - 1))
          My <- cbind(Obs = apply(Obs, 1, function(x) sum(aa * x)), Exp = apply(Exp, 1, function(x) sum(aa * x)))
          Ry <- c(Ry, My[, "Obs"] - My[, "Exp"])
          Sy <- c(Sy, sqrt(apply(Exp, 1, function(x) sum(x * aa^2)) - My[, "Exp"]^2))
          ses <- Sy/sqrt(Nassumed)
          Obs.bnds <- My[, "Obs"] + cbind(-2 * ses, 2 * ses)

          if (missing(ylim)) {
              ylim <- range(Obs.bnds)
          }
          if (missing(xlim)) {
              xlim <- range(years)
          }

          plot(years, My[, "Obs"], type = "n", ylab = "Mean Age/Length", xlab = "Years", xlim = xlim, ylim = ylim, las = 1,...)
          points(years, My[, "Obs"], pch = 1, col = "black")
          segments(years, Obs.bnds[, 1], years, Obs.bnds[, 2], col = "black")
          points(years, My[, "Exp"], col = "red" , pch = 20, cex = 1.3)
        } else {
          stop("unidentified likelihood")
        }
      } else if (type == "resid") {
        stop("This function cannot be used with type 'resid'.\n")
      }
    } else {
      return(t_comp)
    }
  } else {
    stop("This function cannot be used with multiple runs.\n")
  }
}


#' @return \code{NULL}
#'
#' @rdname plot.fits
#' @method plot.fits casal2TAB
#' @export
"plot.fits.casal2TAB" <- function(model, report_label="", type = "resid" ,plot.it = T, xlim, ylim, xlab, ylab, main, col, ...) {
  if (type != "resid") {
    stop("This function cannot be used with fit for class 'casal2TAB'. Use type = 'resid'")
  }

  ## check report label exists
  if (!report_label %in% names(model)) {
    stop(Paste("The report label '", report_label, "' was not found. The report labels available are: ", paste(names(model),collapse = ", ")))
  }

  ## get the report out
  this_report <- get(report_label, model)

  ## check that the report label is of type derived_quantity
  if (this_report$type != "observation") {
    stop(Paste("The report label '", report_label, "' is not an observation. Please check that the correct report label was specified."))
  }

  likelihoods_allowed  <- c("lognormal", "multinomial", "normal")
  observations_allowed <- c("biomass", "abundance", "proportions_at_age", "process_proportions_at_age", "proportions_at_length", "process_proportions_at_length", "process_removals_by_age", "process_removals_by_length")

  if (!this_report$likelihood %in% likelihoods_allowed) {
    stop(Paste("This function can be used with likelihoods " , paste(likelihoods_allowed, collapse = ", "), " only."))
  }
  if (!this_report$observation_type %in% observations_allowed) {
    stop(Paste("This function can be used with observation types " , paste(observations_allowed, collapse = ", "), " only."))
  }

  if (plot.it) {
    if (this_report$likelihood == "lognormal") {
      norm_ndx <- grepl(pattern = "normalised_residuals", x = names(this_report$values))

      if(!any(norm_ndx)) {
        stop("normalised_residuals were not found in the tabular report. Set @report for this observation: 'pearsons_residuals true'")
      }

      this_normal <- this_report$values[,norm_ndx]
      start_index <- as.numeric(regexpr(pattern = "\\[",text = names(this_normal))) + 1
      stop_index  <- as.numeric(regexpr(pattern = "\\]",text = names(this_normal))) - 1
      years  <- unique(as.numeric(substring(names(this_normal), start_index,last = stop_index)))
      end_nd <- as.numeric(regexpr(pattern = "\\.",text = names(this_normal))) - 1
      obs    <- unique(substring(colnames(this_normal),first = 0, last = end_nd))

      ## generate a boxplot of pearsons residuals by year
      boxplot(this_normal,ylim = c(-3,3),xlab = "Year", ylab = "Normalised residuals", main = obs,names = years)
      abline(h = c(2,-2), col = "red")
      abline(h = 0, lty = 0)
    } else if (this_report$likelihood == "normal"){
      norm_ndx  <- grepl(pattern = "normalised_residuals", x = names(this_report$values))
      pears_ndx <- grepl(pattern = "pearsons_residuals", x = names(this_report$values))

      if(!any(norm_ndx) & !any(pears_ndx)) {
        stop("normalised_residuals or pearson_residuals were not found in the tabular report. Set @report for this observation: 'normalised_residuals true' or 'pearsons_residuals true'")
      }

      if (any(norm_ndx)) {
        this_normal <- this_report$values[,norm_ndx]
        start_index <- as.numeric(regexpr(pattern = "\\[",text = names(this_normal))) + 1
        stop_index  <- as.numeric(regexpr(pattern = "\\]",text = names(this_normal))) - 1
        years  <- unique(as.numeric(substring(names(this_normal), start_index,last = stop_index)))
        end_nd <- as.numeric(regexpr(pattern = "\\.",text = names(this_normal))) - 1
        obs    <- unique(substring(colnames(this_normal),first = 0, last = end_nd))

        ## generate a boxplot of pearsons residuals by year
        boxplot(this_normal,ylim = c(-3,3),xlab = "Year", ylab = "Normalised residuals", main = "",names = years)
        abline(h = c(2,-2), col = "red")
        abline(h = 0, lty = 0)
      } else if (any(pears_ndx)) {
        this_normal <- this_report$values[,pears_ndx]
        start_index <- as.numeric(regexpr(pattern = "\\[",text = names(this_normal))) + 1
        stop_index  <- as.numeric(regexpr(pattern = "\\]",text = names(this_normal))) - 1
        years  <- unique(as.numeric(substring(names(this_normal), start_index,last = stop_index)))
        end_nd <- as.numeric(regexpr(pattern = "\\.",text = names(this_normal))) - 1
        obs    <- unique(substring(colnames(this_normal),first = 0, last = end_nd))

        ## generate a boxplot of pearsons residuals by year
        boxplot(this_normal,ylim = c(-3,3),xlab = "Year", ylab = "Pearsons residuals", main = obs,names = years)
        abline(h = c(2,-2), col = "red")
        abline(h = 0, lty = 0)
      }
    } else if (this_report$likelihood == "multinomial"){
      pear_ndx = grepl(pattern = "pearson_residuals", x = names(this_report$values))

      if(!any(pear_ndx)) {
        stop("pearson_residuals were not found in the tabular report. Set @report for this observation: 'pearsons_residuals true'")
      }

      this_pearson <- this_report$values[,pear_ndx]
      start_index  <- as.numeric(regexpr(pattern = "\\[",text = names(this_pearson))) + 1
      stop_index   <- as.numeric(regexpr(pattern = "\\]",text = names(this_pearson))) - 1
      years    <- unique(as.numeric(substring(names(this_pearson), start_index,last = stop_index)))
      start_nd <- as.numeric(regexpr(pattern = "\\]",text = colnames(this_pearson))) + 2
      bins     <- unique(as.numeric(substring(colnames(this_pearson),first = start_nd, last = nchar(colnames(this_pearson)) - 1)))
      end_nd   <- as.numeric(regexpr(pattern = "\\.",text = names(this_pearson))) - 1
      obs      <- unique(substring(colnames(this_pearson),first = 0, last = end_nd))

      ## generate a boxplot of pearsons residuals a panel for each year
      n_years <- length(years)

      if (n_years <= 4) {
          par(mfrow=c(1,n_years))
      } else if (n_years > 4 & n_years <= 8) {
          par(mfrow=c(2,ceiling (n_years/2)))
      } else if (n_years > 8 & n_years <= 12) {
          par(mfrow=c(3,ceiling (n_years/2)))
      } else {
          stop(Paste("There are ", n_years, " years of composition data. Set 'plot.it = FALSE' and use another function to plot these data."))
      }

      for (y in 1:n_years) {
        this_year <- this_pearson[,grepl(pattern = Paste("pearson_residuals\\[",years[y]), x = names(this_pearson))]
        boxplot(this_year,ylim = c(-3,3),xlab = "bins", ylab = "Pearsons residuals", main = years[y] ,names = bins)
        abline(h = c(2,-2), col = "red")
        abline(h = 0, lty = 0)
      }
    }
  } else {
    return(this_report$values)
  }
}

