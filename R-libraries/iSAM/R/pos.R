#' Utility extract function
#'
#' @author Dan Fu
#'
"pos" <-
function(vector, x)
{
  min((1:length(vector))[vector == x])
}

