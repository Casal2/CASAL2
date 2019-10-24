"plot.age.misclassification"<-
function(data,min.age=2,min.prob=0.01,xlim,xlab="True age",ylab="Observed age")
{
  index<-(1:ncol(data))+min.age-1
  if(missing(xlim)) xlim<-range(c(index-1,index+1))
  plot(index,index,type="n",xlab=xlab,ylab=ylab,xlim=xlim,ylim=xlim)
  ans<-split(data,row(data))
  for(i in 1:length(ans)) {
    x<-i+min.age-1
    den<-as.vector(unlist(ans[i]))
    index<-1:length(den)
    index<-ifelse(den<=min.prob,NA,index)
    x<-rep(x,length(den))+den/(max(den)/0.50)
    x<-c(floor(x[1]),rep(x[!is.na(index)],each=2),floor(x[1]),floor(x[1]))
    y<-index[!is.na(index)]+min.age-0.5
    y<-c(rep(c(min(y)-1,y),each=2),min(y)-1)
    lines(x,y)
  }
  invisible()
}

