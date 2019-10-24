# $Id: extract.parameters.at.bounds.R 4636 2012-03-19 23:48:49Z Dunn $
"extract.parameters.at.bounds"<-
function(file,path)
{
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  test<-casal.get.lines(file,starts.with = "The following parameters were estimated at or near a bound:")
  if(length(test)>0) {
    if(all(casal.regexpr("Start extracting output from here",file)<0)) stop(paste("No output data found in file '",filename,"'",sep=""))
    header <- casal.get.lines(file,clip.to="The following parameters were estimated at or near a bound:",clip.from="Start extracting output from here")
    header<-header[-1]
    max.lines<-0
    for(i in 1:length(header)) {
      if(length(casal.string.to.vector.of.words(header[i]))!=4) {
        max.lines<-i-1
        break
      }
    }
    header<-casal.string.to.vector.of.words(header[1:max.lines])
    temp.names<-header[seq(1,length(header)-3,by=4)]
    est<-header[seq(1,length(header)-3,by=4)+1]
    lb<-header[seq(1,length(header)-3,by=4)+2]
    ub<-header[seq(1,length(header)-3,by=4)+3]
    bounds<-data.frame("parameter"=temp.names,"estimate"=est,"lower.bound"=lb,"upper.bound"=ub,stringsAsFactors=FALSE)
  } else {
    bounds<-NULL
  }
  return(bounds)
}
