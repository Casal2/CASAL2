#' Utility for extract function
#'
#' @author Craig Marsh
#' @keywords internal
#'
strip = function(x) {
      tmp <- unlist(strsplit(x, "\t"))
      tmp <- unlist(strsplit(tmp, " "))
      return(as.vector(paste(tmp, collapse = " ")))
} 
