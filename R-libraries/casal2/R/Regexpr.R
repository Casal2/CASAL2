#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"Regexpr" <-
function(x, y, fixed = T) {
  return(regexpr(x, y, fixed = fixed))
}

