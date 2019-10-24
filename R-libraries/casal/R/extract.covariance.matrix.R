# $Id: extract.covariance.matrix.R 1828 2007-11-30 01:16:07Z adunn $
"extract.covariance.matrix"<-
function(file, path="")
{
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  lines<-casal.convert.to.lines(filename)
  if(all(casal.regexpr("Covariance matrix",lines)<0)) stop("No covariance matrix was found. Did you set @print.covariance=True in your output.csl file?")
  lines<-casal.get.lines(lines,clip.to.match="Covariance matrix")
  columns <- casal.string.to.vector.of.words(lines[1])
  if(length(lines) < 2) return(NA)
  data <- matrix(0, length(columns), length(columns))
  for(i in 1:nrow(data))
    data[i,] <- casal.string.to.vector.of.numbers(lines[i])
  data<-data[1:ncol(data),]
  return(data)
}
