#' @title Method.TA1.10
#' @description This function is useful for deciding on the data weights of one or more at-age or at-length data sets with assumed lognormal error structure in a stock assessment.
#' Originally written in Chris Francis's DataWeighting Package, this has been copied over and modified so that users can use this 
#' functionality with Casal2 models/output.

#' @author Chris Francis
#' @param model Casal2 output that is the result of a -r, -e run.
#' @param Observation_label Label of the observation you want to apply the weighting too.
#' @param pe.cv.stg1 process-error c.v. assumed or estimated in stage 1 ("This can be extracted from the report")
#' @param plot.it If TRUE, plot the index and the smoothed fit. Otherwise, return a dataframe of the year, index, smoothed fitted value, and cv)
#' @param ylim y-axis limits for the illustrative plot
#' @param xlim x-axis limits for the illustrative plot
#' @param trial.pe.cvs  vector of trial stage-2 process-error c.v.s (the default value will usually be OK; if not, an error message is output)
#'
#' @return Outputs a stage-2 process-error c.v. for the data set. Returns a 2-vector containing the process-error c.v. assumed or estimated in stage 1, and that estimated using TA1.10 for stage 2
#'
#' @note Method TA1.10 is described in Appendix A of the following paper Francis, R.I.C.C. (2011). Data weighting in statistical fisheries stock assessment models. 
#' Canadian Journal of Fisheries and Aquatic Sciences 68: 1124-1138. (With corrections to the equation in Francis R.I.C.C. (2011) Corrigendum: Data weighting in statistical fisheries stock assessment models. 
#' @export

Method.TA1.10 = function (model, Observation_label, pe.cv.stg1, plotit = F, xlim = NULL, ylim = NULL, trial.pe.cvs = seq(0, 2, 0.05)) {
    Obs <- as.matrix(fit$obs)
    Obs <- sweep(Obs, 1, apply(Obs, 1, sum), "/")
    Exp <- as.matrix(fit$fit)
    Exp <- sweep(Exp, 1, apply(Exp, 1, sum), "/")
    totcv <- as.matrix(fit$error.value)
    if (any(totcv < pe.cv.stg1)) 
        stop("pe.cv.stg1 too large; larger than some total cvs")
    obscv <- sqrt(totcv^2 - pe.cv.stg1^2)
    years <- as.numeric(dimnames(Obs)[[1]])
    ages <- as.numeric(substring(dimnames(Obs)[[2]], 2, nchar(dimnames(Obs)[[2]])))
    obsmn <- apply(Obs, 1, function(x) sum(ages * x))
    expmn <- apply(Exp, 1, function(x) sum(ages * x))
    resid <- obsmn - expmn
    varstat <- rep(0, len(trial.pe.cvs))
    for (i in 1:len(trial.pe.cvs)) {
        totcv <- sqrt(obscv^2 + trial.pe.cvs[i]^2)
        S2 <- apply(Exp * totcv, 1, function(x) sum((ages * x)^2))
        S3 <- apply(Exp * totcv, 1, function(x) sum(x^2))
        S4 <- apply(Exp * totcv, 1, function(x) sum(ages * (x^2)))
        varstat[i] <- var(resid/sqrt(S2 + (expmn^2) * S3 - 2 * 
            expmn * S4))
    }
    if (all(varstat < 1)) {
        if (min(trial.pe.cvs) == 0) 
            pe.cv.stg2 <- 0
        else stop("trial.pe.cvs too high - try lower values")
    }
    else if (all(varstat > 1)) 
        stop("trial.pe.cvs too low - try higher values")
    else pe.cv.stg2 <- approx(varstat, trial.pe.cvs, 1)$y
    if (plotit) {
        mnsdres <- rep(0, 2)
        totcv <- sqrt(obscv^2 + pe.cv.stg1^2)
        S2 <- apply(Exp * totcv, 1, function(x) sum((ages * x)^2))
        S3 <- apply(Exp * totcv, 1, function(x) sum(x^2))
        S4 <- apply(Exp * totcv, 1, function(x) sum(ages * (x^2)))
        sdres <- sqrt(S2 + (expmn^2) * S3 - 2 * expmn * S4)
        mnsdres[i] <- mean(sdres)
        bnds <- cbind(lo = obsmn - 2 * sdres, hi = obsmn + 2 * 
            sdres)
        if (is.null(xlim)) 
            xlim <- range(years)
        if (is.null(ylim)) 
            ylim <- range(c(obsmn, bnds))
        plot(years, obsmn, type = "n", xlim = xlim, ylim = ylim, 
            xlab = "", ylab = "")
        points(years, obsmn, col = 3)
        segments(years, bnds[, "lo"], years, bnds[, "hi"], col = 3)
        lines(years, expmn, col = 2)
    }
    c(stg1.cv = pe.cv.stg1, stg2.cv = pe.cv.stg2)
}
