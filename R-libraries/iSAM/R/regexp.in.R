#' Utility extract function
#'
#' @author Dan Fu
#'
"regexp.in" <-
function(vector, regexp)
{
  if(length(vector) == 0)
    return(F)
  any(Regexpr(regexp, vector) > 0)
}

