# $Id: extract.quantities.R 4827 2012-11-07 20:36:52Z Dunn $
"extract.quantities"<-
function(file,path)
{
  quantities <- list()
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  if(all(casal.regexpr("Start extracting output from here",file)<0)) {
    cat(paste("No output data found in file '",filename,"'",sep=""))
    return(NA)
  }
  file <- casal.get.lines(file, clip.to = "Output quantities start here")
  file <- casal.get.lines(file, clip.from = "Output quantities finish here")
  while(1) {
    if(length(file) == 0)
      break
    header <- casal.remove.first.words(casal.get.lines(file,contains = "\\*")[1], 1)
    file <- casal.get.lines(file, clip.to.match = "\\*")
    temp <- casal.get.lines(file, clip.from.match = "\\*")
    if(length(temp) == 1 && length(casal.string.to.vector.of.words(temp[1])) == 1) {
      quantities[[header]] <- casal.string.to.vector.of.numbers(temp[1])[1]
    } else {
      quantities[[header]] <- casal.make.list(temp)
    }
    if(!casal.regexp.in(file, "\\*"))
      break
  }
  return(quantities)
}
