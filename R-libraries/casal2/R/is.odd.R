#' Utility extract function
#'
#' @author Dan Fu
#'
is.odd <- function(x) {
    ifelse((x/2)==(x%/%2),F,T)
}
