# $Id: plot.SSBs.R 1854 2008-01-22 22:47:44Z adunn $
"plot.SSBs"<-
function(model,type="number",label="",xlim,ylim,xlab,ylab,plot.type=casal.dplot,plot.it=T,...){
  UseMethod("plot.SSBs",model)
}

"plot.SSBs.casalMPD"<-
function(model,type="number",label="",xlim,ylim,xlab,ylab,plot.it=T,...)
{
  data<-model[["quantities"]][["SSBs"]]
  if(label!="") SSB<-data[[label]]
  else SSB<-data$SSB
  if(type=="percent") {
    B0<-model$quantities$"Scalar parameter values"
    if(label!="") B0<-as.vector(unlist(B0[as.vector((casal.regexpr(paste(label,"\\].B0",sep=""),names(B0))>0))]))
    else B0 <- as.vector(unlist(B0[as.vector((casal.regexpr(paste("initialization.B0", sep = ""), names(B0)) > 0))]))
    SSB<-SSB/B0*100
    if(plot.it==F) {
      return(data.frame("year"=data$year,"SSB"=SSB))
    }
    if(missing(xlim)) xlim<-range(data$year)
    if(missing(ylim)) ylim<-range(c(0,SSB,101))
    if(missing(xlab)) xlab<-"Year"
    if(missing(ylab)) ylab<-"Biomass (%B0)"
    plot(data$year,SSB,xlim=xlim,ylim=ylim,type="n",xlab=xlab,ylab=ylab,...)
    lines(data$year,SSB,type="l")
  } else {
    if(plot.it==F) {
      return(data.frame("year"=data$year,"SSB"=SSB))
    } else {
      if(missing(xlim)) xlim<-range(data$year)
      if(missing(ylim)) ylim<-range(c(0,SSB/1000))
      if(missing(xlab)) xlab<-"Year"
      if(missing(ylab)) ylab<-"Biomass ('000 t)"
      plot(data$year,SSB/1000,xlim=xlim,ylim=ylim,type="n",xlab=xlab,ylab=ylab,...)
      lines(data$year,SSB/1000,type="l")
      invisible(data.frame("year"=data$year,"SSB"=SSB))
    }
  }
}

"plot.SSBs.casalMCMC"<-
function(model,type="number",label="",xlim,ylim,xlab,ylab,plot.type=casal.dplot,plot.it=T,...)
{
  index1 <- as.vector((casal.regexpr(paste("SSB\\[", sep = ""), dimnames(model)[[2]]) > 0))
  if(label != "") index2 <- as.vector((casal.regexpr(label, dimnames(model)[[2]]) > 0))
  else index2 <- T
  if(all((index1 & index2)==FALSE)) stop("Unable to find any SSB columns with the appropriate label\n")
  year <- dimnames(model)[[2]][index1 & index2]
  year <- as.numeric(substring(year, nchar(year) - 4, nchar(year) - 1))
  SSB <- model[, index1 & index2]/1000
  if(type == "percent") {
    if(label != "") B0 <- as.vector((casal.regexpr(paste("B0\\[",label, "\\]", sep = ""), dimnames(model)[[2]]) > 0))
    else B0 <- as.vector((casal.regexpr("initialization.B0", dimnames(model)[[2]]) > 0))
    B0 <- model[, B0]/1000
    SSB <- SSB/B0 * 100
    if(plot.it==FALSE) {
      return(cbind(B0,SSB))
    }
    if(missing(ylim)) ylim <- range(c(0, SSB, 101))
    if(missing(xlab)) xlab <- "Year"
    if(missing(ylab)) ylab <- "Biomass (%B0)"
    centers <- plot.type(split(SSB, col(as.matrix(SSB))), name=F, main = "", xlab = xlab, ylab = ylab, ylim = ylim)
    index<-ifelse(year %% 5 ==0,T,F)
    axis(side=1,at=centers[index],labels=as.character(year)[index])
    axis(side=1,at=centers,labels=F,tck=ifelse(is.na(par("tck")),-0.02,0.5*par("tck")))
  } else {
    if(plot.it==FALSE) {
      if(label != "") B0 <- as.vector((casal.regexpr(paste("B0\\[",label, "\\]", sep = ""), dimnames(model)[[2]]) > 0))
      else B0 <- as.vector((casal.regexpr("initialization.B0", dimnames(model)[[2]]) > 0))
      B0 <- model[, B0]/1000
      return(cbind(B0,SSB))
    }
    if(missing(ylim)) ylim <- range(0, SSB)
    if(missing(xlab)) xlab <- "Year"
    if(missing(ylab))ylab <- "Biomass ('000 t)"
    centers <- plot.type(split(SSB, col(as.matrix(SSB))), name = F, main = "", xlab = xlab, ylab = ylab, ylim = ylim)
    index<-ifelse(year %% 5 ==0,T,F)
    axis(side=1,at=centers[index],labels=as.character(year)[index])
    axis(side=1,at=centers,labels=F,tck=ifelse(is.na(par("tck")),-0.02,0.5*par("tck")))
  }
  invisible(centers)
}
