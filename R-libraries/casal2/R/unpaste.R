#' Utility extract function
#'
#' @author Dan Fu
#'
"unpaste" <-
function(string, sep)
{
    return(unlist(strsplit(string, split = sep)))

}