"extract.growth.transition"<-
function(file, path="")
{
  if((!missing(path) | path!="") & (substring(path,nchar(path)-1)!="\\" || substring(path,nchar(path)-1)!="/")) path<-paste(path,"\\",sep="")
  lines<-casal.convert.to.lines(paste(path, file, sep = ""))
  index<-casal.regexpr("Transition matrix",lines)>0
  index<-(1:length(lines))[index]
  if(length(index)>1) {
    data<-vector("list",length(index))
    names(data)<-substring(lines[index],3)
  }
  for(i in 1:length(index)) {
    temp<-casal.get.lines(lines,from=index[i]+1)
    columns <- casal.string.to.vector.of.words(temp[1])
    temp<-temp[1:length(columns)]
    if(length(temp) < 2) return(NA)
    tm <- matrix(0, length(temp), length(columns))
    for(i in 1:length(temp))
      tm[i,] <- casal.string.to.vector.of.numbers(temp[i])
    dimnames(tm)<-list(1:nrow(tm),1:ncol(tm))
    if(length(index)>1) data[[i]]<-tm
    else data<-tm
  }
  return(data)
}
