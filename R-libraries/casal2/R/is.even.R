#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
is.even = function(x) {
  ifelse((x / 2) == (x %/% 2), T, F)
}
