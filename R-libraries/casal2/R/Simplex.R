#' restoresimplex
#' takes an unconstrained vector of length K - 1, and calculates a unit vector of length K
#' @param yk vector of length K - 1 takes unconstrained values
#' @return xk unit vector of length K
#' @export
#'
restoresimplex <- function(yk) {
  K <- length(yk) + 1
  zk <- plogis(yk + log(1 / (K - 1:(K - 1))))
  xk <- vector()
  xk[1] <- zk[1]
  for (k in 2:(K - 1)) {
    xk[k] <- (1 - sum(xk[1:(k - 1)])) * zk[k]
  }
  xk[K] <- 1 - sum(xk)
  return(xk)
}

#' simplex
#' takes a unit vector of length K and converts to unconstrained K - 1 vector
#' @param xk vector of length K - 1 takes unconstrained values
#' @param sum_to_one whether to rescale xk so it sums to one
#' @return vector of length K - 1 unconstrained values
#' @export
#'
simplex <- function(xk, sum_to_one = TRUE) {
  zk <- vector()
  if (!sum_to_one) {
    xk <- xk / sum(xk)
  } else {
    if (abs(sum(xk) - 1.0) > 0.001) {
      stop("xk needs to sum = 1, otherwise speify sum_to_one = TRUE")
    }
  }
  K <- length(xk)
  zk[1] <- xk[1] / (1)
  for (k in 2:(K - 1)) {
    zk[k] <- xk[k] / (1 - sum(xk[1:(k - 1)]))
  }
  yk <- qlogis(zk) - log(1 / (K - 1:(K - 1)))
  return(yk)
}
#' jacobiansimplex
#' takes an unconstrained vector of length K - 1, and calculates the determinate of the jacobian, not this relates to density. You will need to log it for log-likelihood contributions
#' @param yk vector of length K - 1 takes unconstrained values
#' @return xk unit vector of length K
#' @details if you want the value that casal2 returns you need to log and take the negative, this function evaluates change in variable based on the denisty, but Casal2's objective function is with respect to negative log-likelihood.
#' @export
#'
jacobiansimplex <- function(yk) {
  xk <- restoresimplex(yk)
  K <- length(yk) + 1
  zk <- plogis(yk + log(1 / (K - 1:(K - 1))))
  jacobian <- zk[1] * (1 - zk[1])
  # cat(zk[1], " ", 0, " ", jacobian, "\n")
  for (k in 2:(K - 1)) {
    jacobian <- jacobian + zk[k] * (1 - zk[k]) * (1 - sum(xk[1:(k - 1)]))
    # cat(zk[k], " ", sum(xk[1:(k - 1)]), " ", jacobian, "\n")
  }
  return(jacobian)
}
