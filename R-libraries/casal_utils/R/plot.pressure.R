# Plot OBS/fitted
"plot.pressure"<-
function(model,label="",xlim,ylim,plot.type=dplot,...){
  UseMethod("plot.pressure",model)
}

"plot.pressure.casal.mpd"<-
function(model,label="",xlim,ylim,...)
{
  data<-model[["quantities"]]$"fishing_pressures"
  if(length(data)>2) {
    year<-data$year
    data<-as.vector(unlist(data[as.vector(casal.regexpr(label,names(data))>0)]))
  } else {
    year<-data$year
    data<-data[[1]]
  }
  if(missing(xlim)) xlim<-range(year)
  if(missing(ylim)) ylim<-range(c(0,data))
  plot(year,data,xlim=xlim,ylim=ylim,type="n",xlab="Year",ylab="Fishing pressure")
  lines(year,data,...)
  invisible()
}

"plot.pressure.casal.mcmc"<-
function(model,label="",xlim,ylim,plot.type=dplot,...)
{
  if(label!="") label<-paste("fishing_pressure\\[",label,"\\]",sep="")
  else label<-"fishing_pressure"
  index<-as.vector((casal.regexpr(label,dimnames(model)[[2]])>0))
  YCS<-model[,index]
  year<-as.numeric(substring(dimnames(YCS)[[2]],nchar(dimnames(YCS)[[2]])-4,nchar(dimnames(YCS)[[2]])-1))
  xlab<-"Year"
  if(missing(xlim)) xlim<-range(year)
  if(missing(ylim)) ylim<-range(c(0,YCS))
  centers<-plot.type(split(YCS,col(YCS)),name=F,main="",xlab=xlab,ylab="Fishing pressure",ylim=ylim,...)
  index<-ifelse(year %% 5 ==0,T,F)
  axis(side=1,at=centers[index],labels=as.character(year)[index])
  axis(side=1,at=centers,labels=F,tck=ifelse(is.na(par("tck")),-0.02,0.5*par("tck")))
  invisible(centers)
}
