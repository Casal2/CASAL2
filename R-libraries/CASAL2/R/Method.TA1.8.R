#' @title Method.TA1.8
#' @description This function is useful for deciding on the data weights of one or more at-age or at-length data sets with assumed lognormal error structure in a stock assessment.
#' Originally written in Chris Francis's DataWeighting Package, this has been copied over and modified so that users can use this 
#' functionality with Casal2 models/output.

#' @author Chris Francis
#' @param model Casal2 output that is the result of a -r, -e run.
#' @param Observation_label Label of the observation you want to apply the weighting too.
#' @param plot.it If TRUE, plot the index and the smoothed fit. Otherwise, return a dataframe of the year, index, smoothed fitted value, and cv)
#' @param ylim y-axis limits for the illustrative plot
#' @param xlim x-axis limits for the illustrative plot
#'
#' @return Outputs a stage-2 process-error c.v. for the data set. Returns a 2-vector containing the process-error c.v. assumed or estimated in stage 1, and that estimated using TA1.10 for stage 2
#'
#' @note Method TA1.8 is described in Appendix A of the following paper Francis, R.I.C.C. (2011). Data weighting in statistical fisheries stock assessment models. 
#' Canadian Journal of Fisheries and Aquatic Sciences 68: 1124-1138. (With corrections to the equation in Francis R.I.C.C. (2011) Corrigendum: Data weighting in statistical fisheries stock assessment models. 
#' @export

Method.TA1.10 = function (model, Observation_label, pe.cv.stg1, plot.it = F, xlim = NULL, ylim = NULL, trial.pe.cvs = seq(0, 2, 0.05)) {
  ############################
  ## Validate input parameters
  ############################
  multiple = FALSE;
  if (length(Observation_label) > 1)
    multiple = TRUE;
  for (i in 1:length(Observation_label[i]) {
    ## check report label exists
    if (!Observation_label[i] %in% names(model))
      stop(Paste("In model the report label '", Observation_label[i], "' could not be found. The report labels available are ", paste(names(model),collapse = ", ")))
    ## get the report out
    this_report = get(Observation_label[i], model)
    ## check that the Observation_label is of type observation
    if (this_report$'1'$type != "observation") {
      stop(Paste("The report label ", Observation_label[i], " in model is not a observation report, it is a ",this_report$'1'$type," report, plz Check you have specified the correct Observation_label"))     
    }
    if (length(this_report) > 1) {
        stop(Paste("Found: ",length(this_report) ," reports in under the observation_label, ", observation_label[i],", ususally this is caused by a multiline parameter run in casal2 -i or a profile run casal2 -p. This function will not work for those run types"))
    } 
  }
  ###########################
  ## Execute the function.
  ###########################
  this_report = get(Observation_label, this_report$'1')
  
  ## reformat the data so that we can use Chris's code.
  if (plot.it & multiple) 
      stop("Can't produce diagnostic plot for multiple data sets")
  
  Ndat <- length(Observation_label)
  Nassumed <- Ry <- Sy <- c();
  for (i in 1:Ndat) {
      thisfit <- fit[[i]]
      if (length(thisfit$obs) == 0) 
          stop("Can't find observations")
      Obs <- as.matrix(thisfit$obs)
      Obs <- sweep(Obs, 1, apply(Obs, 1, sum), "/")
      Exp <- as.matrix(thisfit$fit)
      Exp <- sweep(Exp, 1, apply(Exp, 1, sum), "/")
      Nassumed <- c(Nassumed, apply(as.matrix(thisfit$error.value), 
          1, mean))
      years <- as.numeric(dimnames(Obs)[[1]])
      aa <- as.numeric(substring(dimnames(Obs)[[2]], 2, nchar(dimnames(Obs)[[2]])))
      My <- cbind(Obs = apply(Obs, 1, function(x) sum(aa * 
          x)), Exp = apply(Exp, 1, function(x) sum(aa * x)))
      Ry <- c(Ry, My[, "Obs"] - My[, "Exp"])
      Sy <- c(Sy, sqrt(apply(Exp, 1, function(x) sum(x * aa^2)) - 
          My[, "Exp"]^2))
  }
  wj <- 1/var(Ry * sqrt(Nassumed)/Sy, na.rm = T)
  if (plotit) {
      ses <- Sy/sqrt(Nassumed)
      Obs.bnds <- My[, "Obs"] + cbind(-2 * ses, 2 * ses)
      if (is.null(ylim)) 
          ylim <- range(Obs.bnds)
      if (is.null(xlim)) 
          xlim <- range(years)
      plot(years, My[, "Obs"], type = "n", ylab = "", xlab = "", 
          xlim = xlim, ylim = ylim, las = 1)
      points(years, My[, "Obs"], pch = "x", col = 3)
      segments(years, Obs.bnds[, 1], years, Obs.bnds[, 2], 
          col = 3)
      lines(years, My[, "Exp"], col = 2)
  }
  wj
}
