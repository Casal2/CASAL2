#' Utility extract function
#'
#' @author Craig marsh
#' @param n number of samples to draw
#' @param mu mean in normal space
#' @param cv cv in normal space
#' @return n randomly generated values from a lognormal distribution

Rlnorm = function(n,mu,cv) {
  log_sigma = sqrt(log(cv*cv + 1.0));
  log_mean = log(mu) - (log_sigma * log_sigma) / 2.0;

  draws = rnorm(n, log_mean,log_sigma)
  return(exp(draws));
}
