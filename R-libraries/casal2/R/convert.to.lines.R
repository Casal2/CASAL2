#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"convert.to.lines" <-
function(filename) {
  scan(filename, what = "", sep = "\n")
}

