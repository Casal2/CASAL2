# $Id: extract.mulitiple.objective.functions.R 1828 2007-11-30 01:16:07Z adunn $
"extract.multiple.objective.functions" <-
function(file,path="")
{
# Extracts multiple objective functions from casal output
# (e.g., output from "casal -i pars -r" where pars contains multiple
# parameter vectors)
# and returns them in a matrix in which the rows are components of
# the objective function and there is one column per objective function
#
  string.match <- function(x, y) return((1:length(y))[casal.regexpr(x, y) > 0])
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  start.lines <- string.match("Start extracting output from here", file)
  n.ojectives <- length(start.lines)
  end.lines <- c(start.lines[-1] - 1, length(file))
  if(length(string.match("Fits :", file)) > 0)
    end.string <- "Fits :"
  else if(length(string.match("Output quantities start here", file)) > 0)
    end.string <- "Output quantities start here"
  else stop("Don't know what end string to use for objective components")
  for(j in 1:n.ojectives) {
    subfile <- file[(start.lines[j]):(end.lines[j])]
    subfile <- casal.get.lines(subfile, clip.to.match = "Components :", clip.from.match = end.string)
    if(j == 1) {
      n.components <- length(subfile)
      out <- matrix(0, n.components, n.ojectives)
      rownam <- rep("", n.components)
      for(i in 1:n.components) {
        linei <- casal.string.to.vector.of.words(subfile[i])
        rownam[i] <- linei[1]
        out[i, j] <- as.numeric(linei[2])
      }
    } else {
      for(i in 1:n.components)
        out[i, j] <- as.numeric(casal.string.to.vector.of.words(subfile[i])[2])
    }
  }
  dimnames(out) <- list(rownam, NULL)
  return(out)
}
