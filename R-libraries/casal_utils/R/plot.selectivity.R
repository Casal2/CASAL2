# Plot OBS/fitted
"plot.selectivity"<-
function(type,model,label="",ages,length.class,xlim,ylim,lty=c(1,8),plot.type=dplot,...) {
  UseMethod("plot.selectivity",model)
}

"plot.selectivity.casalMPD"<-
function(type,model,label="",ages,length.class,xlim,ylim,lty=c(1,8),...)
{
  Names<-names(model[["quantities"]][["Ogive parameter values"]])
  data<-paste("selectivity\\[",label,"\\]",sep="")
  index<-as.vector(casal.regexpr(data,Names)>0)
  if(length(index[index])<1) stop(paste("Valid labels are\n",paste(Names,collapse="\n ")))
  data<-model[["quantities"]][["Ogive parameter values"]][index]
  print(data)

  if(type=="age.by.sex") {
    if(missing(ages)) {
      ages<-1:length(data[[1]])
      xlab<-"Index"
    } else xlab<-"Age"
    if(missing(xlim)) xlim<-range(ages)
    if(missing(ylim)) ylim<-range(unlist(data))
    lty<-rep(lty,length(data))
    plot(ages,data[[1]],type="n",xlim=xlim,ylim=ylim,xlab=xlab,ylab="Selectivity")
    for(i in 1:length(data)){
      lines(ages,data[[i]],lty=lty[i],...)
    }
    box()
  } else  if(type=="length") {
    if(missing(length.class)) {
      length.class<-1:length(data[[1]])
      xlab<-"Index"
    } else xlab<-"Length"
    if(missing(xlim)) xlim<-range(length.class)
    if(missing(ylim)) ylim<-range(unlist(data))
    lty<-rep(lty,length(data))
    plot(length.class,data[[1]],type="n",xlim=xlim,ylim=ylim,xlab=xlab,ylab="Selectivity")
    for(i in 1:length(data)){
      lines(length.class,data[[i]],lty=lty[i],...)
    }
    box()
  } else print("Plot type unknown")
  invisible()
}

"plot.selectivity.casalMCMC"<-
function(type, model, label = "", xlim, ylim, plot.type = dplot, lty = c(1, 8), ...)
{
  if(type == "age.by.sex") {
    indexM <- as.vector((casal.regexpr(paste("selectivity\\[", label, "\\].male", sep = ""), dimnames(model)[[2]]) > 0))
    indexF <- as.vector((casal.regexpr(paste("selectivity\\[", label, "\\].female", sep = ""), dimnames(model)[[2]]) > 0))
    SelectM <- model[, indexM]
    SelectF <- model[, indexF]
    label<-paste("selectivity\\[", label, "\\].male",sep="")
    Mage<-as.numeric(substring(dimnames(SelectM)[[2]],nchar(label),nchar(dimnames(SelectM)[[2]])-1))
    label<-paste("selectivity\\[", label, "\\].female",sep="")
    Fage<-as.numeric(substring(dimnames(SelectF)[[2]],nchar(label),nchar(dimnames(SelectF)[[2]])-1))
    ages<-sort(unique(c(Mage,Fage)))
    xlab <- "Age"
    if(missing(ylim))ylim <- range(c(SelectM, SelectF))
    centers<-plot.type(split(SelectM, col(SelectM)), name =F, main = "", xlab = xlab, ylab = "Selectivity", ylim = ylim, ...)
    mtext(side = 3, "Male", adj = 0, line = 0.3)
    index<-ifelse(ages %% 5 ==0,T,F)
    axis(side=1,at=centers[index],labels=as.character(ages)[index])
    axis(side=1,at=centers,labels=F,tck=ifelse(is.na(par("tck")),-0.02,0.5*par("tck")))
    centers<- plot.type(split(SelectF, col(SelectF)), name = F, main = "", xlab = xlab, ylab = "Selectivity", ylim = ylim, ...)
    axis(side=1,at=centers[index],labels=as.character(ages)[index])
    axis(side=1,at=centers,labels=F,tck=ifelse(is.na(par("tck")),-0.02,0.5*par("tck")))
    mtext(side = 3, "Female", adj = 0, line = 0.3)
  }
  else if(type == "length") {
    indexM <- as.vector((casal.regexpr(paste("selectivity\\[", label, "\\]", sep = ""), dimnames(model)[[2]]) > 0))
    SelectM <- model[, indexM]
    if(missing(length.class)) {
      length.class <- 1:length(data[[1]])
      xlab <- "Index"
    }
    else xlab <- "Length"
    if(missing(xlim)) xlim <- range(length.class)
    if(missing(ylim)) ylim <- range(unlist(SelectM))
    centers <- plot.type(split(SelectM, col(SelectM)), name = as.character(length.class), main = "", xlab = xlab, ylab = "Selectivity", ylim = ylim, ...)
    box()
  }
  else {
    print("Plot type unknown")
    centers <- NULL
  }
  invisible(centers)
}
