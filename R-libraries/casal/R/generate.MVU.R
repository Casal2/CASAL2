# $Id: generate.MVU.R 1836 2007-12-04 23:29:28Z adunn $
"generate.MVU"<-
function(file,path="",output.file,sample.size=1)
{
  parbnds <- extract.free.parameters(file=file,path=path,type="bounds")
  MPD<-extract.mpd.from.file(file=file,path=path)
  mpdData<-unlist(sapply(MPD,length))
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
  rand <- runif(n=sample.size*nrow(parbnds), min=parbnds$lower.bound, max=parbnds$upper.bound*1.0000000000001)
  rand <- matrix(rand,nrow=sample.size,byrow=T)
  if(!missing(output.file)) {
    outfile <- casal.make.filename(file=output.file,path=path)
    write(x=MPD[1],file=outfile,ncolumns=1,append = F)
    write.table(x=signif(rand,6), file=outfile, append=TRUE,quote=FALSE, sep=" ",row.names=FALSE,col.names=FALSE)
  }
  invisible(list("header"=MPD[1],"parameters"=parvec,"bounds"=parbnds,"MVU"=rand))
}
