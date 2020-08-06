# $Id: generate.MVN.R 4636 2012-03-19 23:48:49Z Dunn $
"generate.MVN"<-
function(file,path="",output.file,sample.size=1,step=1,method="chol")
{
  parbnds <- extract.free.parameters(file=file,path=path,type="bounds")
  Cov<-extract.covariance.matrix(file=file,path=path)
  MPD<-extract.mpd.from.file(file=file,path=path)
  mpdData<-unlist(sapply(MPD,length))
  if(!require(mvtnorm)) stop ("Function 'rmvnorm' is required. This can be found in the 'mvtnorm' package, available on CRAN")
  if(max(abs(Cov - t(Cov))) > 9.9999999999999995e-08) {
    cat("Warning: covariance matrix not symmetric\n")
    cat("Will force symmetry (by averaging matrix with its transpose)\n")
    Cov <- 0.5 * (Cov + t(Cov))
  }
  parlist <- extract.free.parameters(file,path,type="values")
  parlen <- unlist(lapply(parlist, length))
  parvec <- unlist(parlist)
  temp <- unlist(lapply(parlist, function(x) if(length(x) == 1) "" else paste(".", 1:length(x), sep = "")))
  names(parvec) <- paste(rep(names(parlist), parlen), temp, sep = "")
  temp<-lapply(parbnds,function(x) length(x$lower.bound))
  temp<-unlist(lapply(temp, function(x) if(x==1) "" else paste(".", 1:x, sep = "")))
  parbnds<-do.call("rbind",lapply(parbnds,function(x) matrix(unlist(x),ncol=2,byrow=F)))
  parbnds<-as.data.frame(parbnds)
  dimnames(parbnds)[[1]]<-paste(ifelse(temp=="",names(temp),substring(names(temp),1,nchar(names(temp))-nchar(temp)+1)),temp,sep="")
  dimnames(parbnds)[[2]]<-c("lower.bound","upper.bound")
  parbnds <- parbnds[names(parvec),  ]
  nonzero <- !apply(Cov == 0, 2, all)
  randstep <- matrix(0, nrow(Cov), sample.size)
  randstep[nonzero,] <- t(rmvnorm(n=sample.size, mean=rep(0, sum(nonzero)), sigma=Cov[nonzero, nonzero],method=method))
  MVN<-step*(parvec+randstep)
  for(i in 1:sample.size) {
    sel <- MVN[, i] > parbnds[, 2]
    if(any(sel)) MVN[sel, i] <- 0.5 * (parbnds[sel, 2] - parvec[sel])
    sel <- (MVN[, i]) < parbnds[, 1]
    if(any(sel)) MVN[sel, i] <- -0.5 * (parvec[sel] - parbnds[sel,1])
  }
  if(!missing(output.file)) {
    outfile <- casal.make.filename(file=output.file,path=path)
    write(x=MPD[1],file=outfile,ncolumns=1,append = F)
    write.table(x=signif(t(MVN),6), file=outfile, append=TRUE,quote=FALSE, sep=" ",row.names=FALSE,col.names=FALSE)
  }
  invisible(list("header"=MPD[1],"parameters"=parvec,"bounds"=parbnds,"MVN"=t(MVN)))
}
