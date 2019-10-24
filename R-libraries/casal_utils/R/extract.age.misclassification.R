"extract.age.misclassification"<-
function(file, path="")
{
  if((!missing(path) | path!="") & (substring(path,nchar(path)-1)!="\\" || substring(path,nchar(path)-1)!="/")) path<-paste(path,"\\",sep="")
  lines<-casal.convert.to.lines(paste(path, file, sep = ""))
  lines<-casal.get.lines(lines,clip.to.match="Misclassification matrix")
  columns <- casal.string.to.vector.of.words(lines[1])
  lines<-lines[1:length(columns)]
  if(length(lines) < 2) return(NA)
  data <- matrix(0, length(lines), length(columns))
  for(i in 1:nrow(data))
    data[i,] <- casal.string.to.vector.of.numbers(lines[i])
  dimnames(data)<-list(1:nrow(data),1:ncol(data))
  data
}
