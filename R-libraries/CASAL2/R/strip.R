#' Utility for extract function
#'
#' @author Craig Marsh
#'
strip = function(x) {
      tmp <- unlist(strsplit(x, "\t"))
      tmp <- unlist(strsplit(tmp, " "))
      return(as.vector(paste(tmp, collapse = " ")))
} 
