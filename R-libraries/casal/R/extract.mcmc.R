# $Id: extract.mcmc.R 1842 2007-12-12 03:35:50Z adunn $
"extract.mcmc"<-
function(samples,objectives,path="",burn.in=0)
{
  set.class <- function(object,new.class){
    # use in the form
    #  object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class,attributes(object)$class[attributes(object)$class!=new.class])
    object
  }
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=samples)
  lines <- casal.convert.to.lines(filename)
  is.samples<-TRUE
  if(lines[1]=="CASAL (C++ algorithmic stock assessment laboratory)") is.samples<-FALSE
  if(is.samples) {
    header <- lines[1]
    header.split <- casal.string.to.vector.of.words(header)
    colnames <- c()
    i <- 1
    while (i < length(header.split)) {
      if (casal.is.whole.number(header.split[i + 1])) {
        n.reps <- as.numeric(header.split[i + 1])
        colnames <- c(colnames, paste(rep(header.split[i], n.reps), 1:n.reps, sep = "."))
        i <- i + 2
      }
      else {
        colnames <- c(colnames, header.split[i])
        i <- i + 1
      }
    }
    if (i == length(header.split)) {
      colnames <- c(colnames, header.split[length(header.split)])
    }
    result <- read.table(filename, skip = 1)
    names(result) <- colnames
  } else {
    result <- casal.make.table(casal.get.lines(lines, clip.to.match = "Quantity values"))
  }
  if(!missing(objectives)) {
    obj.filename<-casal.make.filename(path=path,file=objectives)
    objectives.table <- casal.make.table(casal.get.lines(casal.convert.to.lines(obj.filename), clip.to.match = "Main table"))
    rownames <- objectives.table$sample
    dimnames(result)[[1]] <- rownames
    result$"neg_ln_posterior" <- objectives.table$posterior
    result <- result[objectives.table$sample > burn.in,  ]
  } else {
    cat("Warning: No objectives file supplied. Assuming burn.in is the rows of the input file to remove.\n")
    if(burn.in < nrow(result))
      result <- result[(1:nrow(result)) > burn.in, ]
    else cat("Warning: burn.in is greater than the rows of the input file. Ignoring the burn.in option\n")
  }
  result<-as.matrix(result)
  result<-set.class(result,"casalMCMC")
  return(result)
}
