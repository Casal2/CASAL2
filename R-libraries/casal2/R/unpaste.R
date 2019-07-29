#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"unpaste" <-
function(string, sep)
{
    return(unlist(strsplit(string, split = sep)))

}