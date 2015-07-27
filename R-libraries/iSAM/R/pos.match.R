#' Utility extract function
#'
#' @author Dan Fu
#'
"pos.match" <-
function(vector, regexp)
{
  min((1:length(vector))[Regexpr(regexp, vector) > 0])
}

