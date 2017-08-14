#' @title CV.for.CPUE
#' @description This function is useful for deciding on a c.v. to be used with a CPUE series in a stock assessment model.
#' Originally written in Chris Francis's DataWeighting Package, this has been copied over and modified so that users can use this 
#' functionality with Casal2 models/output.

#' @author Chris Francis
#' @param year vector of years with CPUE indices
#' @param cpue CPUE indices
#' @param f degree of lowess smoothing (0 = no smoothing, 1 = maximum smoothing)
#' @param plot.it If TRUE, plot the index and the smoothed fit. Otherwise, return a dataframe of the year, index, smoothed fitted value, and cv)
#' @return The function either plots the CPUE, together with a lowess line fitted to it, and returns the c.v. of the residuals to the fit. Or returns a dataframe of the lowess line fits and assocated c.v.s for each point.
#' @noteThe idea is to fit a lowess line of "appropriate" smoothness to the CPUE data, and use the c.v. of the residuals from that fit in the stock assessment model. 
#' This is equivalent to saying that we expect the stock assessment model to fit these data as well as the smoother. You judge what is "appropriate" by visual examination of lines with different degrees of smoothing. 
#' This approach is recommended on p. 1132 of the following paper: Francis, R.I.C.C. (2011). Data weighting in statistical fisheries stock assessment models. Canadian Journal of Fisheries and Aquatic Sciences 68: 1124-1138.
#' @export
CV.for.CPUE = function (year, cpue, f, plot.it = TRUE) {
    CV.for.lognormal.error <- function(yobs, yfit) {
        lognormal.neg.log.likl <- function(cv, obs, fit) {
            sd <- sqrt(log(1 + cv^2))
            neg.log.likl <- sum(log(sd) + 0.5 * (0.5 * sd + log(obs/fit)/sd)^2)
            neg.log.likl
        }
        est.cv <- optimize(lognormal.neg.log.likl, c(0.01, 0.9), 
            obs = yobs, fit = yfit)$minimum
        return(est.cv)
    }
    lowess.fit <- lowess(year, cpue, f = f)
    CV <- CV.for.lognormal.error(cpue, lowess.fit$y)
    if (plot.it) {
        plot(year, cpue, pch = "x", ylim = c(0, 1.05 * max(c(cpue, 
            lowess.fit$y))), yaxs = "i", xlab = "", ylab = "")
        lines(lowess.fit)
        return(CV)
    }
    else {
        return(data.frame(year = year, cpue = cpue, lowess = lowess.fit$y, 
            cv = rep(CV, length(year))))
    }
}
