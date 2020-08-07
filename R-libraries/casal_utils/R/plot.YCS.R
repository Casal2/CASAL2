# Plot OBS/fitted
"plot.YCS"<-
function(model,label="",year,xlim,ylim,ylab,plot.type=dplot,variable="true\_YCS",...){
  UseMethod("plot.YCS",model)
}

"plot.YCS.casal.mpd"<-
function(model,label="",year,xlim,ylim,ylab,variable="true\_YCS",...)
{
  data<-model$quantities
  data<-eval(parse(text=paste("data$\"",variable,"\"",sep="")))
  if(length(data)>2) {
    year<-data$year
    data<-as.vector(unlist(data[as.vector(casal.regexpr(label,names(data))>0)]))
  } else {
    year<-data$year
    data<-eval(parse(text=paste("data$\"",variable,"\"",sep="")))
  }
  if(missing(xlim)) xlim<-range(year)
  if(missing(ylim)) ylim<-range(c(0,data))
  if(missing(ylab)) ylab<-variable
  plot(year,data,xlim=xlim,ylim=ylim,type="n",xlab="Year",ylab=ylab)
  lines(year,data,...)
  invisible()
}

"plot.YCS.casal.mcmc"<-
function(model, label = "", year, xlim, ylim, ylab, plot.type = dplot, variable = "true_YCS", ...)
{
  if(label != "") label <- paste(variable, "\\[", label, "\\]", sep = "")
  else label <- variable
  index <- as.vector((casal.regexpr(label, dimnames(model)[[2]]) > 0))
  YCS <- model[, index]
  year<-as.numeric(substring(dimnames(YCS)[[2]],nchar(label)+2,nchar(dimnames(YCS)[[2]])-1))
  xlab <- "Year"
  if(missing(xlim)) xlim <- range(year)
  if(missing(ylim)) ylim <- range(c(0, YCS))
  if(missing(ylab)) ylab <- variable
  centers<-plot.type(split(YCS, col(YCS)), name = F, main = "", xlab = xlab, ylab = ylab, ylim = ylim, ...)
  index<-ifelse(year %% 5 ==0,T,F)
  axis(side=1,at=centers[index],labels=as.character(year)[index])
  axis(side=1,at=centers,labels=F,tck=ifelse(is.na(par("tck")),-0.02,0.5*par("tck")))
  invisible()
}
