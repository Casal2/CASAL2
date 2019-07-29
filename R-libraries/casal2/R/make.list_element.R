#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"make.list_element" <-
function(lines)
{
   data = string.to.vector.of.words(remove.first.words(lines[1],1))
   if(is.all.numeric(data))
       data = as.numeric(data)
   data
}

