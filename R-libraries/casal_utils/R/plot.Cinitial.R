# Plot OBS/fitted
"plot.Cinitial"<-
function(model,label="",ages,xlim,ylim,lty=c(1,8),plot.type=dplot) {
  UseMethod("plot.Cinitial",model)
}

"plot.Cinitial.casal.mpd"<-
function(model,label="",ages,xlim,ylim,lty=c(1,8))
{
  data<-model$quantities$"Ogive parameter values"
  if(label!="") Names<-paste("initialization\\[",label,"\\]",sep="")
  else Names<-"initialization.Cinitial"
  index<-as.vector(casal.regexpr(Names,names(data))>0)
  data<-data[index]
  if(any(regexpr("male",names(data))>0)) { # Cinitial is by sex
    if(missing(ages)) {
      ages<-1:length(data[[1]])
      xlab<-"Index"
    } else xlab<-"Age"
    if(missing(xlim)) xlim<-range(ages)
    if(missing(ylim)) ylim<-range(unlist(data))/1000000
    lty<-rep(lty,length(data))
    plot(ages,data[[1]]/1000000,type="n",xlim=xlim,ylim=ylim,xlab=xlab,ylab="Numbers ('000 000s)")
    texts<-c("m","f")
    for(i in 1:length(data)){
      lines(ages,data[[i]]/1000000,type="b",pch=texts[i],lty=lty[i])
    }
    box()
  } else { #Cinitial is not by sex
    if(missing(ages)) {
      ages<-1:length(data[[1]])
      xlab<-"Index"
    } else xlab<-"Age"
    if(missing(xlim)) xlim<-range(ages)
    if(missing(ylim)) ylim<-range(unlist(data))/1000000
    lty<-rep(lty,length(data))
    plot(ages,data[[1]]/1000000,type="n",xlim=xlim,ylim=ylim,xlab=xlab,ylab="Numbers ('000 000s)")
    for(i in 1:length(data)){
      lines(ages,data[[i]]/1000000,lty=lty[i])
    }
    box()
  }
  invisible()
}

"plot.Cinitial.casal.mcmc"<-
function(model,label="",ages,xlim,ylim,plot.type=dplot,lty=c(1,8))
{
  if(label!="") {
    indexM<-as.vector((casal.regexpr(paste("initialization\\[",label,"\\].Cinitial\_male",sep=""),dimnames(model)[[2]])>0))
    indexF<-as.vector((casal.regexpr(paste("initialization\\[",label,"\\].Cinitial\_female",sep=""),dimnames(model)[[2]])>0))
  } else {
    indexM<-as.vector((casal.regexpr(paste("initialization.Cinitial\_male",sep=""),dimnames(model)[[2]])>0))
    indexF<-as.vector((casal.regexpr(paste("initialization.Cinitial\_female",sep=""),dimnames(model)[[2]])>0))
  }
  SelectM<-model[,indexM]/1000000
  SelectF<-model[,indexF]/1000000
  if(missing(ages)) {
    ages<-1:length(indexM[indexM])
    xlab<-"Index"
  } else xlab<-"Age"
  ages<-ages[1:ncol(SelectM)]
  if(missing(ylim)) ylim<-range(c(SelectM,SelectF))
  plot.type(split(SelectM,col(SelectM)),name=as.character(ages),main="",xlab=xlab,ylab="Numbers ('000 000s)",ylim=ylim)
  mtext(side=3,"Male",adj=0,line=0.3)
  plot.type(split(SelectF,col(SelectF)),name=as.character(ages),main="",xlab=xlab,ylab="Numbers ('000 000s)",ylim=ylim)
  mtext(side=3,"Female",adj=0,line=0.3)
  invisible()
}
