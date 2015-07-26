#' Utility extract function
#'
#' @author Dan Fu
#'
"string.to.vector.of.numbers" <-
function(string)
{
  as.numeric(string.to.vector.of.words(string))
}

