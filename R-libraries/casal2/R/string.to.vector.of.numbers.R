#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"string.to.vector.of.numbers" <-
function(string)
{
  as.numeric(string.to.vector.of.words(string))
}

