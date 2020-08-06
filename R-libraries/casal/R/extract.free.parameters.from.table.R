# $Id: extract.free.parameters.from.table.R 1828 2007-11-30 01:16:07Z adunn $
"extract.free.parameters.from.table"<-
function(file,path="")
{
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  header <- scan(filename, what = "", sep = "\n")[1]
  header.split <- casal.string.to.vector.of.words(header)
  colnames <- c()
  i <- 1
  while(i < length(header.split)) {
    if(casal.is.whole.number(header.split[i + 1])) {
      n.reps <- as.numeric(header.split[i + 1])
      colnames <- c(colnames, paste(rep(header.split[i],
        n.reps), 1:n.reps, sep = "."))
      i <- i + 2
    }
    else {
      colnames <- c(colnames, header.split[i])
      i <- i + 1
    }
  }
  if(i == length(header.split)) {
    colnames <- c(colnames, header.split[length(header.split)])
  }
  free.parameters <- read.table(filename, skip = 1)
  names(free.parameters) <- colnames
  return(free.parameters)
}
