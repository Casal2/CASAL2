# $Id: extract.mpd.R 1836 2007-12-04 23:29:28Z adunn $
"extract.mpd"<-
function(file,path="")
{
  set.class <- function(object,new.class){
  # use in the form
  #  object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class,attributes(object)$class[attributes(object)$class!=new.class])
    object
  }
  if(missing(path)) path<-""
  header<-extract.header(file=file,path=path)
  objective.function<-extract.objective.function(file=file,path=path)
  parameters.at.bounds<-extract.parameters.at.bounds(file=file,path=path)
  fits<-extract.fits(file=file,path=path)
  free<-extract.free.parameters(file=file,path=path,type="values")
  bounds<-extract.free.parameters(file=file,path=path,type="bounds")
  quantities<-extract.quantities(file=file,path=path)
  res<-list(header=header,objective.function=objective.function,parameters.at.bounds=parameters.at.bounds,fits=fits,free=free,bounds=bounds,quantities=quantities)
  res<-set.class(res,"casalMPD")
  return(res)
}
