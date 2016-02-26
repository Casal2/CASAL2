#' Utility extract function
#'
#' @author Dan Fu
#'
"string.to.vector.of.words" <-
function(string)
{
  temp <- unpaste(string, sep = " ")
  return(temp[temp != ""])
}

