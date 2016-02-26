#' Utility extract function
#'
#' @author Dan Fu
#'
is.even = function(x) {
    ifelse((x/2)==(x%/%2),T,F)
}
