#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
make.data.frame <- function(lines)
{
  columns <- string.to.vector.of.words(lines[1])
  data <- data.frame(matrix(nrow=length(lines)-1,ncol=length(columns)))
  for(i in 2:length(lines)) {
    line = string.to.vector.of.words(lines[i])
    if (length(line) != length(columns)) {
        stop(paste(lines[i],"is not the same length as",lines[1]))
    }
    
    data[i-1,] <- line
  }
  colnames(data) <- columns
  for (i in 1:ncol(data)) {
      if (is.all.numeric(data[,i])) {
          data[,i]=as.numeric(data[,i])
      }
  }
  data
}

