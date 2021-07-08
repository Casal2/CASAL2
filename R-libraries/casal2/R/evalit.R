#' Utility plot function
#'
#' @author Craig Marsh
#' @keywords internal
#'
evalit <- function(x) {
  result <- eval(parse(text = x))
  return(result)
}
