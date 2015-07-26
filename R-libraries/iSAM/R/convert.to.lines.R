#' Utility extract function
#'
#' @author Dan Fu
#'
"convert.to.lines" <-
function(filename) {
  scan(filename, what = "", sep = "\n")
}

