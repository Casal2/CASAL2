#' @title plot.prior plot that could be assumed in a Casal2 model
#'
#' @description
#' A plotting a prior, to visulise
#'
#' @author A.Dunn
#' @return plot of a priors
#' @rdname plot.prior
#' @export plot.prior
"plot.prior"<-
function(type="lognormal",mu,sd,cv,A=0,B=1,bounds,xlim,label=F,xlab="x",ylab="Density",logx=F,add=F,dump=F,n=201,...)
{
  # type define the type of prior to plot. mu, sd, cv are the parameters of the
  # priors (not all parameters are valid for all priors). bounds are the bounds
  # on the prior. xlim is the xlim to plot. label is a T/F to display the name of
  # the prior. logx=T/F to plot the xaxis in log-space.
  if(missing(n))
    n<-201
  if(missing(xlim) & !missing(bounds))
    xlim <- bounds
  else if(missing(bounds) &!missing(xlim))
    bounds <- xlim
  else if(missing(bounds) &missing(xlim))
    stop("Specify the bounds and/or xlim")
  if(logx & (min(xlim)<=0 | min(bounds)<=0))
    stop("With 'logx' option, the lower bound/xlim must be a postive number")
  Allowed<-c("lognormal","normal-by-stdev","uniform","uniform-log","normal-log","beta")
  type<-Allowed[as.numeric(pmatch(casefold(type),casefold(Allowed),nomatch=NA))]
  if(type=="")
    stop(paste("The prior was not found. Available priors are:\n",paste(Allowed,collapse=", ")))
  else
    cat(paste("Plotting ",type," prior\n",sep=""))
  if(logx)
    p<-exp(seq(log(bounds[1]),log(bounds[2]),length=n))
  else
    p<-seq(bounds[1],bounds[2],length=n)
  if(type=="lognormal") {
    sigma<-sqrt(log(1+(cv^2)))
    res<-exp(-(log(p)+0.5*((log(p/mu)/sigma + sigma/2)^2)))
  } else if(type=="normal-by-stdev") {
    res<-exp(-(0.5*(((p-mu)/sd)^2)))
  } else if(type=="uniform") {
    if(logx)
      res<-p
    else
      res<-rep(1,length(p))
  } else  if(type=="uniform-log") {
    if(logx)
      res<- rep(1,length(p))
    else
      res<-exp(-log(p))
  } else if (type=="normal-log") {
    res<-exp(-(log(p)+0.5*(((log(p)-mu)/sd)^2)))
  } else if (type=="beta") {
    new.mu<-(mu-A)/(B-A)
    new.t<-(((mu-A)*(B-mu))/(sd^2)) - 1
    if(new.t <= 0)
      stop("Standard deviation too large");
    if (min(bounds,na.rm=T) < A || max(bounds,na.rm=T) > B)
      stop("Bad bounds on Beta prior")
    Bm<-new.t*new.mu
    Bn<-new.t*(1-new.mu)
    res<-(1-Bm)*log(p-A)+(1-Bn)*log(B-p)
    res<-exp(-res)
  }
  res<-c(0,res/max(res, na.rm = T),0)
  p<-c(bounds[1],p,bounds[2])
  res<-res/casal.area(list("x"=p,"y"=res))
  ylim<-c(min(res, na.rm = T),max(res, na.rm = T)*1.04)
  if(logx) {
    xlab<-paste("log(",xlab,")",sep="")
    if(!add) {
      plot(log(p),res,type="n",xlab=xlab,ylab="",xlim=log(xlim),axes=F,ylim=ylim)
       axis(side=1)
       mtext(side=2,line=0.5,text=ylab)
       if(label)
         mtext(side=3,type,adj=0,line=0.3)
       box()
     }
    lines(log(p),res,type="l",...)
  } else {
    if(!add) {
      plot(p,res,type="n",xlab=xlab,ylab="",xlim=xlim,axes=F,ylim=ylim)
      axis(side=1)
      mtext(side=2,line=0.5,text=ylab)
      if(label)
        mtext(side=3,type,adj=0,line=0.3)
      box()
    }
    lines(p,res,type="l",...)
  }
  if(dump) return(data.frame("x"=p,"y"=res))
  else invisible()
}
