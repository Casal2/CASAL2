
#' @title strip_comments
#'
#' @description
#' A utility function for stripping out comments that is
#' line start with #
#' or a wrapped in /* */
#'
#' @author Craig Marsh
#' @param file a file read in by scan
#' @return a file that has removed lines starting with # or betwee '/*' '*/'
#' @rdname strip_comments
#' @export
strip_comments <- function(file) {
  file <- file[substring(file, 1, 1) != "#"]
  index1 <- ifelse(substring(file, 1, 2) == "/*", 1:length(file),0)
  index2 <- ifelse(substring(file, 1, 2) == "*/", 1:length(file),0)
  index1 <- index1[index1 != 0]
  index2 <- index2[index2 != 0]
  if (length(index1) != length(index2))
    stop(paste("Error in the file ", filename, ". Cannot find a matching '/*' or '*/'",
               sep = ""))
  if (length(index1) > 0 || length(index2) > 0) {
    index <- unlist(apply(cbind(index1, index2), 1, function(x) seq(x[1],
                                                                    x[2])))
    file <- file[!1:length(file) %in% index]
  }
  file <- ifelse(regexpr("#", file) > 0, substring(file, 1, regexpr("#", file) - 1), file)
  
  file <- file[file != ""]
  ## remove lines that have /* */ partway through them
  ## probably a little inefficient
  for(i in 1:length(file)) {
    index1 = regexpr(pattern = "//*", file[i])
    first_section = substring(file[i], first = 1, last = index1 - 1)
    second_section = substring(file[i], first = index1 +2)
    index2 = regexpr(pattern = "*/", second_section)
    file[i] = paste0(first_section, substring(second_section, first = index2 + 1))
  }
  
  return(file)
}
