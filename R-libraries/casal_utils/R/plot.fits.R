# Plot OBS/fitted
"plot.fits"<-
function(fit,type,model,label,xlim,ylim,xlab,ylab,scale=0.8){
  UseMethod("plot.fits",model)
}

"plot.fits.casal.mpd"<-
function(fit,type,model,label,xlim,ylim,xlab,ylab,scale=0.8)
{
# fit    #data.type: biomass, age, size, CPUE, or anything user defined
# type   #type: fit, residuals, or qqnorm for biomass, CPUE and user defined data
         #    : fit, residuals, qqnorm, or symbols for age and size
# model  #model: the result file read from x.out
# label  #data: data to plot
  data<-model[["fits"]][[label]]
  if(fit=="biomass") {
    if(type=="fit") {
      if(missing(xlim)) xlim<-range(data$year)
      if(missing(ylim)) ylim<-range(c(data$obs/1000,data$fits/1000))
      if(missing(xlab)) xlab<-"Year"
      if(missing(ylab)) ylab<-"Biomass ('000 t)"
      plot(data$year,data$obs/1000,xlim=xlim,ylim=ylim,type="n",xlab=xlab,ylab=ylab)
      lines(data$year,data$obs/1000,type="b",pch="o",lty=8)
      lines(data$year,data$fits/1000,type="b",pch="e",lty=1)
    } else if(type=="residuals") {
      if(missing(xlim)) xlim<-range(data$fits/1000)
      if(missing(ylim)) ylim<-range(data$normalised.resids)
      if(missing(xlab)) xlab<-"Expected biomass ('000 t)"
      if(missing(ylab)) ylab<-"Normalised residuals"
      plot(data$fits/1000,data$normalised.resids,xlim=xlim,ylim=ylim,type="n",xlab=xlab,ylab=ylab)
      text(data$fits/1000,data$normalised.resids,as.character(data$year),cex=par()$cex)
      abline(h=0)
    } else if(type=="qqnorm") {
      ans<-qqnorm(data$normalised.resids,plot.it=F)
      if(missing(xlim)) xlim<-range(c(ans$x,ans$y))
      if(missing(ylim)) ylim<-range(c(ans$x,ans$y))
      if(missing(xlab)) xlab<-"Quantiles of Standard Normal"
      if(missing(ylab)) ylab<-"Normalised residuals"
      qqDist(data$normalised.resids,standardise=T,xlim=xlim,ylim=ylim,xlab=xlab,ylab=ylab,pch=16)
#      qqline(data$normalised.resids)
#      abline(1,lty=2)
    }
  } else if(fit=="age" || fit=="length") {
    if(fit=="age") XLAB<-"Age"
    if(fit=="length") XLAB<-"Length"
    index<-as.vector((casal.regexpr("M",names(data$fits))>0))
    Mage<-as.numeric(substring(names(data$fits[index]),2))
    Fage<-as.numeric(substring(names(data$fits[!index]),2))
    Mobs<-as.vector(unlist(t(data$obs[,index])))
    Fobs<-as.vector(unlist(t(data$obs[,!index])))
    Mfit<-as.vector(unlist(t(data$fits[,index])))
    Ffit<-as.vector(unlist(t(data$fits[,!index])))
#   if(missing(xlim)) xlim<-range(c(range(data$fits*.96),range(data$fits*1.04)))
    if(type=="fit") {
      if(missing(xlim)) xlim<-range(c(Mage,Fage))
      if(missing(ylim)) ylim<-range(cbind(data$obs,data$fits))
      n<-length(Mfit)/length(Mage)
      for (i in 1:n){
        if(missing(ylab)) ylab<-"Proportion"
        plot(Mage,Mobs[((1+length(Mage)*(i-1))):(length(Mage)*i)],type="n",xlab=XLAB,ylab=ylab,ylim=ylim,xlim=xlim) #,log="y")
        lines(Mage,Mobs[((1+length(Mage)*(i-1))):(length(Mage)*i)],type="b",pch="o",lty=8)
        lines(Mage,Mfit[((1+length(Mage)*(i-1))):(length(Mage)*i)],type="b",pch="e",lty=1)
        mtext(side=3,paste("Male ",data$year[i]),adj=0,line=0.3)
        plot(Fage,Fobs[((1+length(Fage)*(i-1))):(length(Fage)*i)],type="n",xlab=XLAB,ylab=ylab,ylim=ylim,xlim=xlim) #,log="y")
        lines(Fage,Fobs[((1+length(Fage)*(i-1))):(length(Fage)*i)],type="b",pch="o",lty=8)
        lines(Fage,Ffit[((1+length(Fage)*(i-1))):(length(Fage)*i)],type="b",pch="e",lty=1)
        mtext(side=3,paste("Female",data$year[i]),adj=0,line=0.3)
      }
    }
    if(type=="residuals") {
      Mres<-as.vector(unlist(t(data$normalised.resids[,index])))
      Fres<-as.vector(unlist(t(data$normalised.resids[,!index])))
      if(missing(xlim)) xlim<-range(c(Mfit,Ffit))
      if(missing(ylim)) ylim<-range(c(Mres,Fres))
      if(missing(xlab)) xlab<-"Expected proportion"
      if(missing(ylab)) ylab<-"Normalised residuals"
      plot(Mfit,Mres,type="n",xlab=xlab,ylab=ylab,ylim=ylim,xlim=xlim,log="x")
      text(Mfit,Mres,as.character(rep(Mage,ncol(data$fits))),cex=par()$cex)
      mtext(side=3,"Male",adj=0,line=0.3)
      abline(h=0)
      plot(Ffit,Fres,type="n",xlab=xlab,ylab=ylab,ylim=ylim,xlim=xlim,log="x")
      text(Ffit,Fres,as.character(rep(Fage,ncol(data$fits))),cex=par()$cex)
      mtext(side=3,"Female",adj=0,line=0.3)
      abline(h=0)
    } else if(type=="qqnorm") {
      Mres<-as.vector(unlist(t(data$normalised.resids[,index])))
      Fres<-as.vector(unlist(t(data$normalised.resids[,!index])))
      ans<-qqnorm(Mres,plot.it=F)
      if(missing(xlim)) xlim<-range(c(ans$x,ans$y))
      if(missing(ylim)) ylim<-range(c(ans$x,ans$y))
      if(missing(xlab)) xlab<-"Quantiles of Standard Normal"
      if(missing(ylab)) ylab<-"Normalised residuals"
      qqDist(Mres,standardise=T,xlim=xlim,ylim=ylim,xlab=xlab,ylab=ylab,pch=16)
#      qqline(Mres)
#      abline(1,lty=2)
      mtext(side=3,"Male",adj=0,line=0.3)
      ans<-qqnorm(Fres,plot.it=F)
      if(missing(xlim)) xlim<-range(c(ans$x,ans$y))
      if(missing(ylim)) ylim<-range(c(ans$x,ans$y))
      qqDist(Fres,standardise=T,xlim=xlim,ylim=ylim,xlab=xlab,ylab=ylab,pch=16)
#      qqline(Fres)
#      abline(1,lty=2)
      mtext(side=3,"Female",adj=0,line=0.3)
    }
    if(type=="symbols") {
      circles<-function(x,y,z,scale=1) {for(i in 1:length(x)) points(x[i],y[i],pch=ifelse(z[i]>0,16,1),cex=max(c(sqrt(abs(z[i]))*par()$cex*scale,0.05)))}
      Mage<-rep(Mage,nrow(data$fits))
      Fage<-rep(Fage,nrow(data$fits))
      if(missing(xlim)) xlim<-range(c(Mage,Fage))
      if(missing(ylim)) ylim<-c(min(data$year)-0.5,max(data$year)+0.5)
      plot(xlim,ylim,type="n",xlab=XLAB,ylab="",axes=F,xlim=xlim,ylim=ylim)
      axis(side=1)
      axis(side=2,at=data$year,adj=1)
      box()
      y<-rep(data$year,rep(length(index[index]),length(data$year)))
      circles(x=Mage,y=y,z=Mres,scale=scale)
      mtext(side=3,"Male",adj=0,line=0.3)
      axis(side=1,at=unique(c(Mage,Fage)),labels=F,tick=T)
      axis(side=2,at=unique(data$year),labels=F,tick=T)
      plot(xlim,ylim,type="n",xlab=XLAB,ylab="",axes=F)
      axis(side=1)
      axis(side=2,at=data$year,adj=1)
      box()
      y<-rep(data$year,rep(length(index[!index]),length(data$year)))
      circles(Fage,y,Fres,scale)
      mtext(side=3,"Female",adj=0,line=0.3)
      axis(side=1,at=unique(c(Mage,Fage)),labels=F,tick=T)
      axis(side=2,at=unique(data$year),labels=F,tick=T)
    }
  }
  else if(fit=="CPUE") {
    if(type=="fit") {
      if(missing(xlim)) xlim<-range(data$year)
      if(missing(ylim)) ylim<-range(c(data$obs,data$fits))
      if(missing(ylab)) ylab<-"CPUE"
      plot(data$year,data$obs,xlim=xlim,ylim=ylim,type="n",xlab="Year",ylab=ylab)
      lines(data$year,data$obs,type="b",pch="o",lty=8)
      lines(data$year,data$fits,type="b",pch="e",lty=1)
    } else if(type=="residuals") {
      if(missing(xlim)) xlim<-range(data$fits)
      if(missing(ylim)) ylim<-range(data$normalised.resids)
      if(missing(xlab)) xlab<-"Expected CPUE"
      if(missing(ylab)) ylab<-"Normalised residuals"
      plot(data$fits,data$normalised.resids,xlim=xlim,ylim=ylim,type="n",xlab=xlab,ylab=ylab)
      text(data$fits,data$normalised.resids,as.character(data$year),cex=par()$cex)
      abline(h=0)
    } else if(type=="qqnorm") {
      ans<-qqnorm(data$normalised.resids,plot.it=F)
      if(missing(xlim)) xlim<-range(c(ans$x,ans$y))
      if(missing(ylim)) ylim<-range(c(ans$x,ans$y))
      if(missing(xlab)) xlab<-"Quantiles of Standard Normal"
      if(missing(ylab)) ylab<-"Normalised residuals"
      qqDist(data$normalised.resids,standardise=T,xlim=xlim,ylim=ylim,ylab=ylab,xlab=xlab,pch=16)
#      qqline(data$normalised.resids)
#      abline(1,lty=2)
    }
  }
  #plot for user defined data
  else if(fit!="") {
    if(type=="fit") {
      if(missing(xlim)) xlim<-range(data$year)
      if(missing(ylim)) ylim<-range(c(data$obs,data$fits))
      plot(data$year,data$obs,xlim=xlim,ylim=ylim,type="n",xlab="Year",ylab=data.type)
      lines(data$year,data$obs,type="b",pch="o",lty=8)
      lines(data$year,data$fits,type="b",pch="e",lty=1)
    } else if(type=="residuals") {
      if(missing(xlim)) xlim<-range(data$fits)
      if(missing(ylim)) ylim<-range(data$normalised.resids)
      plot(data$fits,data$normalised.resids,xlim=xlim,ylim=ylim,type="n",xlab=paste("Expected",data.type),ylab="Normalised residuals")
      text(data$fits,data$normalised.resids,as.character(data$year),cex=par()$cex)
      abline(h=0)
    } else if(type=="qqnorm") {
      ans<-qqnorm(data$normalised.resids,plot.it=F)
      if(missing(xlim)) xlim<-range(c(ans$x,ans$y))
      if(missing(ylim)) ylim<-range(c(ans$x,ans$y))
      qqDist(data$normalised.resids,standardise=T,xlim=xlim,ylim=ylim,ylab="Normalised residuals",xlab="Quantiles of Standard Normal",pch=16)
#      qqline(data$normalised.resids)
#      abline(1,lty=2)
    }
  }
  else print("Plot type unknown")
  invisible()
}

"plot.fits.casal.mcmc"<-
function(fit,type,model,label,xlim,ylim,xlab,ylab,scale=0.8)
{
  print("plot.fits is not yet implemented for mcmc chains")
  invisible()
}
