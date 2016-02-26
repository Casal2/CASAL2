#' Utility extract function
#'
#' @author Dan Fu
#'
"remove.first.words" <-
function(string, words = 1)
{
  paste(unpaste(string, sep = " ")[ - (1:words)], collapse = " ")
}

