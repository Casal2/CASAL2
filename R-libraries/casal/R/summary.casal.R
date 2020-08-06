# $Id: summary.casal.R 1836 2007-12-04 23:29:28Z adunn $
"summary.casalMPD"<-
function(x)
{
  value<-unlist(c("call"=x$header$call,"date"=x$header$date,"version"=x$header$version,"value"=x$objective.function$value))
  observations<-data.frame("observations"=names(x$fits))
  observations$years<-sapply(x$fits,function(x) paste(x$year[1],"to",rev(x$year)[1]))
  observations$n.years<-sapply(x$fits,function(x) length(x$year))
  free<-names(x$free)
  qs<-unlist(x$quantities$"Nuisance q's")
  B0s<-unlist(x$quantities$"Scalar parameter values")
  Bcur<-unlist(lapply(x$quantities$SSBs,function(y) rev(y)[1]))
  if(is.null(x$parameters.at.bounds))
    value<-list("values"=value,"free.parameters"=free,"observations"=observations,"q"=qs,"Scalar"=B0s,"Bcur"=Bcur)
  else
    value<-list("values"=value,"free.parameters"=free,"parameters.at.bounds"=x$parameters.at.bounds,"observations"=observations,"q"=qs,"Scalar"=B0s,"Bcur"=Bcur)
  return(value)
}

"summary.casalMCMC"<-
function(x)
{
  get.value<-function(x,n) {
    ans<-apply(x,2,function(y) quantile(y,c(0,0.025,0.5,0.975,1)))
    ans<-rbind(ans,"Mean"=apply(x,2,mean))
    dimnames(ans)[[1]]<-c("Min","2.5%","Median","97.5%","Max","Mean")
    ans<-ans[c(1,2,3,6,4,5),,drop=F]
    return(round(ans,n))
  }
  rows<-range(as.numeric(dimnames(x)[[1]]))
  samp<-nrow(x)
  index<-as.vector(casal.regexpr("\\[",dimnames(x)[[2]])-1)
  index<-ifelse(index>0,index,250)
  value<-unique(substring(dimnames(x)[[2]],1,index))
  Names<-dimnames(x)[[2]]
  B0<-x[,casal.regexpr("initialization",Names)>0,drop=F]
  index<-as.vector(casal.regexpr("SSB\\[",Names)>0)
  Years<-as.numeric(substring(Names[index],nchar(Names[index])-4,nchar(Names[index])-1))
  index2<-ifelse(Years==max(Years),T,F)
  Bcur<-x[,index,drop=F][,index2,drop=F]
  res<-list("MCMC.chain"=c(paste("Rows from",rows[1],"to",rows[2]),paste("Number of samples:",samp)))
  res[["Quantities"]]<-value
  ans<-get.value(B0,0)
  ans<-cbind(ans,get.value(Bcur,0))
  ans<-cbind(ans,get.value(Bcur/B0*100,2))
  temp<-dimnames(ans)[[2]]
  temp[(length(temp)/3*2+1):length(temp)]<-paste("%",temp[length(temp)],sep="")
  dimnames(ans)[[2]]<-temp
  res[["Biomass"]]<-ans
  return(res)
}

