# $Id: extract.parameter.bounds.R 1828 2007-11-30 01:16:07Z adunn $
"extract.parameter.bounds"<-
function(file,path="",output.type="matrix")
{
# Extracts parameter bounds for all estimated parameters in an estimation.csl
# file, returning these either as an n x 2 matrix (if outform='matrix')
# of a list containing one component for each parameter
# (or parameter vector),  where each component is a list with components
# 'lowerbound' and 'upperbound'.
  unlapply <- function(lst, fun, ...)  unlist(lapply(lst, fun, ...))
  test<-pmatch(output.type,c("matrix", "list"))
  if(is.na(output.type)) stop("Illegal value for argument output.type. Use either matrix or list.")
  if(missing(path)) path<-""
  cmds <- extract.csl.file(file=file,path=path)
  cmds <- get.csl.command(cmds, "estimate")
  outlist <- list()
  for(i in 1:length(cmds)) {
    temp <- cmds[[i]]
    outlist[[temp$parameter]] <- list(lowerbound = as.numeric(temp$"lower_bound"), upperbound = as.numeric(temp$"upper_bound"))
    res<-outlist
  }
  if(test==1) {
    parlen <- unlapply(outlist, function(x) length(x$lowerbound))
    temp <- unlapply(outlist, function(x) if(length(x$lowerbound) == 1) "" else paste(".", 1:length(x$lowerbound), sep = ""))
    rownams <- paste(rep(names(outlist), parlen), temp, sep = "")
    outmat <- matrix(0, length(rownams), 2, dimnames = list(rownams,c("lowerbound", "upperbound")))
    outmat[, "lowerbound"] <- unlist(lapply(outlist,function(x) x$lowerbound))
    outmat[, "upperbound"] <- unlist(lapply(outlist,function(x) x$upperbound))
    res<-outmat
  }
  return(res)
}
