#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"pos" <-
function(vector, x) {
  min((1:length(vector))[vector == x])
}

